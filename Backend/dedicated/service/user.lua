local crypt = require("crypt") -- 添加加密库支持
local json  = require("json")
local db    = require("service.db")
local utils = require("service.utils")

-- 定义 user 模块
local user  = {}

-- 密码哈希函数
local function hash_password(password)
    -- 在实际应用中使用更强的哈希算法如bcrypt
    return crypt.base64encode(crypt.hmac_sha1(password, "salt"))
end

-- 验证密码
local function verify_password(input_password, stored_hash)
    return hash_password(input_password) == stored_hash
end

--- 用户注册
-- @param username 用户名
-- @param password 密码
-- @param profile 用户资料
-- @param callback 回调函数
function user.register(username, password, profile, callback)
    -- 参数校验
    if not username or not password then
        if callback then
            callback(false, "用户名和密码不能为空")
        end
        return
    end

    -- 检查数据库连接是否就绪
    if not db.postgresql.is_ready() then
        if callback then
            callback(false, "系统初始化中，请稍后再试")
        end
        return
    end

    -- 查询用户信息
    local check_sql = string.format("SELECT id FROM users WHERE username = '%s'", db.escape_sql(username))
    db.execute_query(check_sql, function(check_result)
        if check_result.code then
            -- 减少敏感错误信息的暴露
            if callback then
                callback(false, "数据库查询错误")
            end
            return
        end

        if #check_result.data > 0 then
            if callback then
                callback(false, "用户已存在")
            end
            return
        end

        -- 插入新用户
        local profile_data = profile or {}
        local profile_json = "{}"
        local success, encoded = pcall(json.encode, profile_data)
        if success then
            profile_json = encoded
        else
            -- JSON编码失败时使用空对象
            profile_json = "{}"
        end
        
        local hashed_password = hash_password(password)
        local insert_sql = string.format(
            "INSERT INTO users (username, password, profile) VALUES ('%s', '%s', '%s') RETURNING id",
            db.escape_sql(username), hashed_password, db.escape_sql(profile_json))
        db.execute_query(insert_sql, function(insert_result)
            if insert_result.code then
                -- 减少敏感错误信息的暴露
                if callback then
                    callback(false, "注册失败")
                end
                return
            end

            if callback then
                callback(true, { id = insert_result.data[1].id, username = username })
            end
        end)
    end)
end

--- 用户登录
-- @param username 用户名
-- @param password 密码
-- @param callback 回调函数
function user.login(username, password, callback)
    -- 参数校验
    if not username or not password then
        if callback then
            callback(false, "用户名和密码不能为空")
        end
        return
    end

    -- 检查数据库连接是否就绪
    if not db.postgresql.is_ready() then
        if callback then
            callback(false, "系统初始化中，请稍后再试")
        end
        return
    end

    -- 查询用户信息
    local sql = string.format("SELECT id, username, password FROM users WHERE username = '%s'", db.escape_sql(username))
    db.execute_query(sql, function(result)
        if result.code then
            -- 减少敏感错误信息的暴露
            if callback then
                callback(false, "登录失败")
            end
            return
        end

        if #result.data == 0 then
            if callback then
                callback(false, "用户不存在")
            end
            return
        end

        local user_data = result.data[1]

        -- 验证密码
        if not verify_password(password, user_data.password) then
            if callback then
                callback(false, "密码错误")
            end
            return
        end

        -- 返回用户信息（不包含密码）
        if callback then
            callback(true, { id = user_data.id, username = user_data.username })
        end
    end)
end

--- 用户登出
-- @param session_id 会话ID
-- @param callback 回调函数
function user.logout(session_id, callback)
    if not session_id then
        if callback then
            callback(false, "会话ID不能为空")
        end
        return
    end

    -- 注意：这里只清理本地会话信息，实际的令牌清理在secure_auth模块中完成
    if callback then
        callback(true)
    end
end

--- 获取用户信息
-- @param username 用户名
-- @param callback 回调函数
function user.get_profile(username, callback)
    if not username then
        if callback then
            callback(false, "用户名不能为空")
        end
        return
    end

    -- 检查数据库连接是否就绪
    if not db.postgresql.is_ready() then
        if callback then
            callback(false, "系统初始化中，请稍后再试")
        end
        return
    end

    -- 使用字符串拼接而不是参数占位符
    local sql = string.format("SELECT username, profile, created_at FROM users WHERE username = '%s'", db.escape_sql(username))
    db.execute_query(sql, function(result)
        if result.code then
            -- 减少敏感错误信息的暴露
            if callback then
                callback(false, "获取用户信息失败")
            end
            return
        end

        if #result.data == 0 then
            if callback then
                callback(false, "用户不存在")
            end
            return
        end

        local user_info = result.data[1]
        local profile = {}
        if user_info.profile then
            local success, decoded = pcall(json.decode, user_info.profile)
            if success then
                profile = decoded
            else
                -- JSON解析失败时使用空表
                profile = {}
            end
        end

        if callback then
            callback(true, {
                username = user_info.username,
                profile = profile,
                created_at = user_info.created_at
            })
        end
    end)
end

return user