local moon = require("moon")
local json = require("json")
local websocket = require("moon.http.websocket")
local config = require("service.config")
local db = require("service.db")

local utils = {}

--- 安全发送WebSocket消息
-- @param fd WebSocket连接标识
-- @param message 消息内容
function utils.send_message(fd, message)
    local msg_content
    if type(message) == "table" then
        -- 使用pcall防止json编码错误导致程序崩溃
        local success, result = pcall(json.encode, message)
        if not success then
            moon.error("JSON编码失败:", result)
            return false
        end
        msg_content = result
    else
        msg_content = tostring(message)
    end

    -- 检查连接是否有效
    if not fd then
        moon.error("无效的连接标识")
        return false
    end

    -- 发送消息
    local success, err = pcall(websocket.write_text, fd, msg_content)
    if not success then
        -- 减少不必要的详细错误日志
        moon.error("发送WebSocket消息失败")
        return false
    end

    return true
end

--- 广播消息给用户列表
-- @param users 用户列表
-- @param message 消息内容
function utils.broadcast_message(users, message)
    local sent_count = 0
    local failed_fds = {}

    for fd, _ in pairs(users) do
        local success = utils.send_message(fd, message)
        if success then
            sent_count = sent_count + 1
        else
            table.insert(failed_fds, fd)
        end
    end

    -- 清理发送失败的连接
    for _, fd in ipairs(failed_fds) do
        users[fd] = nil
    end

    return sent_count
end

--- HTML转义
-- @param str 待转义的字符串
function utils.html_escape(str)
    if not str then return "" end
    
    -- 转义特殊字符
    str = str:gsub("&", "&amp;"):gsub('"', "&quot;"):gsub("'", "&#39;")
    str = str:gsub("<", "&lt;"):gsub(">", "&gt;")
    
    return str
end

--- 生成安全的随机字符串
-- @param length 字符串长度
function utils.generate_random_string(length)
    local chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
    local result = ""
    for i = 1, length or 32 do
        local rand = math.random(#chars)
        result = result .. chars:sub(rand, rand)
    end
    return result
end

--- 获取用户ID的辅助函数
-- @param username 用户名
function utils.get_user_id_by_name(username)
    -- 检查数据库是否就绪
    if not db.postgresql.is_ready() then
        return nil, "数据库未就绪"
    end

    local sql = string.format("SELECT id FROM users WHERE username = '%s'", db.escape_sql(username))
    local result = db.postgresql.execute_query(sql)

    if result.code then
        return nil, "查询用户ID失败: " .. result.message
    end

    if #result.data == 0 then
        return nil, "用户不存在"
    end

    return result.data[1].id
end

--- 获取用户名的辅助函数
-- @param user_id 用户ID
function utils.get_user_name_by_id(user_id)
    -- 检查数据库是否就绪
    if not db.postgresql.is_ready() then
        return nil, "数据库未就绪"
    end

    local sql = string.format("SELECT username FROM users WHERE id = %d", user_id)
    local result = db.postgresql.execute_query(sql)

    if result.code then
        return nil, "查询用户名失败: " .. result.message
    end

    if #result.data == 0 then
        return nil, "用户不存在"
    end

    return result.data[1].username
end

--- 通用参数验证函数
-- @param params 参数表
-- @param required_fields 必需字段列表
function utils.validate_params(params, required_fields)
    for _, field in ipairs(required_fields) do
        if not params[field] then
            return false, "缺少必要参数: " .. field
        end
    end
    return true
end

--- 通用安全检查和用户验证函数
-- @param fd WebSocket连接标识
-- @param token 安全令牌
-- @param secure_auth 安全认证模块
function utils.check_user_auth(fd, token, secure_auth)
    if not token then
        return false, "缺少认证令牌"
    end
    
    local is_valid, user_info = secure_auth.validate_token(fd, token)
    if not is_valid then
        return false, user_info
    end
    
    return true, user_info
end

--- 执行数据库操作的辅助函数
-- @param operation 数据库操作函数
function utils.execute_db_operation(operation)
    -- 检查数据库是否就绪
    if not db.postgresql.is_ready() then
        return false, "数据库未就绪"
    end
    
    -- 直接执行操作
    local success, result = pcall(operation)
    if not success then
        moon.error("数据库操作执行失败:", result)
        return false, "操作执行失败"
    end
    
    return result
end

return utils