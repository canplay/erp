local moon = require("moon")
local redis = require("moon.db.redis")
local pg = require("moon.db.pg")
local config = require("service.config")

-- SQL转义函数，替代原来的moon.db.escape
local function escape_sql(str)
    if not str then return "" end
    str = tostring(str)
    -- 转义单引号
    str = str:gsub("'", "''")
    -- 转义反斜杠
    str = str:gsub("\\", "\\\\")
    return str
end

-- Redis配置
local redis_config = {
    host = config.get("redis.host", "127.0.0.1"),
    port = tonumber(config.get("redis.port", "6379")),
    password = config.get("redis.password", nil)
}

-- PostgreSQL配置
local pg_config = {
    host = config.get("postgresql.host", "127.0.0.1"),
    port = tonumber(config.get("postgresql.port", "5432")),
    database = config.get("postgresql.database", "chat_app"),
    user = config.get("postgresql.user", "postgres"),
    password = config.get("postgresql.password", "password")
}

-- 连接池配置
local CONNECTION_POOL_SIZE = 5

-- Redis连接池
local redis_clients = {}
local redis_pool_index = 1

-- PostgreSQL连接池
local pg_connections = {}
local pg_pool_index = 1

-- 初始化状态
local redis_initialized = false
local pg_initialized = false

-- Redis相关函数
local redis_module = {}

-- 获取Redis连接池中的一个连接
function redis_module.get_client()
    if not redis_initialized or #redis_clients == 0 then
        return nil
    end

    -- 循环使用连接池中的连接
    redis_pool_index = (redis_pool_index % #redis_clients) + 1
    return redis_clients[redis_pool_index]
end

-- 初始化Redis连接池（异步）
function redis_module.init(on_complete)
    if redis_initialized then
        if on_complete then
            on_complete(true, "Redis已初始化")
        end
        return
    end

    moon.info("初始化Redis连接池...")

    -- 使用moon.async来避免协程yield问题
    moon.async(function()
        -- 清空旧的连接（如果有的话）
        redis_clients = {}

        -- 创建连接池
        for i = 1, CONNECTION_POOL_SIZE do
            local client, err = redis.connect(redis_config)
            if client then
                -- 如果配置了密码，则进行认证
                if redis_config.password then
                    local auth_result, auth_err = client:auth(redis_config.password)
                    if not auth_result then
                        moon.error("Redis认证失败: ", auth_err)
                        if on_complete then
                            on_complete(false, "Redis认证失败: " .. (auth_err or ""))
                        end
                        return
                    end
                end
                table.insert(redis_clients, client)
            else
                moon.error("Redis连接失败: ", err)
                if on_complete then
                    on_complete(false, "Redis连接失败: " .. (err or ""))
                end
                return
            end
        end

        -- 检查是否成功创建了连接
        if #redis_clients == 0 then
            moon.error("未能创建任何Redis连接")
            if on_complete then
                on_complete(false, "未能创建任何Redis连接")
            end
            return
        end

        -- 标记为已初始化
        redis_initialized = true
        moon.info("Redis连接池初始化成功")

        -- 调用完成回调
        if on_complete then
            on_complete(true, "Redis初始化成功")
        end
    end)
end

-- 确保Redis连接有效
function redis_module.ensure_connection()
    if not redis_initialized then
        return false
    end

    -- 检查连接池是否为空
    if #redis_clients == 0 then
        moon.error("Redis连接池为空")
        return false
    end

    -- 检查连接是否仍然有效
    local client = redis_module.get_client()
    if not client then
        moon.error("无法获取Redis客户端")
        return false
    end

    -- 使用ping操作检查连接状态
    local ping_result, ping_err = client:ping()
    if not ping_result then
        moon.error("Redis连接失效:", ping_err)
        return false
    end

    return true
end

-- 执行Redis命令（同步方式）
function redis_module.execute_command(command, ...)
    -- 确保连接正常
    if not redis_module.ensure_connection() then
        return nil, "Redis连接异常"
    end

    local client = redis_module.get_client()
    if not client then
        return nil, "无法获取Redis客户端"
    end

    -- 执行命令
    return client[command](client, ...)
end

-- 检查Redis是否就绪
function redis_module.is_ready()
    return redis_initialized and #redis_clients > 0 and redis_module.ensure_connection()
end

-- PostgreSQL相关函数
local pg_module = {}

-- 获取PostgreSQL连接池中的一个连接
function pg_module.get_connection()
    if not pg_initialized or #pg_connections == 0 then
        return nil
    end

    -- 循环使用连接池中的连接
    pg_pool_index = (pg_pool_index % #pg_connections) + 1
    return pg_connections[pg_pool_index]
end

-- 初始化PostgreSQL连接池
function pg_module.init(on_complete)
    if pg_initialized then
        if on_complete then
            on_complete(true, "PostgreSQL已初始化")
        end
        return
    end

    moon.info("初始化PostgreSQL连接池...")

    -- 使用moon.async来避免协程yield问题
    moon.async(function()
        -- 清空旧的连接（如果有的话）
        pg_connections = {}

        -- 创建连接池
        for i = 1, CONNECTION_POOL_SIZE do
            local conn, err = pg.connect(pg_config)
            if conn then
                if conn.code then
                    moon.error("PostgreSQL连接失败: ", conn.message)
                    if on_complete then
                        on_complete(false, "PostgreSQL连接失败: " .. (conn.message or ""))
                    end
                    return
                end
                table.insert(pg_connections, conn)
            else
                moon.error("PostgreSQL连接失败: ", err)
                if on_complete then
                    on_complete(false, "PostgreSQL连接失败: " .. (err or ""))
                end
                return
            end
        end

        -- 检查是否成功创建了连接
        if #pg_connections == 0 then
            moon.error("未能创建任何数据库连接")
            if on_complete then
                on_complete(false, "未能创建任何数据库连接")
            end
            return
        end

        -- 标记为已初始化
        pg_initialized = true

        -- 测试第一个连接是否有效
        local test_conn = pg_connections[1]
        local test_result = pg.query(test_conn, "SELECT 1")
        if test_result.code then
            moon.error("PostgreSQL连接测试失败: ", test_result.message)
            pg_initialized = false
            if on_complete then
                on_complete(false, "PostgreSQL连接测试失败: " .. (test_result.message or ""))
            end
            return
        end

        moon.info("PostgreSQL连接池初始化成功")

        -- 调用完成回调
        if on_complete then
            on_complete(true, "PostgreSQL初始化成功")
        end
    end)
end

-- 确保PostgreSQL连接有效
function pg_module.ensure_connection()
    if not pg_initialized then
        return false
    end

    -- 检查连接池是否为空
    if #pg_connections == 0 then
        moon.error("数据库连接池为空")
        return false
    end

    -- 测试连接是否仍然有效
    local conn = pg_module.get_connection()
    if not conn then
        moon.error("无法获取数据库连接")
        return false
    end

    local result = pg.query(conn, "SELECT 1")
    if result.code then
        moon.error("数据库连接失效:", result.message)
        return false
    end

    return true
end

-- 执行PostgreSQL查询
function pg_module.execute_query(sql, ...)
    -- 检查数据库是否已初始化且连接池不为空
    if not pg_initialized or #pg_connections == 0 then
        moon.error("数据库未就绪")
        return { code = -1, message = "数据库未就绪" }
    end

    local conn = pg_module.get_connection()
    if not conn then
        moon.error("无法获取数据库连接")
        return { code = -1, message = "无法获取数据库连接" }
    end

    -- 处理SQL参数替换
    local params = { ... }
    if #params > 0 then
        -- 替换占位符 $1, $2, ... 为实际值
        local formatted_sql = sql
        for i, param in ipairs(params) do
            -- 对参数进行适当转义
            local escaped_param = escape_sql(param)
            formatted_sql = formatted_sql:gsub("%$" .. i, "'" .. escaped_param .. "'")
        end
        -- 执行查询
        return pg.query(conn, formatted_sql)
    else
        -- 执行无参数查询
        return pg.query(conn, sql)
    end
end

-- 保存消息到PostgreSQL数据库
function pg_module.save_message(user_id, username, content, image_url, callback)
    local sql = "INSERT INTO messages (user_id, username, content, image_url) VALUES ($1, $2, $3, $4) RETURNING id"
    local result = pg_module.execute_query(sql, user_id, username, content, image_url)

    if result.code then
        moon.error("保存消息失败:", result.message)
        if callback then
            callback(false, "保存消息失败")
        end
    else
        moon.debug("消息保存成功，ID:", result.data[1].id)
        if callback then
            callback(true, result.data[1].id)
        end
    end
end

-- 从PostgreSQL数据库获取历史消息
function pg_module.get_history_messages(limit, callback)
    local limit_value = math.min(tonumber(limit) or 50, 1000)

    local sql = string.format([[
        SELECT user_id, username, content, image_url, created_at
        FROM messages
        ORDER BY created_at DESC
        LIMIT %d
    ]], limit_value)

    local result = pg_module.execute_query(sql)
    if result.code then
        moon.error("获取历史消息失败:", result.message)
        if callback then
            callback(false, "获取历史消息失败")
        end
    else
        -- 反转结果以按时间顺序显示
        local messages = {}
        for i = #result.data, 1, -1 do
            table.insert(messages, result.data[i])
        end
        if callback then
            callback(true, messages)
        end
    end
end

-- 检查PostgreSQL是否就绪
function pg_module.is_ready()
    return pg_initialized and #pg_connections > 0
end

-- 提供初始化函数供外部调用
local function init()
    redis_module.init()
    pg_module.init()
end

-- 返回模块和初始化函数
return {
    redis = redis_module,
    postgresql = pg_module,
    escape_sql = escape_sql,
    init = init,
    execute_query = function(sql, callback)
        local result = pg_module.execute_query(sql)
        if callback then
            callback(result)
        end
        return result
    end
}
