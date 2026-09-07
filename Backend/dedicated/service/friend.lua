-- 定义 friend 模块
local friend = {}
local moon = require("moon")
local utils = require("service.utils")
local secure_auth = require("service.secure_auth")
local db = require("service.db")

-- 通用的用户验证和参数检查函数
local function check_user_and_params(fd, msg_dec, required_params)
    local token = msg_dec.token
    
    -- 验证输入
    local valid, err = utils.validate_params(msg_dec, required_params)
    if not valid then
        utils.send_message(fd, {
            type = "friend",
            action = msg_dec.action or "unknown",
            success = false,
            message = err
        })
        return nil, nil, nil
    end

    -- 验证用户身份
    local is_valid, username_or_err = utils.check_user_auth(fd, token, secure_auth)
    if not is_valid then
        utils.send_message(fd, {
            type = "friend",
            action = msg_dec.action or "unknown",
            success = false,
            message = username_or_err
        })
        return nil, nil, nil
    end

    local user_id, user_err = utils.get_user_id_by_name(username_or_err)
    if not user_id then
        utils.send_message(fd, {
            type = "friend",
            action = msg_dec.action or "unknown",
            success = false,
            message = user_err
        })
        return nil, nil, nil
    end

    return username_or_err, user_id, is_valid
end

--- 发送好友请求
-- @param fd WebSocket连接标识
-- @param msg_dec 消息内容
function friend.handle_add_friend_request(fd, msg_dec, users, sessions)
    local username, user_id, is_valid = check_user_and_params(fd, msg_dec, {"friend_name"})
    if not username then return end

    local friend_name = msg_dec.friend_name

    -- 验证是否尝试添加自己为好友
    if user_id == friend_name then
        utils.send_message(fd, {
            type = "friend",
            action = "add_request",
            success = false,
            message = "不能添加自己为好友"
        })
        return
    end

    local friend_id, friend_err = utils.get_user_id_by_name(friend_name)
    if not friend_id then
        utils.send_message(fd, {
            type = "friend",
            action = "add_request",
            success = false,
            message = friend_err
        })
        return
    end

    -- 检查是否已经是好友
    local check_sql = string.format(
        "SELECT 1 FROM friends WHERE (user_id = %d AND friend_id = %d) OR (user_id = %d AND friend_id = %d)",
        user_id, friend_id, friend_id, user_id)
    local check_result = db.execute_query(check_sql)

    if not check_result.code and #check_result.data > 0 then
        utils.send_message(fd, {
            type = "friend",
            action = "add_request",
            success = false,
            message = "你们已经是好友了"
        })
        return
    end

    -- 检查是否已经发送过好友请求
    local check_request_sql = string.format(
        "SELECT 1 FROM friend_requests WHERE user_id = %d AND friend_id = %d",
        user_id, friend_id)
    local check_request_result = db.execute_query(check_request_sql)

    if not check_request_result.code and #check_request_result.data > 0 then
        utils.send_message(fd, {
            type = "friend",
            action = "add_request",
            success = false,
            message = "好友请求已发送，请等待对方确认"
        })
        return
    end

    -- 检查对方是否已经向自己发送了好友请求
    local check_reverse_request_sql = string.format(
        "SELECT 1 FROM friend_requests WHERE user_id = %d AND friend_id = %d",
        friend_id, user_id)
    local check_reverse_request_result = db.execute_query(check_reverse_request_sql)

    if not check_reverse_request_result.code and #check_reverse_request_result.data > 0 then
        -- 对方已经发送了好友请求，直接建立好友关系
        local delete_request_sql = string.format(
            "DELETE FROM friend_requests WHERE user_id = %d AND friend_id = %d",
            friend_id, user_id)
        local delete_result = db.execute_query(delete_request_sql)

        if delete_result.code then
            utils.send_message(fd, {
                type = "friend",
                action = "add_request",
                success = false,
                message = "处理好友请求失败: " .. delete_result.message
            })
            return
        end

        -- 建立双向好友关系
        local insert_sql1 = string.format(
            "INSERT INTO friends (user_id, friend_id, created_at) VALUES (%d, %d, %d)",
            user_id, friend_id, moon.time())
        local insert_sql2 = string.format(
            "INSERT INTO friends (user_id, friend_id, created_at) VALUES (%d, %d, %d)",
            friend_id, user_id, moon.time())

        local insert_result1 = db.execute_query(insert_sql1)
        local insert_result2 = db.execute_query(insert_sql2)

        if insert_result1.code or insert_result2.code then
            utils.send_message(fd, {
                type = "friend",
                action = "add_request",
                success = false,
                message = "添加好友失败"
            })
            return
        end

        utils.send_message(fd, {
            type = "friend",
            action = "add_request",
            success = true,
            message = "成功添加好友"
        })

        -- 通知对方已自动接受好友请求
        for user_fd, user_info in pairs(users) do
            if user_info.username == friend_name then
                utils.send_message(user_fd, {
                    type = "friend",
                    action = "request_accepted",
                    from = username,
                    message = username .. " 已成为您的好友"
                })
                break
            end
        end
        return
    end

    -- 发送好友请求
    local sql = string.format(
        "INSERT INTO friend_requests (user_id, friend_id, message, created_at) VALUES (%d, %d, '%s', %d)",
        user_id, friend_id, db.escape_sql(msg_dec.message or ""), moon.time())
    local result = db.execute_query(sql)

    if result.code then
        utils.send_message(fd, {
            type = "friend",
            action = "add_request",
            success = false,
            message = "发送好友请求失败: " .. result.message
        })
        return
    end

    -- 通知对方收到好友请求
    for user_fd, user_info in pairs(users) do
        if user_info.username == friend_name then
            utils.send_message(user_fd, {
                type = "friend",
                action = "new_request",
                from = username,
                message = msg_dec.message or ""
            })
            break
        end
    end

    utils.send_message(fd, {
        type = "friend",
        action = "add_request",
        success = true,
        message = "好友请求已发送"
    })
end

--- 处理好友请求
-- @param fd WebSocket连接标识
-- @param msg_dec 消息内容
function friend.handle_friend_request_response(fd, msg_dec, users, sessions)
    local username, user_id, is_valid = check_user_and_params(fd, msg_dec, {"requester_name", "accept"})
    if not username then return end

    local requester_name = msg_dec.requester_name
    local accept = msg_dec.accept

    local requester_id, requester_err = utils.get_user_id_by_name(requester_name)
    if not requester_id then
        utils.send_message(fd, {
            type = "friend",
            action = "request_response",
            success = false,
            message = requester_err
        })
        return
    end

    -- 检查是否存在好友请求
    local check_sql = string.format(
        "SELECT 1 FROM friend_requests WHERE user_id = %d AND friend_id = %d",
        requester_id, user_id)
    local check_result = db.execute_query(check_sql)

    if not check_result.code and #check_result.data == 0 then
        utils.send_message(fd, {
            type = "friend",
            action = "request_response",
            success = false,
            message = "好友请求不存在或已处理"
        })
        return
    end

    -- 删除好友请求
    local delete_sql = string.format(
        "DELETE FROM friend_requests WHERE user_id = %d AND friend_id = %d",
        requester_id, user_id)
    local delete_result = db.execute_query(delete_sql)

    if delete_result.code then
        utils.send_message(fd, {
            type = "friend",
            action = "request_response",
            success = false,
            message = "处理好友请求失败: " .. delete_result.message
        })
        return
    end

    if not accept then
        -- 拒绝好友请求
        utils.send_message(fd, {
            type = "friend",
            action = "request_response",
            success = true,
            message = "已拒绝好友请求"
        })

        -- 通知请求者
        for user_fd, user_info in pairs(users) do
            if user_info.username == requester_name then
                utils.send_message(user_fd, {
                    type = "friend",
                    action = "request_rejected",
                    from = username,
                    message = username .. " 拒绝了您的好友请求"
                })
                break
            end
        end
        return
    end

    -- 接受好友请求，建立双向好友关系
    local insert_sql1 = string.format(
        "INSERT INTO friends (user_id, friend_id, created_at) VALUES (%d, %d, %d)",
        requester_id, user_id, moon.time())
    local insert_sql2 = string.format(
        "INSERT INTO friends (user_id, friend_id, created_at) VALUES (%d, %d, %d)",
        user_id, requester_id, moon.time())

    local insert_result1 = db.execute_query(insert_sql1)
    local insert_result2 = db.execute_query(insert_sql2)

    if insert_result1.code or insert_result2.code then
        utils.send_message(fd, {
            type = "friend",
            action = "request_response",
            success = false,
            message = "添加好友失败"
        })
        return
    end

    utils.send_message(fd, {
        type = "friend",
        action = "request_response",
        success = true,
        message = "成功添加好友"
    })

    -- 通知请求者
    for user_fd, user_info in pairs(users) do
        if user_info.username == requester_name then
            utils.send_message(user_fd, {
                type = "friend",
                action = "request_accepted",
                from = username,
                message = username .. " 已成为您的好友"
            })
            break
        end
    end
end

--- 删除好友
-- @param fd WebSocket连接标识
-- @param msg_dec 消息内容
function friend.handle_remove_friend(fd, msg_dec, users, sessions)
    local username, user_id, is_valid = check_user_and_params(fd, msg_dec, {"friend_name"})
    if not username then return end

    local friend_name = msg_dec.friend_name

    local friend_id, friend_err = utils.get_user_id_by_name(friend_name)
    if not friend_id then
        utils.send_message(fd, {
            type = "friend",
            action = "remove",
            success = false,
            message = friend_err
        })
        return
    end

    -- 删除双向好友关系
    local delete_sql1 = string.format(
        "DELETE FROM friends WHERE user_id = %d AND friend_id = %d",
        user_id, friend_id)
    local delete_sql2 = string.format(
        "DELETE FROM friends WHERE user_id = %d AND friend_id = %d",
        friend_id, user_id)

    local delete_result1 = db.execute_query(delete_sql1)
    local delete_result2 = db.execute_query(delete_sql2)

    if delete_result1.code or delete_result2.code then
        utils.send_message(fd, {
            type = "friend",
            action = "remove",
            success = false,
            message = "删除好友失败"
        })
        return
    end

    utils.send_message(fd, {
        type = "friend",
        action = "remove",
        success = true,
        message = "成功删除好友"
    })
end

--- 获取好友列表
-- @param fd WebSocket连接标识
-- @param msg_dec 消息内容
function friend.handle_get_friends_list(fd, msg_dec, users, sessions)
    local username, user_id, is_valid = check_user_and_params(fd, msg_dec, {})
    if not username then return end

    -- 查询好友列表
    local sql = string.format([[
        SELECT u.id, u.username, u.nickname, u.status, u.avatar
        FROM friends f
        JOIN users u ON f.friend_id = u.id
        WHERE f.user_id = %d
        ORDER BY u.username
    ]], user_id)
    local result = db.execute_query(sql)

    if result.code then
        utils.send_message(fd, {
            type = "friend",
            action = "list",
            success = false,
            message = "获取好友列表失败: " .. result.message
        })
        return
    end

    utils.send_message(fd, {
        type = "friend",
        action = "list",
        success = true,
        friends = result.data
    })
end

--- 获取好友请求列表
-- @param fd WebSocket连接标识
-- @param msg_dec 消息内容
function friend.handle_get_friend_requests(fd, msg_dec, users, sessions)
    local username, user_id, is_valid = check_user_and_params(fd, msg_dec, {})
    if not username then return end

    -- 查询好友请求列表
    local sql = string.format([[
        SELECT fr.id, u.username, u.nickname, u.avatar, fr.message, fr.created_at
        FROM friend_requests fr
        JOIN users u ON fr.user_id = u.id
        WHERE fr.friend_id = %d
        ORDER BY fr.created_at DESC
    ]], user_id)
    local result = db.execute_query(sql)

    if result.code then
        utils.send_message(fd, {
            type = "friend",
            action = "requests",
            success = false,
            message = "获取好友请求列表失败: " .. result.message
        })
        return
    end

    utils.send_message(fd, {
        type = "friend",
        action = "requests",
        success = true,
        requests = result.data
    })
end

--- 搜索用户
-- @param fd WebSocket连接标识
-- @param msg_dec 消息内容
function friend.handle_search_users(fd, msg_dec, users, sessions)
    local username, user_id, is_valid = check_user_and_params(fd, msg_dec, {"keyword"})
    if not username then return end

    local keyword = msg_dec.keyword

    -- 搜索用户（排除自己）
    local sql = string.format([[
        SELECT id, username, nickname, avatar, status
        FROM users
        WHERE id != %d AND (username LIKE '%s' OR nickname LIKE '%s')
        ORDER BY username
        LIMIT 50
    ]], user_id, '%' .. db.escape_sql(keyword) .. '%', '%' .. db.escape_sql(keyword) .. '%')
    local result = db.execute_query(sql)

    if result.code then
        utils.send_message(fd, {
            type = "friend",
            action = "search",
            success = false,
            message = "搜索用户失败: " .. result.message
        })
        return
    end

    utils.send_message(fd, {
        type = "friend",
        action = "search",
        success = true,
        users = result.data
    })
end

return friend
