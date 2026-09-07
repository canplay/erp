-- 定义 group 模块
local group = {}
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
            type = "group",
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
            type = "group",
            action = msg_dec.action or "unknown",
            success = false,
            message = username_or_err
        })
        return nil, nil, nil
    end

    local user_id, user_err = utils.get_user_id_by_name(username_or_err)
    if not user_id then
        utils.send_message(fd, {
            type = "group",
            action = msg_dec.action or "unknown",
            success = false,
            message = user_err
        })
        return nil, nil, nil
    end

    return username_or_err, user_id, is_valid
end

--- 创建群组
-- @param creator 创建者
-- @param group_name 群组名称
-- @param description 群组描述
function group.create(creator, group_name, description)
    -- 检查参数
    if not creator or not group_name then
        return false, "创建者和群组名称不能为空"
    end

    -- 检查数据库连接
    if not db.postgresql.is_ready() then
        return false, "系统初始化中，请稍后再试"
    end

    -- 获取创建者ID
    local creator_id, creator_err = utils.get_user_id_by_name(creator)
    if not creator_id then
        return false, creator_err
    end

    -- 创建群组
    local desc_escaped = description and db.escape(description) or ""
    local sql = string.format(
        "INSERT INTO groups (name, description, creator_id) VALUES ('%s', '%s', %d) RETURNING id",
        db.escape(group_name), desc_escaped, creator_id)
    local create_result = db.execute_query(sql)

    if create_result.code then
        return false, "创建群组失败: " .. create_result.message
    end

    local group_id = create_result.data[1].id

    -- 将创建者添加为群组成员（管理员）
    local member_sql = string.format(
        "INSERT INTO group_members (group_id, user_id, role) VALUES (%d, %d, 'admin')",
        group_id, creator_id)
    local member_result = db.execute_query(member_sql)

    if member_result.code then
        return false, "添加群组成员失败: " .. member_result.message
    end

    return true, { group_id = group_id, name = group_name, description = description }
end

--- 解散群组
-- @param username 操作用户
-- @param group_id 群组ID
function group.dismiss(username, group_id)
    -- 检查参数
    if not username or not group_id then
        return false, "用户名和群组ID不能为空"
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

    -- 检查群组是否存在且用户是管理员
    local check_sql = string.format([[
        SELECT g.id
        FROM groups g
        JOIN group_members gm ON g.id = gm.group_id
        WHERE g.id = %d AND gm.user_id = %d AND gm.role = 'admin'
    ]], tonumber(group_id), user_id)
    local check_result = db.execute_query(check_sql)

    if check_result.code then
        return false, "检查群组权限失败: " .. check_result.message
    end

    if #check_result.data == 0 then
        return false, "群组不存在或您不是管理员"
    end

    -- 删除所有群组成员
    local delete_members_sql = string.format("DELETE FROM group_members WHERE group_id = %d", tonumber(group_id))
    local delete_members_result = db.execute_query(delete_members_sql)

    if delete_members_result.code then
        return false, "删除群组成员失败: " .. delete_members_result.message
    end

    -- 删除群组
    local delete_group_sql = string.format("DELETE FROM groups WHERE id = %d", tonumber(group_id))
    local delete_group_result = db.execute_query(delete_group_sql)

    if delete_group_result.code then
        return false, "解散群组失败: " .. delete_group_result.message
    end

    return true, "群组解散成功"
end

--- 加入群组
-- @param username 用户名
-- @param group_id 群组ID
function group.join(username, group_id)
    -- 检查参数
    if not username or not group_id then
        return false, "用户名和群组ID不能为空"
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

    -- 检查群组是否存在
    local check_group_sql = string.format("SELECT id, name FROM groups WHERE id = %d", tonumber(group_id))
    local check_group_result = db.execute_query(check_group_sql)

    if check_group_result.code then
        return false, "检查群组失败: " .. check_group_result.message
    end

    if #check_group_result.data == 0 then
        return false, "群组不存在"
    end

    local group_info = check_group_result.data[1]

    -- 检查是否已经是群组成员
    local check_member_sql = string.format("SELECT id FROM group_members WHERE group_id = %d AND user_id = %d",
        tonumber(group_id), user_id)
    local check_member_result = db.execute_query(check_member_sql)

    if check_member_result.code then
        return false, "检查群组成员失败: " .. check_member_result.message
    end

    if #check_member_result.data > 0 then
        return false, "您已经是该群组成员"
    end

    -- 添加为群组成员
    local insert_sql = string.format("INSERT INTO group_members (group_id, user_id, role) VALUES (%d, %d, 'member')",
        tonumber(group_id), user_id)
    local insert_result = db.execute_query(insert_sql)

    if insert_result.code then
        return false, "加入群组失败: " .. insert_result.message
    end

    return true, { message = "成功加入群组: " .. group_info.name, group_id = group_info.id }
end

--- 退出群组
-- @param username 用户名
-- @param group_id 群组ID
function group.leave(username, group_id)
    -- 检查参数
    if not username or not group_id then
        return false, "用户名和群组ID不能为空"
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

    -- 检查是否是群组成员
    local check_member_sql = string.format("SELECT id, role FROM group_members WHERE group_id = %d AND user_id = %d",
        tonumber(group_id), user_id)
    local check_member_result = db.execute_query(check_member_sql)

    if check_member_result.code then
        return false, "检查群组成员失败: " .. check_member_result.message
    end

    if #check_member_result.data == 0 then
        return false, "您不是该群组成员"
    end

    local member_info = check_member_result.data[1]

    -- 检查是否是管理员
    if member_info.role == "admin" then
        -- 检查是否是最后一个管理员
        local check_admin_sql = string.format(
            "SELECT COUNT(*) as admin_count FROM group_members WHERE group_id = %d AND role = 'admin'",
            tonumber(group_id))
        local check_admin_result = db.execute_query(check_admin_sql)

        if check_admin_result.code then
            return false, "检查管理员信息失败: " .. check_admin_result.message
        end

        if check_admin_result.data[1].admin_count <= 1 then
            return false, "您是最后一个管理员，不能退出群组，请先转让管理员权限"
        end
    end

    -- 删除群组成员
    local delete_sql = string.format("DELETE FROM group_members WHERE group_id = %d AND user_id = %d",
        tonumber(group_id), user_id)
    local delete_result = db.execute_query(delete_sql)

    if delete_result.code then
        return false, "退出群组失败: " .. delete_result.message
    end

    return true, "退出群组成功"
end

--- 获取用户群组列表
-- @param username 用户名
function group.list_groups(username)
    -- 检查参数
    if not username then
        return false, "用户名不能为空"
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

    -- 查询用户加入的群组
    local sql = string.format([[
        SELECT g.id, g.name, g.description, gm.role, g.creator_id
        FROM groups g
        JOIN group_members gm ON g.id = gm.group_id
        WHERE gm.user_id = %d
        ORDER BY g.name
    ]], user_id)
    local result = db.execute_query(sql)

    if result.code then
        return false, "查询群组列表失败: " .. result.message
    end

    local groups_list = {}
    for _, row in ipairs(result.data) do
        -- 获取创建者用户名
        local creator_name, creator_err = utils.get_user_name_by_id(row.creator_id)
        if not creator_name then
            creator_name = "未知用户"
        end

        table.insert(groups_list, {
            id = row.id,
            name = row.name,
            description = row.description,
            role = row.role,
            creator = creator_name
        })
    end

    return true, groups_list
end

--- 获取群组成员列表
-- @param group_id 群组ID
function group.list_members(group_id)
    -- 检查参数
    if not group_id then
        return false, "群组ID不能为空"
    end

    -- 检查数据库连接
    if not db.postgresql.is_ready() then
        return false, "系统初始化中，请稍后再试"
    end

    -- 检查群组是否存在
    local check_group_sql = string.format("SELECT id FROM groups WHERE id = %d", tonumber(group_id))
    local check_group_result = db.execute_query(check_group_sql)

    if check_group_result.code then
        return false, "检查群组失败: " .. check_group_result.message
    end

    if #check_group_result.data == 0 then
        return false, "群组不存在"
    end

    -- 查询群组成员
    local sql = string.format([[
        SELECT u.id, u.username, gm.role
        FROM group_members gm
        JOIN users u ON gm.user_id = u.id
        WHERE gm.group_id = %d
        ORDER BY gm.role DESC, u.username
    ]], tonumber(group_id))
    local result = db.execute_query(sql)

    if result.code then
        return false, "查询群组成员失败: " .. result.message
    end

    local members_list = {}
    for _, row in ipairs(result.data) do
        table.insert(members_list, {
            id = row.id,
            username = row.username,
            role = row.role
        })
    end

    return true, members_list
end

--- 获取群组信息
-- @param group_id 群组ID
function group.get_info(group_id)
    -- 检查参数
    if not group_id then
        return false, "群组ID不能为空"
    end

    -- 检查数据库连接
    if not db.postgresql.is_ready() then
        return false, "系统初始化中，请稍后再试"
    end

    -- 查询群组信息
    local sql = string.format([[
        SELECT g.id, g.name, g.description, g.creator_id, g.created_at, u.username as creator_name
        FROM groups g
        JOIN users u ON g.creator_id = u.id
        WHERE g.id = %d
    ]], tonumber(group_id))
    local result = db.execute_query(sql)

    if result.code then
        return false, "查询群组信息失败: " .. result.message
    end

    if #result.data == 0 then
        return false, "群组不存在"
    end

    return true, result.data[1]
end

--- 转让群组管理员权限
-- @param username 当前管理员用户名
-- @param group_id 群组ID
-- @param new_admin_username 新管理员用户名
function group.transfer_ownership(username, group_id, new_admin_username)
    -- 检查参数
    if not username or not group_id or not new_admin_username then
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

    local new_admin_id, new_admin_err = utils.get_user_id_by_name(new_admin_username)
    if not new_admin_id then
        return false, new_admin_err
    end

    -- 检查群组是否存在且用户是管理员
    local check_sql = string.format([[
        SELECT g.id
        FROM groups g
        JOIN group_members gm ON g.id = gm.group_id
        WHERE g.id = %d AND gm.user_id = %d AND gm.role = 'admin'
    ]], tonumber(group_id), user_id)
    local check_result = db.execute_query(check_sql)

    if check_result.code then
        return false, "检查群组权限失败: " .. check_result.message
    end

    if #check_result.data == 0 then
        return false, "群组不存在或您不是管理员"
    end

    -- 检查目标用户是否是群组成员
    local check_member_sql = string.format([[
        SELECT id, role
        FROM group_members
        WHERE group_id = %d AND user_id = %d
    ]], tonumber(group_id), new_admin_id)
    local check_member_result = db.execute_query(check_member_sql)

    if check_member_result.code then
        return false, "检查成员信息失败: " .. check_member_result.message
    end

    if #check_member_result.data == 0 then
        return false, "目标用户不是群组成员"
    end

    -- 更新权限
    local update_old_admin_sql = string.format([[
        UPDATE group_members
        SET role = 'member'
        WHERE group_id = %d AND user_id = %d
    ]], tonumber(group_id), user_id)
    local update_old_admin_result = db.execute_query(update_old_admin_sql)

    if update_old_admin_result.code then
        return false, "更新权限失败: " .. update_old_admin_result.message
    end

    local update_new_admin_sql = string.format([[
        UPDATE group_members
        SET role = 'admin'
        WHERE group_id = %d AND user_id = %d
    ]], tonumber(group_id), new_admin_id)
    local update_new_admin_result = db.execute_query(update_new_admin_sql)

    if update_new_admin_result.code then
        return false, "更新权限失败: " .. update_new_admin_result.message
    end

    return true, "管理员权限转让成功"
end

--- 创建群组
-- @param fd WebSocket连接标识
-- @param msg_dec 消息内容
function group.handle_create_group(fd, msg_dec, users, sessions)
    local username, user_id, is_valid = check_user_and_params(fd, msg_dec, {"group_name"})
    if not username then return end

    local group_name = msg_dec.group_name
    local description = msg_dec.description or ""

    -- 创建群组
    local sql = string.format(
        "INSERT INTO groups (name, description, creator_id, created_at) VALUES ('%s', '%s', %d, %d)",
        db.escape_sql(group_name), db.escape_sql(description), user_id, moon.time())
    local result = db.execute_query(sql)

    if result.code then
        utils.send_message(fd, {
            type = "group",
            action = "create",
            success = false,
            message = "创建群组失败: " .. result.message
        })
        return
    end

    -- 获取创建的群组ID
    local group_id = result.last_insert_id

    -- 将创建者添加为群组管理员
    local member_sql = string.format(
        "INSERT INTO group_members (group_id, user_id, role, joined_at) VALUES (%d, %d, 'admin', %d)",
        group_id, user_id, moon.time())
    local member_result = db.execute_query(member_sql)

    if member_result.code then
        utils.send_message(fd, {
            type = "group",
            action = "create",
            success = false,
            message = "添加群组成员失败: " .. member_result.message
        })
        return
    end

    utils.send_message(fd, {
        type = "group",
        action = "create",
        success = true,
        message = "群组创建成功",
        group_id = group_id
    })
end

--- 加入群组
-- @param fd WebSocket连接标识
-- @param msg_dec 消息内容
function group.handle_join_group(fd, msg_dec, users, sessions)
    local username, user_id, is_valid = check_user_and_params(fd, msg_dec, {"group_id"})
    if not username then return end

    local group_id = msg_dec.group_id

    -- 检查群组是否存在
    local check_sql = string.format("SELECT 1 FROM groups WHERE id = %d", group_id)
    local check_result = db.execute_query(check_sql)

    if check_result.code or #check_result.data == 0 then
        utils.send_message(fd, {
            type = "group",
            action = "join",
            success = false,
            message = "群组不存在"
        })
        return
    end

    -- 检查是否已经是群组成员
    local member_check_sql = string.format(
        "SELECT 1 FROM group_members WHERE group_id = %d AND user_id = %d",
        group_id, user_id)
    local member_check_result = db.execute_query(member_check_sql)

    if not member_check_result.code and #member_check_result.data > 0 then
        utils.send_message(fd, {
            type = "group",
            action = "join",
            success = false,
            message = "您已经是该群组的成员"
        })
        return
    end

    -- 加入群组（普通成员）
    local join_sql = string.format(
        "INSERT INTO group_members (group_id, user_id, role, joined_at) VALUES (%d, %d, 'member', %d)",
        group_id, user_id, moon.time())
    local join_result = db.execute_query(join_sql)

    if join_result.code then
        utils.send_message(fd, {
            type = "group",
            action = "join",
            success = false,
            message = "加入群组失败: " .. join_result.message
        })
        return
    end

    utils.send_message(fd, {
        type = "group",
        action = "join",
        success = true,
        message = "成功加入群组"
    })
end

--- 更新群组信息
-- @param fd WebSocket连接标识
-- @param msg_dec 消息内容
function group.handle_update_group(fd, msg_dec, users, sessions)
    local username, user_id, is_valid = check_user_and_params(fd, msg_dec, {"group_id"})
    if not username then return end

    local group_id = msg_dec.group_id
    local group_name = msg_dec.group_name
    local description = msg_dec.description

    -- 检查是否是群主
    local check_sql = string.format(
        "SELECT 1 FROM groups WHERE id = %d AND creator_id = %d",
        group_id, user_id)
    local check_result = db.execute_query(check_sql)

    if check_result.code or #check_result.data == 0 then
        utils.send_message(fd, {
            type = "group",
            action = "update",
            success = false,
            message = "您不是该群组的群主"
        })
        return
    end

    -- 更新群组信息
    local update_fields = {}
    if group_name then
        table.insert(update_fields, string.format("name = '%s'", db.escape_sql(group_name)))
    end
    if description then
        table.insert(update_fields, string.format("description = '%s'", db.escape_sql(description)))
    end
    table.insert(update_fields, string.format("updated_at = %d", moon.time()))

    if #update_fields > 0 then
        local update_sql = string.format(
            "UPDATE groups SET %s WHERE id = %d",
            table.concat(update_fields, ", "), group_id)
        local update_result = db.execute_query(update_sql)

        if update_result.code then
            utils.send_message(fd, {
                type = "group",
                action = "update",
                success = false,
                message = "更新群组信息失败: " .. update_result.message
            })
            return
        end
    end

    utils.send_message(fd, {
        type = "group",
        action = "update",
        success = true,
        message = "群组信息更新成功"
    })
end

--- 发送群组消息
-- @param fd WebSocket连接标识
-- @param msg_dec 消息内容
function group.handle_group_message(fd, msg_dec, users, sessions)
    local username, user_id, is_valid = check_user_and_params(fd, msg_dec, {"group_id", "content"})
    if not username then return end

    local group_id = msg_dec.group_id
    local content = msg_dec.content

    -- 检查是否是群组成员
    local member_check_sql = string.format(
        "SELECT role FROM group_members WHERE group_id = %d AND user_id = %d",
        group_id, user_id)
    local member_check_result = db.execute_query(member_check_sql)

    if member_check_result.code or #member_check_result.data == 0 then
        utils.send_message(fd, {
            type = "group",
            action = "message",
            success = false,
            message = "您不是该群组的成员"
        })
        return
    end

    -- 保存消息到数据库
    local message_sql = string.format(
        "INSERT INTO group_messages (group_id, user_id, content, created_at) VALUES (%d, %d, '%s', %d)",
        group_id, user_id, db.escape_sql(content), moon.time())
    local message_result = db.execute_query(message_sql)

    if message_result.code then
        utils.send_message(fd, {
            type = "group",
            action = "message",
            success = false,
            message = "发送消息失败: " .. message_result.message
        })
        return
    end

    -- 广播消息给群组成员
    local members_sql = string.format(
        "SELECT user_id FROM group_members WHERE group_id = %d", group_id)
    local members_result = db.execute_query(members_sql)

    if not members_result.code then
        local message_data = {
            type = "group",
            action = "message",
            group_id = group_id,
            from = username,
            content = content,
            timestamp = moon.time()
        }

        for _, row in ipairs(members_result.data) do
            -- 发送给在线的群组成员
            for user_fd, user_info in pairs(users) do
                local target_user_id, target_user_err = utils.get_user_id_by_name(user_info.username)
                if target_user_id and target_user_id == row.user_id then
                    utils.send_message(user_fd, message_data)
                    break
                end
            end
        end
    end

    utils.send_message(fd, {
        type = "group",
        action = "message",
        success = true,
        message = "消息发送成功"
    })
end

--- 获取群组列表
-- @param fd WebSocket连接标识
-- @param msg_dec 消息内容
function group.handle_get_groups(fd, msg_dec, users, sessions)
    local username, user_id, is_valid = check_user_and_params(fd, msg_dec, {})
    if not username then return end

    -- 查询用户参与的群组
    local sql = string.format([[
        SELECT g.id, g.name, g.description, g.created_at, u.username as creator_name
        FROM group_members gm
        JOIN groups g ON gm.group_id = g.id
        JOIN users u ON g.creator_id = u.id
        WHERE gm.user_id = %d
        ORDER BY g.created_at DESC
    ]], user_id)
    local result = db.execute_query(sql)

    if result.code then
        utils.send_message(fd, {
            type = "group",
            action = "list",
            success = false,
            message = "获取群组列表失败: " .. result.message
        })
        return
    end

    -- 为每个群组添加创建者用户名
    for _, group_data in ipairs(result.data) do
        local creator_name, creator_err = utils.get_user_name_by_id(group_data.creator_id)
        if creator_name then
            group_data.creator_name = creator_name
        end
    end

    utils.send_message(fd, {
        type = "group",
        action = "list",
        success = true,
        groups = result.data
    })
end

--- 获取群组成员
-- @param fd WebSocket连接标识
-- @param msg_dec 消息内容
function group.handle_get_group_members(fd, msg_dec, users, sessions)
    local username, user_id, is_valid = check_user_and_params(fd, msg_dec, {"group_id"})
    if not username then return end

    local group_id = msg_dec.group_id

    -- 检查是否是群组成员
    local member_check_sql = string.format(
        "SELECT 1 FROM group_members WHERE group_id = %d AND user_id = %d",
        group_id, user_id)
    local member_check_result = db.execute_query(member_check_sql)

    if member_check_result.code or #member_check_result.data == 0 then
        utils.send_message(fd, {
            type = "group",
            action = "members",
            success = false,
            message = "您不是该群组的成员"
        })
        return
    end

    -- 查询群组成员
    local sql = string.format([[
        SELECT u.id, u.username, u.nickname, u.avatar, gm.role, gm.joined_at
        FROM group_members gm
        JOIN users u ON gm.user_id = u.id
        WHERE gm.group_id = %d
        ORDER BY gm.role DESC, gm.joined_at ASC
    ]], group_id)
    local result = db.execute_query(sql)

    if result.code then
        utils.send_message(fd, {
            type = "group",
            action = "members",
            success = false,
            message = "获取群组成员失败: " .. result.message
        })
        return
    end

    utils.send_message(fd, {
        type = "group",
        action = "members",
        success = true,
        members = result.data
    })
end

--- 退出群组
-- @param fd WebSocket连接标识
-- @param msg_dec 消息内容
function group.handle_leave_group(fd, msg_dec, users, sessions)
    local username, user_id, is_valid = check_user_and_params(fd, msg_dec, {"group_id"})
    if not username then return end

    local group_id = msg_dec.group_id

    -- 检查是否是群组成员
    local member_check_sql = string.format(
        "SELECT role FROM group_members WHERE group_id = %d AND user_id = %d",
        group_id, user_id)
    local member_check_result = db.execute_query(member_check_sql)

    if member_check_result.code or #member_check_result.data == 0 then
        utils.send_message(fd, {
            type = "group",
            action = "leave",
            success = false,
            message = "您不是该群组的成员"
        })
        return
    end

    -- 检查是否是群主（群主不能直接退出）
    if member_check_result.data[1].role == "owner" then
        utils.send_message(fd, {
            type = "group",
            action = "leave",
            success = false,
            message = "群主不能直接退出群组，请先转让群主身份"
        })
        return
    end

    -- 退出群组
    local leave_sql = string.format(
        "DELETE FROM group_members WHERE group_id = %d AND user_id = %d",
        group_id, user_id)
    local leave_result = db.execute_query(leave_sql)

    if leave_result.code then
        utils.send_message(fd, {
            type = "group",
            action = "leave",
            success = false,
            message = "退出群组失败: " .. leave_result.message
        })
        return
    end

    utils.send_message(fd, {
        type = "group",
        action = "leave",
        success = true,
        message = "成功退出群组"
    })
end

--- 转让群主身份
-- @param fd WebSocket连接标识
-- @param msg_dec 消息内容
function group.handle_transfer_ownership(fd, msg_dec, users, sessions)
    local username, user_id, is_valid = check_user_and_params(fd, msg_dec, {"group_id", "new_owner_username"})
    if not username then return end

    local group_id = msg_dec.group_id
    local new_owner_username = msg_dec.new_owner_username

    local new_owner_id, new_owner_err = utils.get_user_id_by_name(new_owner_username)
    if not new_owner_id then
        utils.send_message(fd, {
            type = "group",
            action = "transfer",
            success = false,
            message = new_owner_err
        })
        return
    end

    -- 检查是否是群组成员
    local member_check_sql = string.format(
        "SELECT role FROM group_members WHERE group_id = %d AND user_id = %d",
        group_id, user_id)
    local member_check_result = db.execute_query(member_check_sql)

    if member_check_result.code or #member_check_result.data == 0 then
        utils.send_message(fd, {
            type = "group",
            action = "transfer",
            success = false,
            message = "您不是该群组的成员"
        })
        return
    end

    -- 检查是否是群主
    if member_check_result.data[1].role ~= "owner" then
        utils.send_message(fd, {
            type = "group",
            action = "transfer",
            success = false,
            message = "只有群主可以转让群主身份"
        })
        return
    end

    -- 检查新群主是否是群组成员
    local new_owner_check_sql = string.format(
        "SELECT 1 FROM group_members WHERE group_id = %d AND user_id = %d",
        group_id, new_owner_id)
    local new_owner_check_result = db.execute_query(new_owner_check_sql)

    if new_owner_check_result.code or #new_owner_check_result.data == 0 then
        utils.send_message(fd, {
            type = "group",
            action = "transfer",
            success = false,
            message = "新群主必须是群组成员"
        })
        return
    end

    -- 转让群主身份
    local update_sql1 = string.format(
        "UPDATE group_members SET role = 'admin' WHERE group_id = %d AND user_id = %d",
        group_id, user_id)
    local update_sql2 = string.format(
        "UPDATE group_members SET role = 'owner' WHERE group_id = %d AND user_id = %d",
        group_id, new_owner_id)

    local update_result1 = db.execute_query(update_sql1)
    local update_result2 = db.execute_query(update_sql2)

    if update_result1.code or update_result2.code then
        utils.send_message(fd, {
            type = "group",
            action = "transfer",
            success = false,
            message = "转让群主身份失败"
        })
        return
    end

    utils.send_message(fd, {
        type = "group",
        action = "transfer",
        success = true,
        message = "成功转让群主身份"
    })
end

return group