local moon = require("moon")
local crypt = require("crypt")
local json = require("json")

local secure_auth = {}

-- 存储用户会话信息，包含IP地址和过期时间
local user_sessions = {}

-- 会话有效期（秒）- 2小时
local SESSION_EXPIRY = 7200

-- 生成安全令牌
local function generate_token()
    local timestamp = moon.time()
    local random_part = math.random(100000, 999999)
    local hash_part = crypt.base64encode(crypt.sha256(tostring(timestamp) .. tostring(random_part)))
    return string.format("%s%x", hash_part:sub(1, 32), random_part)
end

-- 验证令牌
function secure_auth.validate_token(fd, token)
    if not token then
        return false, "缺少认证令牌"
    end

    local session = user_sessions[token]
    if not session then
        return false, "无效的认证令牌"
    end

    -- 检查会话是否过期
    if moon.time() > session.expires_at then
        user_sessions[token] = nil
        return false, "会话已过期，请重新登录"
    end

    -- 检查是否为同一连接（可选的安全检查）
    -- 如果需要更强的安全性，可以启用下面的检查
    -- if session.fd ~= fd then
    --     return false, "会话绑定错误"
    -- end

    -- 刷新过期时间
    session.expires_at = moon.time() + SESSION_EXPIRY
    return true, session.user_info
end

-- 用户登录并创建安全会话
function secure_auth.login_user(fd, username, ip_address)
    -- 生成新的认证令牌
    local token = generate_token()
    
    -- 存储会话信息
    user_sessions[token] = {
        user_info = {username = username},
        fd = fd,
        ip_address = ip_address,
        created_at = moon.time(),
        expires_at = moon.time() + SESSION_EXPIRY
    }
    
    return token
end

-- 用户登出
function secure_auth.logout_user(token)
    if token then
        user_sessions[token] = nil
    end
end

-- 获取会话信息
function secure_auth.get_session_info(token)
    return user_sessions[token]
end

-- 清理会过期会话
function secure_auth.cleanup_expired_sessions()
    local current_time = moon.time()
    local expired_tokens = {}
    
    for token, session in pairs(user_sessions) do
        if current_time > session.expires_at then
            table.insert(expired_tokens, token)
        end
    end
    
    for _, token in ipairs(expired_tokens) do
        user_sessions[token] = nil
    end
    
    return #expired_tokens
end

-- 定期清理过期会话
local function start_cleanup_timer()
    moon.timeout(300000, function()  -- 每5分钟清理一次
        local count = secure_auth.cleanup_expired_sessions()
        -- 只在有清理会话时才输出日志，减少无用日志
        if count > 0 then
            moon.info("清理了 " .. count .. " 个过期会话")
        end
        start_cleanup_timer()  -- 递归调用以持续清理
    end)
end

start_cleanup_timer()

return secure_auth