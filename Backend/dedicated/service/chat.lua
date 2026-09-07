local moon = require("moon")
local json = require("json")

local db = require("service.db")
local utils = require("service.utils")

local chat = {}

local MAX_HISTORY = 100

--- 保存消息到Redis缓存
-- @param msg_type 消息类型
-- @param data 消息数据
local function save_message_to_cache(msg_type, data)
    -- 检查Redis是否已准备就绪
    if not db.redis.is_ready() then
        moon.warn("Redis尚未准备就绪，跳过缓存消息")
        return
    end

    local msg = {
        type = msg_type,
        data = data,
        timestamp = moon.time()
    }

    -- 获取当前类型的消息列表
    local result, err = db.redis.execute_command("get", "latest_messages")
    if err then
        moon.error("Redis获取消息失败:", err)
    end

    local messages = nil

    if result then
        local success, decoded = pcall(json.decode, result)
        if success then
            messages = decoded
        else
            moon.error("Redis消息解码失败:", decoded)
        end
    end

    messages = messages or {}

    -- 如果没有该类型的消息列表，则创建
    if not messages[msg_type] then
        messages[msg_type] = {}
    end

    -- 添加新消息
    table.insert(messages[msg_type], msg)

    -- 限制历史消息数量
    if #messages[msg_type] > MAX_HISTORY then
        table.remove(messages[msg_type], 1)
    end

    -- 缓存更新后的消息列表
    local json_str = json.encode(messages)
    local set_result, set_err = db.redis.execute_command("set", "latest_messages", json_str)
    if set_err then
        moon.error("Redis设置消息失败:", set_err)
    end

    local expire_result, expire_err = db.redis.execute_command("expire", "latest_messages", 86400)
    if expire_err then
        moon.error("Redis设置过期时间失败:", expire_err)
    end

    return msg
end

--- 批量保存消息到PostgreSQL数据库
-- @param messages 消息列表
local function batch_save_messages_to_db(messages)
    -- 检查数据库是否已准备就绪
    if not db.postgresql.is_ready() then
        moon.warn("数据库尚未准备就绪，跳过保存消息")
        return
    end

    -- 批量处理消息
    for _, msg_data in ipairs(messages) do
        local msg_type = msg_data.type
        local data = msg_data.data

        -- 根据消息类型决定如何存储
        if msg_type == "public" or msg_type:sub(1, 7) == "private" or msg_type:sub(1, 5) == "group" then
            -- 所有类型的消息存储到数据库
            local success, result = db.postgresql.save_message(
                tonumber(data.sender) or 0,
                tostring(data.sender),
                tostring(data.data),
                nil
            )
            if not success then
                moon.error("保存消息到数据库失败:", result)
            end
        end
    end
end

--- 处理公共消息
-- @param users 在线用户列表
-- @param msg_dec 解析后的消息
function chat.process_public_message(users, msg_dec)
    local message_data = {
        sender = msg_dec.username,
        data = msg_dec.data,
        timestamp = moon.time()
    }

    -- 保存消息到缓存
    local msg = save_message_to_cache("public", message_data)

    -- 广播消息给所有在线用户
    for fd, user_info in pairs(users) do
        utils.send_message(fd, {
            type = "public",
            data = message_data
        })
    end

    -- 异步保存到数据库
    moon.async(function()
        batch_save_messages_to_db({ msg })
    end)
end

--- 处理私聊消息
-- @param users 在线用户列表
-- @param fd 发送方连接ID
-- @param msg_dec 解析后的消息
function chat.process_private_message(users, fd, msg_dec)
    local sender_info = users[fd]
    if not sender_info then
        utils.send_message(fd, {
            type = "error",
            message = "发送者信息不存在"
        })
        return
    end

    local message_data = {
        sender = sender_info.username,
        receiver = msg_dec.receiver,
        data = msg_dec.data,
        timestamp = moon.time()
    }

    -- 保存消息到缓存
    local msg_type = "private_" .. msg_dec.receiver
    local msg = save_message_to_cache(msg_type, message_data)

    -- 发送给接收方
    local receiver_fd = nil
    for target_fd, user_info in pairs(users) do
        if user_info.username == msg_dec.receiver then
            receiver_fd = target_fd
            break
        end
    end

    if receiver_fd then
        utils.send_message(receiver_fd, {
            type = "private",
            data = message_data
        })
    end

    -- 同时回传给发送方确认
    utils.send_message(fd, {
        type = "private_ack",
        data = message_data
    })

    -- 异步保存到数据库
    moon.async(function()
        batch_save_messages_to_db({ msg })
    end)
end

--- 处理群组消息
-- @param users 在线用户列表
-- @param msg_dec 解析后的消息
function chat.process_group_message(users, msg_dec)
    local message_data = {
        sender = msg_dec.username,
        group_id = msg_dec.group_id,
        data = msg_dec.data,
        timestamp = moon.time()
    }

    -- 保存消息到缓存
    local msg_type = "group_" .. msg_dec.group_id
    local msg = save_message_to_cache(msg_type, message_data)

    -- 广播消息给群组成员
    for fd, user_info in pairs(users) do
        -- 这里应该检查用户是否属于该群组
        -- 简化实现，广播给所有在线用户
        utils.send_message(fd, {
            type = "group",
            data = message_data
        })
    end

    -- 异步保存到数据库
    moon.async(function()
        batch_save_messages_to_db({ msg })
    end)
end

return chat