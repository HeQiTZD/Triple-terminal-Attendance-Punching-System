pragma Singleton

import QtQuick

QtObject {
    id: root

    readonly property var pageOrder: [
        "dashboard", "person", "device", "configDeploy", "attendance", "face",
        "user", "rbac"
    ]

    function roleDisplayName(roleKey) {
        if (!roleKey)
            return ""
        switch (roleKey) {
        case "super_admin":        return qsTr("超级管理员")
        case "hr_admin":           return qsTr("人事管理员")
        case "device_admin":       return qsTr("设备管理员")
        case "attendance_auditor": return qsTr("考勤审核员")
        case "viewer":             return qsTr("只读观察者")
        default:                   return roleKey
        }
    }

    function permissionDisplayName(permKey) {
        if (!permKey)
            return ""
        switch (permKey) {
        case "person.create":             return qsTr("新增人员")
        case "person.read":               return qsTr("查询人员")
        case "person.update":             return qsTr("修改人员")
        case "person.delete":             return qsTr("删除人员")
        case "attendance.create":         return qsTr("录入考勤")
        case "attendance.read":           return qsTr("查询考勤")
        case "attendance.update":         return qsTr("修改考勤")
        case "attendance.delete":         return qsTr("删除考勤")
        case "attendance.archive.read":   return qsTr("查询归档考勤")
        case "attendance.archive.delete": return qsTr("删除归档考勤")
        case "device.create":             return qsTr("新增设备")
        case "device.read":               return qsTr("查询设备")
        case "device.update":             return qsTr("修改设备")
        case "device.delete":             return qsTr("删除设备")
        case "device.command":            return qsTr("设备指令")
        case "config.deploy":             return qsTr("配置下发")
        case "user.create":               return qsTr("新增用户")
        case "user.read":                 return qsTr("查询用户")
        case "user.update":               return qsTr("修改用户")
        case "user.delete":               return qsTr("删除用户")
        case "face.register":             return qsTr("注册人脸")
        case "face.read":                 return qsTr("查询人脸")
        case "face.delete":               return qsTr("删除人脸")
        case "event.subscribe":           return qsTr("订阅事件")
        default:                          return permKey
        }
    }

    // 必须通过 Q_PROPERTY（permissions / roles）判断权限，勿用 hasPermission/hasRole，
    // 否则 QML 绑定不会在登录切换或热更新后刷新。
    function _permissions(sessionManager) {
        return (sessionManager && sessionManager.permissions)
                ? sessionManager.permissions : []
    }

    function _roles(sessionManager) {
        return (sessionManager && sessionManager.roles)
                ? sessionManager.roles : []
    }

    function hasPerm(sessionManager, permKey) {
        if (!permKey || permKey.length === 0)
            return true
        return _permissions(sessionManager).indexOf(permKey) >= 0
    }

    function hasRoleKey(sessionManager, roleKey) {
        if (!roleKey || roleKey.length === 0)
            return true
        return _roles(sessionManager).indexOf(roleKey) >= 0
    }

    function hasAnyModuleRead(sessionManager) {
        const keys = [
            "person.read", "device.read", "attendance.read",
            "attendance.archive.read", "face.read", "user.read", "event.subscribe"
        ]
        for (let i = 0; i < keys.length; ++i) {
            if (hasPerm(sessionManager, keys[i]))
                return true
        }
        return hasRoleKey(sessionManager, "super_admin")
    }

    function formatRoles(sessionManager) {
        const roles = _roles(sessionManager)
        if (!roles.length)
            return qsTr("未分配角色")
        const parts = []
        for (let i = 0; i < roles.length; ++i)
            parts.push(roleDisplayName(roles[i]))
        return parts.join("、")
    }

    function navLabelForKey(key) {
        for (let i = 0; i < allNavItems.length; ++i) {
            const item = allNavItems[i]
            if (item.key === key)
                return item.label
        }
        return key
    }

    function accessRequirementHint(key) {
        switch (key) {
        case "person":     return permissionDisplayName("person.read")
        case "device":     return permissionDisplayName("device.read")
        case "configDeploy": return permissionDisplayName("config.deploy")
        case "attendance": return permissionDisplayName("attendance.read")
                             + " / " + permissionDisplayName("attendance.archive.read")
        case "face":       return permissionDisplayName("face.read")
        case "user":       return permissionDisplayName("user.read")
        case "rbac":       return roleDisplayName("super_admin")
        default:           return ""
        }
    }

    readonly property var allNavItems: [
        { group: qsTr("总览") },
        { key: "dashboard", label: qsTr("仪表盘"), icon: "◆" },

        { group: qsTr("数据管理") },
        { key: "person", label: qsTr("人员管理"), icon: "☺" },
        { key: "device", label: qsTr("设备管理"), icon: "⬡" },
        { key: "configDeploy", label: qsTr("配置下发"), icon: "⇧" },
        { key: "attendance", label: qsTr("考勤记录"), icon: "☰" },
        { key: "face", label: qsTr("人脸库"), icon: "◎" },

        { group: qsTr("账号与权限") },
        { key: "user", label: qsTr("用户账号"), icon: "⚿" },
        { key: "rbac", label: qsTr("用户权限"), icon: "⛨" },

        { group: qsTr("系统") },
        { key: "events", label: qsTr("事件中心"), icon: "⚡" },
        { key: "history", label: qsTr("调用历史"), icon: "≡" },
    ]

    function canAccessNav(key, sessionManager) {
        if (!sessionManager || !sessionManager.isLoggedIn)
            return false
        switch (key) {
        case "dashboard":
            return true
        case "person":
            return hasPerm(sessionManager, "person.read")
        case "device":
            return hasPerm(sessionManager, "device.read")
        case "configDeploy":
            return hasPerm(sessionManager, "config.deploy")
        case "attendance":
            return hasPerm(sessionManager, "attendance.read")
                || hasPerm(sessionManager, "attendance.archive.read")
        case "face":
            return hasPerm(sessionManager, "face.read")
        case "user":
            return hasPerm(sessionManager, "user.read")
        case "rbac":
            return hasRoleKey(sessionManager, "super_admin")
        default:
            return false
        }
    }

    function canViewFullPermissions(sessionManager) {
        return hasPerm(sessionManager, "user.read")
            || hasRoleKey(sessionManager, "super_admin")
    }

    function accessiblePageKeys(sessionManager) {
        const out = []
        for (let i = 0; i < pageOrder.length; ++i) {
            const key = pageOrder[i]
            if (canAccessNav(key, sessionManager))
                out.push(key)
        }
        return out
    }

    function defaultPageKey(sessionManager) {
        if (canAccessNav("dashboard", sessionManager))
            return "dashboard"
        const keys = accessiblePageKeys(sessionManager)
        return keys.length > 0 ? keys[0] : "dashboard"
    }

    function filteredNavItems(sessionManager) {
        const out = []
        for (let i = 0; i < allNavItems.length; ++i) {
            const item = allNavItems[i]
            if (item.group) {
                let groupHasVisible = false
                for (let j = i + 1; j < allNavItems.length; ++j) {
                    const next = allNavItems[j]
                    if (next.group)
                        break
                    if (next.key && canAccessNav(next.key, sessionManager)) {
                        groupHasVisible = true
                        break
                    }
                }
                if (groupHasVisible)
                    out.push(item)
            } else if (item.key && canAccessNav(item.key, sessionManager)) {
                out.push(item)
            }
        }
        return out
    }

    function stackIndexForKey(key) {
        const idx = pageOrder.indexOf(key)
        return idx >= 0 ? idx : 0
    }

    function isAllowed(sessionManager, requiredPermission, requiredRole) {
        if (!sessionManager || !sessionManager.isLoggedIn)
            return false
        if (requiredRole && requiredRole.length > 0
                && !hasRoleKey(sessionManager, requiredRole))
            return false
        if (requiredPermission && requiredPermission.length > 0
                && !hasPerm(sessionManager, requiredPermission))
            return false
        return true
    }

    function invoke(sessionManager, requiredPermission, requiredRole, callback, deniedDialog) {
        if (!isAllowed(sessionManager, requiredPermission, requiredRole)) {
            const hint = requiredRole && requiredRole.length
                ? roleDisplayName(requiredRole)
                : permissionDisplayName(requiredPermission || "")
            if (deniedDialog)
                deniedDialog.openFor(hint || "login")
            Logger.warn(qsTr("权限不足：") + hint)
            return false
        }
        if (callback)
            callback()
        return true
    }

    function validatePassword(password) {
        const pw = String(password || "")
        if (pw.length < 8)
            return qsTr("密码至少 8 个字符")
        if (!/[A-Za-z]/.test(pw) || !/[0-9]/.test(pw))
            return qsTr("密码须同时包含字母和数字")
        return ""
    }

    function managedModulesSummary(sessionManager) {
        if (!sessionManager || !sessionManager.isLoggedIn)
            return ""
        const labels = []
        if (hasPerm(sessionManager, "person.read")) labels.push(qsTr("人员"))
        if (hasPerm(sessionManager, "device.read")) labels.push(qsTr("设备"))
        if (hasPerm(sessionManager, "attendance.read")
                || hasPerm(sessionManager, "attendance.archive.read"))
            labels.push(qsTr("考勤"))
        if (hasPerm(sessionManager, "face.read")) labels.push(qsTr("人脸"))
        if (hasPerm(sessionManager, "user.read")) labels.push(qsTr("用户"))
        if (hasRoleKey(sessionManager, "super_admin")) labels.push(qsTr("RBAC"))
        return labels.length ? labels.join(" · ") : qsTr("仅概览与连接设置")
    }

    function primaryRoleKey(sessionManager) {
        const roles = _roles(sessionManager)
        if (!roles.length)
            return ""
        const priority = [
            "super_admin", "hr_admin", "device_admin",
            "attendance_auditor", "viewer"
        ]
        for (let p = 0; p < priority.length; ++p) {
            if (roles.indexOf(priority[p]) >= 0)
                return priority[p]
        }
        return roles[0]
    }

    function welcomeTitle(sessionManager) {
        const role = primaryRoleKey(sessionManager)
        if (!role)
            return qsTr("欢迎使用考勤管理端")
        return qsTr("欢迎，%1").arg(roleDisplayName(role))
    }
}
