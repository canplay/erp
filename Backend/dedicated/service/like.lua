-- 定义 like 模块
local like = {}
local utils = require("service.utils")
local secure_auth = require("service.secure_auth")
local db = require("service.db")
local moon = require("moon")

-- 通用的用户验证和参数检查函数
local function check_user_and_params(fd, msg_dec, required_params)
    local token = msg_dec.token
    
    -- 验证输入
    local valid, err = utils.validate_params(msg_dec, required_params)
    if not valid then
        utils.send_message(fd, {
            type = "like",
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
            type = "like",
            action = msg_dec.action or "unknown",
            success = false,
            message = username_or_err
        })
        return nil, nil, nil
    end

    local user_id, user_err = utils.get_user_id_by_name(username_or_err)
    if not user_id then
        utils.send_message(fd, {
            type = "like",
            action = msg_dec.action or "unknown",
            success = false,
            message = user_err
        })
        return nil, nil, nil
    end

    return username_or_err, user_id, is_valid
end

--- 点赞
-- @param username 用户名
-- @param target_type 目标类型 (message, post等)
-- @param target_id 目标ID
function like.add(username, target_type, target_id)
    -- 检查参数
    if not username or not target_type or not target_id then
        return false, "参数不能为空"
    end

    -- 检查数据库连接
    if not db.postgresql.is_ready() then
        return false, "系统初始化中，请稍后再试"
    end

    -- 获取用户ID
    local user_id, user_err = utils.get_user_id_by_name(username)
    if not user_id then
        return false, user_err
    end

    local key = target_type .. ":" .. target_id

    -- 先检查是否已经点踩，如果已点踩则取消点踩
    local check_dislike_sql = string.format(
        "SELECT id FROM likes WHERE user_id = %d AND target_type = '%s' AND target_id = '%s'",
        user_id, "dislike_" .. db.escape(target_type), db.escape(target_id))
    local check_dislike_result = db.execute_query(check_dislike_sql)

    if check_dislike_result.code then
        return false, "检查点踩状态失败: " .. check_dislike_result.message
    end

    if #check_dislike_result.data > 0 then
        -- 取消点踩
        local remove_dislike_sql = string.format("DELETE FROM likes WHERE id = %d", check_dislike_result.data[1].id)
        local remove_dislike_result = db.execute_query(remove_dislike_sql)

        if remove_dislike_result.code then
            return false, "取消点踩失败: " .. remove_dislike_result.message
        end
    end

    -- 检查是否已经点赞
    local check_sql = string.format(
        "SELECT id FROM likes WHERE user_id = %d AND target_type = '%s' AND target_id = '%s'",
        user_id, db.escape(target_type), db.escape(target_id))
    local check_result = db.execute_query(check_sql)

    if check_result.code then
        return false, "检查点赞状态失败: " .. check_result.message
    end

    if #check_result.data > 0 then
        -- 已经点赞，取消点赞
        local remove_sql = string.format("DELETE FROM likes WHERE id = %d", check_result.data[1].id)
        local remove_result = db.execute_query(remove_sql)

        if remove_result.code then
            return false, "取消点赞失败: " .. remove_result.message
        end

        return true, "取消点赞成功"
    else
        -- 未点赞，添加点赞
        local insert_sql = string.format("INSERT INTO likes (user_id, target_type, target_id) VALUES (%d, '%s', '%s')",
            user_id, db.escape(target_type), db.escape(target_id))
        local insert_result = db.execute_query(insert_sql)

        if insert_result.code then
            return false, "点赞失败: " .. insert_result.message
        end

        return true, "点赞成功"
    end
end

--- 点踩
-- @param username 用户名
-- @param target_type 目标类型 (message, post等)
-- @param target_id 目标ID
function like.dislike(username, target_type, target_id)
    -- 检查参数
    if not username or not target_type or not target_id then
        return false, "参数不能为空"
    end

    -- 检查数据库连接
    if not db.postgresql.is_ready() then
        return false, "系统初始化中，请稍后再试"
    end

    -- 获取用户ID
    local user_id, user_err = utils.get_user_id_by_name(username)
    if not user_id then
        return false, user_err
    end

    -- 先检查是否已经点赞，如果已点赞则取消点赞
    local check_like_sql = string.format(
        "SELECT id FROM likes WHERE user_id = %d AND target_type = '%s' AND target_id = '%s'",
        user_id, db.escape(target_type), db.escape(target_id))
    local check_like_result = db.execute_query(check_like_sql)

    if check_like_result.code then
        return false, "检查点赞状态失败: " .. check_like_result.message
    end

    if #check_like_result.data > 0 then
        -- 取消点赞
        local remove_like_sql = string.format("DELETE FROM likes WHERE id = %d", check_like_result.data[1].id)
        local remove_like_result = db.execute_query(remove_like_sql)

        if remove_like_result.code then
            return false, "取消点赞失败: " .. remove_like_result.message
        end
    end

    -- 检查是否已经点踩
    local check_sql = string.format(
        "SELECT id FROM likes WHERE user_id = %d AND target_type = '%s' AND target_id = '%s'",
        user_id, "dislike_" .. db.escape(target_type), db.escape(target_id))
    local check_result = db.execute_query(check_sql)

    if check_result.code then
        return false, "检查点踩状态失败: " .. check_result.message
    end

    if #check_result.data > 0 then
        -- 已经点踩，取消点踩
        local remove_sql = string.format("DELETE FROM likes WHERE id = %d", check_result.data[1].id)
        local remove_result = db.execute_query(remove_sql)

        if remove_result.code then
            return false, "取消点踩失败: " .. remove_result.message
        end

        return true, "取消点踩成功"
    else
        -- 未点踩，添加点踩
        local insert_sql = string.format("INSERT INTO likes (user_id, target_type, target_id) VALUES (%d, '%s', '%s')",
            user_id, "dislike_" .. db.escape(target_type), db.escape(target_id))
        local insert_result = db.execute_query(insert_sql)

        if insert_result.code then
            return false, "点踩失败: " .. insert_result.message
        end

        return true, "点踩成功"
    end
end

--- 取消点赞/点踩
-- @param username 用户名
-- @param target_type 目标类型 (message, post等)
-- @param target_id 目标ID
function like.remove(username, target_type, target_id)
    -- 检查参数
    if not username or not target_type or not target_id then
        return false, "参数不能为空"
    end

    -- 检查数据库连接
    if not db.postgresql.is_ready() then
        return false, "系统初始化中，请稍后再试"
    end

    -- 获取用户ID
    local user_id, user_err = utils.get_user_id_by_name(username)
    if not user_id then
        return false, user_err
    end

    -- 删除点赞记录
    local delete_like_sql = string.format(
        "DELETE FROM likes WHERE user_id = %d AND target_type = '%s' AND target_id = '%s'",
        user_id, db.escape(target_type), db.escape(target_id))
    local delete_like_result = db.execute_query(delete_like_sql)

    if delete_like_result.code then
        return false, "取消点赞失败: " .. delete_like_result.message
    end

    -- 删除点踩记录
    local delete_dislike_sql = string.format(
        "DELETE FROM likes WHERE user_id = %d AND target_type = '%s' AND target_id = '%s'",
        user_id, "dislike_" .. db.escape(target_type), db.escape(target_id))
    local delete_dislike_result = db.execute_query(delete_dislike_sql)

    if delete_dislike_result.code then
        return false, "取消点踩失败: " .. delete_dislike_result.message
    end

    -- 检查是否有任何记录被删除
    if delete_like_result.affected_rows > 0 or delete_dislike_result.affected_rows > 0 then
        return true, "取消操作成功"
    else
        return false, "没有找到相关的点赞或点踩记录"
    end
end

--- 获取点赞/点踩列表
-- @param target_type 目标类型
-- @param target_id 目标ID
function like.list(target_type, target_id)
    -- 检查参数
    if not target_type or not target_id then
        return false, "参数不能为空"
    end

    -- 检查数据库连接
    if not db.postgresql.is_ready() then
        return false, "系统初始化中，请稍后再试"
    end

    -- 查询点赞列表
    local like_sql = string.format([[
        SELECT u.username, l.created_at
        FROM likes l
        JOIN users u ON l.user_id = u.id
        WHERE l.target_type = '%s' AND l.target_id = '%s'
        ORDER BY l.created_at DESC
    ]], db.escape(target_type), db.escape(target_id))
    local like_result = db.execute_query(like_sql)

    if like_result.code then
        return false, "查询点赞列表失败: " .. like_result.message
    end

    -- 查询点踩列表
    local dislike_sql = string.format([[
        SELECT u.username, l.created_at
        FROM likes l
        JOIN users u ON l.user_id = u.id
        WHERE l.target_type = '%s' AND l.target_id = '%s'
        ORDER BY l.created_at DESC
    ]], "dislike_" .. db.escape(target_type), db.escape(target_id))
    local dislike_result = db.execute_query(dislike_sql)

    if dislike_result.code then
        return false, "查询点踩列表失败: " .. dislike_result.message
    end

    return true, {
        likes = like_result.data,
        dislikes = dislike_result.data
    }
end

--- 检查用户对目标的点赞状态
-- @param username 用户名
-- @param target_type 目标类型
-- @param target_id 目标ID
function like.get_user_status(username, target_type, target_id)
    -- 检查参数
    if not username or not target_type or not target_id then
        return false, "参数不能为空"
    end

    -- 检查数据库连接
    if not db.postgresql.is_ready() then
        return false, "系统初始化中，请稍后再试"
    end

    -- 获取用户ID
    local user_id, user_err = utils.get_user_id_by_name(username)
    if not user_id then
        return false, user_err
    end

    -- 检查点赞状态
    local like_sql = string.format([[
        SELECT 'like' as status
        FROM likes
        WHERE user_id = %d AND target_type = '%s' AND target_id = '%s'
    ]], user_id, db.escape(target_type), db.escape(target_id))
    local like_result = db.execute_query(like_sql)

    if like_result.code then
        return false, "查询点赞状态失败: " .. like_result.message
    end

    if #like_result.data > 0 then
        return true, "like"
    end

    -- 检查点踩状态
    local dislike_sql = string.format([[
        SELECT 'dislike' as status
        FROM likes
        WHERE user_id = %d AND target_type = '%s' AND target_id = '%s'
    ]], user_id, "dislike_" .. db.escape(target_type), db.escape(target_id))
    local dislike_result = db.execute_query(dislike_sql)

    if dislike_result.code then
        return false, "查询点踩状态失败: " .. dislike_result.message
    end

    if #dislike_result.data > 0 then
        return true, "dislike"
    end

    return true, "none"
end

--- 处理点赞消息
-- @param fd WebSocket连接标识
-- @param msg_dec 消息内容
function like.handle_like_message(fd, msg_dec, users, sessions)
    local username, user_id, is_valid = check_user_and_params(fd, msg_dec, {"target_type", "target_id"})
    if not username then return end

    local target_type = msg_dec.target_type
    local target_id = msg_dec.target_id

    -- 检查是否已经点赞
    local check_sql = string.format(
        "SELECT 1 FROM likes WHERE user_id = %d AND target_type = '%s' AND target_id = '%s'",
        user_id, db.escape_sql(target_type), db.escape_sql(target_id))
    local check_result = db.execute_query(check_sql)

    if not check_result.code and #check_result.data > 0 then
        utils.send_message(fd, {
            type = "like",
            action = "like",
            success = false,
            message = "您已经点过赞了"
        })
        return
    end

    -- 添加点赞记录
    local like_sql = string.format(
        "INSERT INTO likes (user_id, target_type, target_id, created_at) VALUES (%d, '%s', '%s', %d)",
        user_id, db.escape_sql(target_type), db.escape_sql(target_id), moon.time())
    local like_result = db.execute_query(like_sql)

    if like_result.code then
        utils.send_message(fd, {
            type = "like",
            action = "like",
            success = false,
            message = "点赞失败: " .. like_result.message
        })
        return
    end

    -- 获取目标信息（如果是消息）
    if target_type == "message" then
        local message_sql = string.format(
            "SELECT user_id FROM messages WHERE id = %s", db.escape_sql(target_id))
        local message_result = db.execute_query(message_sql)

        if not message_result.code and #message_result.data > 0 then
            local message_owner_id = message_result.data[1].user_id

            -- 通知消息所有者被点赞
            for user_fd, user_info in pairs(users) do
                local target_user_id, target_user_err = utils.get_user_id_by_name(user_info.username)
                if target_user_id and target_user_id == message_owner_id then
                    utils.send_message(user_fd, {
                        type = "like",
                        action = "new_like",
                        from = username,
                        target_type = target_type,
                        target_id = target_id
                    })
                    break
                end
            end
        end
    end

    utils.send_message(fd, {
        type = "like",
        action = "like",
        success = true,
        message = "点赞成功"
    })
end

--- 处理取消点赞消息
-- @param fd WebSocket连接标识
-- @param msg_dec 消息内容
function like.handle_unlike_message(fd, msg_dec, users, sessions)
    local username, user_id, is_valid = check_user_and_params(fd, msg_dec, {"target_type", "target_id"})
    if not username then return end

    local target_type = msg_dec.target_type
    local target_id = msg_dec.target_id

    -- 检查是否已经点赞
    local check_sql = string.format(
        "SELECT 1 FROM likes WHERE user_id = %d AND target_type = '%s' AND target_id = '%s'",
        user_id, db.escape_sql(target_type), db.escape_sql(target_id))
    local check_result = db.execute_query(check_sql)

    if check_result.code or #check_result.data == 0 then
        utils.send_message(fd, {
            type = "like",
            action = "unlike",
            success = false,
            message = "您还没有点赞"
        })
        return
    end

    -- 删除点赞记录
    local unlike_sql = string.format(
        "DELETE FROM likes WHERE user_id = %d AND target_type = '%s' AND target_id = '%s'",
        user_id, db.escape_sql(target_type), db.escape_sql(target_id))
    local unlike_result = db.execute_query(unlike_sql)

    if unlike_result.code then
        utils.send_message(fd, {
            type = "like",
            action = "unlike",
            success = false,
            message = "取消点赞失败: " .. unlike_result.message
        })
        return
    end

    utils.send_message(fd, {
        type = "like",
        action = "unlike",
        success = true,
        message = "取消点赞成功"
    })
end

--- 获取点赞列表
-- @param fd WebSocket连接标识
-- @param msg_dec 消息内容
function like.handle_get_likes(fd, msg_dec, users, sessions)
    local username, user_id, is_valid = check_user_and_params(fd, msg_dec, {"target_type", "target_id"})
    if not username then return end

    local target_type = msg_dec.target_type
    local target_id = msg_dec.target_id

    -- 查询点赞列表
    local sql = string.format([[
        SELECT l.id, u.username, u.nickname, u.avatar, l.created_at
        FROM likes l
        JOIN users u ON l.user_id = u.id
        WHERE l.target_type = '%s' AND l.target_id = '%s'
        ORDER BY l.created_at DESC
    ]], db.escape_sql(target_type), db.escape_sql(target_id))
    local result = db.execute_query(sql)

    if result.code then
        utils.send_message(fd, {
            type = "like",
            action = "list",
            success = false,
            message = "获取点赞列表失败: " .. result.message
        })
        return
    end

    utils.send_message(fd, {
        type = "like",
        action = "list",
        success = true,
        likes = result.data
    })
end

--- 获取用户收到的点赞数
-- @param fd WebSocket连接标识
-- @param msg_dec 消息内容
function like.handle_get_user_likes(fd, msg_dec, users, sessions)
    local username, user_id, is_valid = check_user_and_params(fd, msg_dec, {})
    if not username then return end

    local target_type = msg_dec.target_type or "message"

    -- 查询用户收到的点赞数
    local sql = string.format([[
        SELECT COUNT(*) as like_count
        FROM likes l
        JOIN messages m ON l.target_id = m.id
        WHERE l.target_type = '%s' AND m.user_id = %d
    ]], db.escape_sql(target_type), user_id)
    local result = db.execute_query(sql)

    if result.code then
        utils.send_message(fd, {
            type = "like",
            action = "user_likes",
            success = false,
            message = "获取点赞数失败: " .. result.message
        })
        return
    end

    local like_count = result.data[1].like_count or 0

    utils.send_message(fd, {
        type = "like",
        action = "user_likes",
        success = true,
        like_count = like_count
    })
end

return like