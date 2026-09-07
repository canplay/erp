-- 如果是初始化阶段，返回配置信息
if _G["__init__"] then
    local arg = ...
    return {
        thread = 8,                                                           -- 设置线程数
        enable_stdout = true,                                                 -- 启用标准输出
        logfile = string.format("logs/%s.log", os.date("%Y-%m-%d-%H-%M-%S")), -- 日志文件路径
        loglevel = "DEBUG"                                                    -- 日志级别
    }
end

-- 引入必要的模块
local moon = require("moon")
local socket = require("moon.socket")
local json = require("json")
local websocket = require("moon.http.websocket")
local config = require("service.config")
local MessageHandler = require("service.message_handler")
local secure_auth = require("service.secure_auth")
local db = require("service.db")

-- 从配置模块获取配置
local conf = config.get_all()
conf.host = config.get("host", "127.0.0.1")
conf.port = config.get("port", 51530)

-- 全局常量定义
local HEARTBEAT_INTERVAL = 30 -- 心跳间隔时间(秒)

-- 存储在线用户信息和会话
local users = {}
local sessions = {}
local session_counter = 0

-- 异步初始化数据库连接
moon.async(function()
    db.init()
end)

--- 生成会话ID
local function generate_session_id()
    session_counter = session_counter + 1
    return "session_" .. moon.time() .. "_" .. session_counter
end

--- 获取客户端IP地址
local function get_client_ip(fd)
    local info = socket.getaddress(fd)
    return info and info or "unknown"
end

--- 用户心跳检测
local function check_user_heartbeat()
    local current_time = moon.time()
    local disconnected_users = {}

    for fd, user_info in pairs(users) do
        -- 检查用户是否超时
        if user_info.last_heartbeat and (current_time - user_info.last_heartbeat) > (HEARTBEAT_INTERVAL * 3) then
            -- 用户超时，标记为断开连接
            table.insert(disconnected_users, fd)
        end
    end

    -- 清理超时用户
    for _, fd in ipairs(disconnected_users) do
        moon.info("用户心跳超时，断开连接:", fd)
        websocket.close(fd)
    end
end

--- 定期执行心跳检测
local function start_heartbeat_check()
    moon.timeout(HEARTBEAT_INTERVAL * 1000, function()
        check_user_heartbeat()
        start_heartbeat_check() -- 递归调用以实现重复执行
    end)
end

start_heartbeat_check() -- 启动心跳检测

--- 处理 WebSocket 消息
websocket.wson("message", function(fd, msg)
    -- 解码消息内容
    local decoded_msg = moon.decode(msg, "Z")

    -- 尝试解析 JSON 消息
    local success, msg_dec = pcall(json.decode, decoded_msg)
    -- 检查消息格式是否有效
    if not success or not msg_dec or not msg_dec.type then
        print("未知的消息格式")
        return
    end

    -- 更新用户心跳时间
    if users[fd] then
        users[fd].last_heartbeat = moon.time()
    end

    -- 使用消息处理器处理不同类型的消息
    local handler = MessageHandler:new(users, sessions, generate_session_id, secure_auth)
    handler:handle(fd, msg_dec)
end)

--- 处理 WebSocket 连接事件
websocket.on_accept(function(fd, msg)
    print_r(msg)
    print("wsaccept ", fd)
    socket.settimeout(fd, 64400)

    -- 获取客户端IP地址
    local client_ip = get_client_ip(fd)

    users[fd] = {
        connected_at = moon.time(),
        last_heartbeat = moon.time(),
        ip_address = client_ip -- 记录客户端IP地址
    }

    -- 直接调用，避免在pcall中使用可能产生yield的操作
    local success, err = websocket.write_text(fd, json.encode({
        type = "system",
        data = "connected",
        fd = fd
    }))

    if not success then
        moon.error("发送连接确认消息失败:", err)
    end
end)

--- 处理 WebSocket 关闭事件
websocket.wson("close", function(fd, msg)
    -- 处理各种编码格式的消息，包括GB2312和UTF-8
    local msg_str = ""
    if msg then
        -- 尝试解码为字符串
        local decoded_msg = moon.decode(msg, "Z")
        if decoded_msg then
            -- 尝试多种方式清理消息内容
            local cleaned_msg = ""

            -- 方法1: 移除控制字符，保留可打印字符
            cleaned_msg = string.gsub(decoded_msg, "[\0-\31\127-\159]", "")

            -- 如果清理后的消息不为空，则使用它
            if #cleaned_msg > 0 then
                msg_str = cleaned_msg
            else
                -- 方法2: 替换控制字符为点号
                msg_str = string.gsub(decoded_msg, "[\0-\31\127-\159]", ".")

                -- 如果点号太多，则还原为原始消息
                local dot_count = 0
                for c in msg_str:gmatch(".") do
                    if c == "." then dot_count = dot_count + 1 end
                end

                if dot_count > #msg_str * 0.7 then
                    msg_str = decoded_msg
                end
            end

            -- 去除首尾空白字符
            msg_str = string.match(msg_str, "^%s*(.-)%s*$") or msg_str
        else
            -- 如果解码失败，尝试直接转换为字符串
            msg_str = tostring(msg)
        end

        -- 如果最终消息为空，则显示特殊标记
        if msg_str == "" then
            msg_str = "<no readable message>"
        end
    else
        msg_str = "<no message>"
    end

    print("wsclose ", fd, msg_str)

    local user_info = users[fd]
    if user_info and user_info.session_id then
        sessions[user_info.session_id] = nil
    end

    users[fd] = nil
end)

-- 启动 WebSocket 服务器并监听指定端口
local listenfd = websocket.listen(conf.host, conf.port)
print("websocket server start ", conf.host, conf.port)
print("enter 'CTRL-C' stop server.")

-- 异步测试连接
moon.async(function()
    local fd = websocket.connect(string.format("ws://%s:%s/chat", conf.host, conf.port))
    -- 直接调用，避免在pcall中使用可能产生yield的操作
    local success, err = websocket.write_text(fd, '{"type": "msg", "data": "hello world"}')
    if not success then
        moon.error("发送测试消息失败:", err)
    end
    websocket.close(fd) -- 关闭连接
end)

--- 停止服务器时的清理逻辑
moon.shutdown(function()
    websocket.close(listenfd) -- 关闭监听套接字
    moon.quit()               -- 退出服务
end)
