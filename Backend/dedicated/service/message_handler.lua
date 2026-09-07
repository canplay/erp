local moon = require("moon")
local websocket = require("moon.http.websocket")

local chat = require("service.chat")
local user = require("service.user")
local friend = require("service.friend")
local group = require("service.group")
local like = require("service.like")
local utils = require("service.utils")

local MessageHandler = {}

function MessageHandler:new(users, sessions, generate_session_id_func, secure_auth)
    local obj = {
        users = users,
        sessions = sessions,
        generate_session_id = generate_session_id_func,
        secure_auth = secure_auth,  -- 安全认证模块
        handlers = {} -- 注册的消息处理器
    }
    setmetatable(obj, self)
    self.__index = self

    -- 注册默认处理器
    obj:register_handler("system", obj.handle_system_message)
    obj:register_handler("public", obj.handle_public_message)
    obj:register_handler("private", obj.handle_private_message)
    obj:register_handler("user", obj.handle_user_message)
    obj:register_handler("friend", obj.handle_friend_message)
    obj:register_handler("group", obj.handle_group_message)
    obj:register_handler("like", obj.handle_like_message)
    obj:register_handler("history", obj.handle_history_message)

    return obj
end

--- 注册消息处理器
-- @param msg_type 消息类型
-- @param handler 处理函数
function MessageHandler:register_handler(msg_type, handler)
    self.handlers[msg_type] = handler
end

--- 发送响应消息
-- @param fd WebSocket连接标识
-- @param response 响应内容
function MessageHandler:send_response(fd, response)
    utils.send_message(fd, response)
end

--- 检查用户是否登录（增强版）
-- @param fd WebSocket连接标识
-- @param token 安全令牌（可选）
function MessageHandler:check_login(fd, token)
    -- 优先使用安全令牌验证
    if token and self.secure_auth then
        local valid, user_info = self.secure_auth.validate_token(fd, token)
        if valid then
            -- 更新用户信息
            if self.users[fd] then
                self.users[fd].username = user_info.username
            end
            return true
        else
            self:send_response(fd, {
                type = "error",
                message = user_info  -- user_info here contains error message
            })
            return false
        end
    end
    
    -- 回退到原有的连接级验证
    local user_info = self.users[fd]
    if not user_info or not user_info.username then
        self:send_response(fd, {
            type = "error",
            message = "请先登录"
        })
        return false
    end
    return true
end

--- 处理系统消息
function MessageHandler:handle_system_message(fd, msg_dec)
    if msg_dec.data == "close" then
        websocket.close(fd)
    elseif msg_dec.data == "heartbeat" then
        if self.users[fd] then
            self.users[fd].last_heartbeat = moon.time()
            self:send_response(fd, {
                type = "system",
                data = "heartbeat_ack"
            })
        end
    end
end

--- 处理公共消息
function MessageHandler:handle_public_message(fd, msg_dec)
    if not self:check_login(fd, msg_dec.token) then return end
    chat.process_public_message(self.users, msg_dec)
end

--- 处理私聊消息
function MessageHandler:handle_private_message(fd, msg_dec)
    if not self:check_login(fd, msg_dec.token) then return end
    chat.process_private_message(self.users, fd, msg_dec)
end

--- 处理群组消息
function MessageHandler:handle_group_message(fd, msg_dec)
    if not self:check_login(fd, msg_dec.token) then return end
    
    -- 区分群组操作和群组消息
    if msg_dec.action then
        self:handle_group_operations(fd, msg_dec)
    else
        -- 处理群组聊天消息
        chat.process_group_message(self.users, msg_dec)
    end
end

--- 处理用户相关消息
function MessageHandler:handle_user_message(fd, msg_dec)
    if msg_dec.action == "register" then
        user.register(msg_dec.username, msg_dec.password, msg_dec.profile, function(success, result)
            self:send_response(fd, {
                type = "user_response",
                action = "register",
                success = success,
                message = result
            })
        end)
    elseif msg_dec.action == "login" then
        user.login(msg_dec.username, msg_dec.password, function(success, result)
            if success then
                local session_id = self.generate_session_id()
                self.sessions[session_id] = msg_dec.username
                self.users[fd] = {
                    username = msg_dec.username,
                    user_id = result.id,
                    session_id = session_id,
                    last_heartbeat = moon.time(),
                    ip_address = self.users[fd] and self.users[fd].ip_address or "unknown"
                }
                
                -- 生成安全令牌
                local token = self.secure_auth and self.secure_auth.login_user(
                    fd, 
                    msg_dec.username, 
                    self.users[fd].ip_address
                )
                
                self:send_response(fd, {
                    type = "user_response",
                    action = "login",
                    success = success,
                    message = result.message or "登录成功",
                    session_id = session_id,
                    token = token  -- 返回安全令牌
                })
            else
                self:send_response(fd, {
                    type = "user_response",
                    action = "login",
                    success = success,
                    message = result
                })
            end
        end)
    elseif msg_dec.action == "logout" then
        if not self:check_login(fd, msg_dec.token) then return end
        local user_info = self.users[fd]
        if user_info and user_info.session_id then
            self.sessions[user_info.session_id] = nil
            
            -- 清除安全令牌
            if msg_dec.token and self.secure_auth then
                self.secure_auth.logout_user(msg_dec.token)
            end
            
            self.users[fd] = nil
        end
        self:send_response(fd, {
            type = "user_response",
            action = "logout",
            success = true,
            message = "登出成功"
        })
    elseif msg_dec.action == "profile" then
        if not self:check_login(fd, msg_dec.token) then return end
        user.get_profile(msg_dec.username, function(success, result)
            self:send_response(fd, {
                type = "user_response",
                action = "profile",
                success = success,
                data = result
            })
        end)
    else
        self:send_response(fd, {
            type = "user_response",
            action = "unknown",
            success = false,
            message = "未知的用户操作"
        })
    end
end

--- 处理好友相关消息
function MessageHandler:handle_friend_message(fd, msg_dec)
    if not self:check_login(fd, msg_dec.token) then return end
    
    -- 根据action字段分发到不同的处理函数
    if msg_dec.action == "add_request" then
        friend.handle_add_friend_request(fd, msg_dec, self.users, self.sessions)
    elseif msg_dec.action == "request_response" then
        friend.handle_friend_request_response(fd, msg_dec, self.users, self.sessions)
    elseif msg_dec.action == "remove" then
        friend.handle_remove_friend(fd, msg_dec, self.users, self.sessions)
    elseif msg_dec.action == "list" then
        friend.handle_get_friends_list(fd, msg_dec, self.users, self.sessions)
    elseif msg_dec.action == "requests" then
        friend.handle_get_friend_requests(fd, msg_dec, self.users, self.sessions)
    elseif msg_dec.action == "search" then
        friend.handle_search_users(fd, msg_dec, self.users, self.sessions)
    else
        self:send_response(fd, {
            type = "error",
            message = "未知的好友操作: " .. tostring(msg_dec.action)
        })
    end
end

--- 处理群组操作消息
function MessageHandler:handle_group_operations(fd, msg_dec)
    if not self:check_login(fd, msg_dec.token) then return end
    
    -- 根据action字段分发到不同的处理函数
    if msg_dec.action == "create" then
        group.handle_create_group(fd, msg_dec, self.users, self.sessions)
    elseif msg_dec.action == "join" then
        group.handle_join_group(fd, msg_dec, self.users, self.sessions)
    elseif msg_dec.action == "leave" then
        group.handle_leave_group(fd, msg_dec, self.users, self.sessions)
    elseif msg_dec.action == "list" then
        group.handle_get_groups(fd, msg_dec, self.users, self.sessions)
    elseif msg_dec.action == "members" then
        group.handle_get_group_members(fd, msg_dec, self.users, self.sessions)
    elseif msg_dec.action == "update" then
        group.handle_update_group(fd, msg_dec, self.users, self.sessions)
    elseif msg_dec.action == "message" then
        group.handle_group_message(fd, msg_dec, self.users, self.sessions)
    elseif msg_dec.action == "transfer" then
        group.handle_transfer_ownership(fd, msg_dec, self.users, self.sessions)
    else
        self:send_response(fd, {
            type = "error",
            message = "未知的群组操作: " .. tostring(msg_dec.action)
        })
    end
end

--- 处理点赞相关消息
function MessageHandler:handle_like_message(fd, msg_dec)
    if not self:check_login(fd, msg_dec.token) then return end
    
    -- 根据action字段分发到不同的处理函数
    if msg_dec.action == "like" then
        like.handle_like_message(fd, msg_dec, self.users, self.sessions)
    elseif msg_dec.action == "unlike" then
        like.handle_unlike_message(fd, msg_dec, self.users, self.sessions)
    elseif msg_dec.action == "list" then
        like.handle_get_likes(fd, msg_dec, self.users, self.sessions)
    elseif msg_dec.action == "user_likes" then
        like.handle_get_user_likes(fd, msg_dec, self.users, self.sessions)
    else
        self:send_response(fd, {
            type = "error",
            message = "未知的点赞操作: " .. tostring(msg_dec.action)
        })
    end
end

--- 处理历史消息查询
function MessageHandler:handle_history_message(fd, msg_dec)
    if not self:check_login(fd, msg_dec.token) then return end
    
    -- 这里可以添加历史消息处理逻辑
    self:send_response(fd, {
        type = "history_response",
        success = true,
        data = {}
    })
end

--- 主处理函数
function MessageHandler:handle(fd, msg_dec)
    local handler = self.handlers[msg_dec.type]
    if handler then
        handler(self, fd, msg_dec)
    else
        self:send_response(fd, {
            type = "error",
            message = "未知的消息类型: " .. tostring(msg_dec.type)
        })
    end
end

return MessageHandler