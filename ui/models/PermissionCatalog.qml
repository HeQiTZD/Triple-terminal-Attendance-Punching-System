pragma Singleton

import QtQuick

QtObject {
    id: root

    readonly property var pageOrder: [
        "dashboard", "person", "device", "attendance", "face",
        "user", "rbac", "events", "history", "settings"
    ]

    readonly property var allNavItems: [
        { group: qsTr("总览") },
        { key: "dashboard", label: qsTr("仪表盘"), icon: "■" },

        { group: qsTr("数据管理") },
        { key: "person", label: qsTr("人员管理"), icon: "●" },
        { key: "device", label: qsTr("设备管理"), icon: "●" },
        { key: "attendance", label: qsTr("考勤记录"), icon: "●" },
        { key: "face", label: qsTr("人脸库"), icon: "●" },

        { group: qsTr("账号与权限") },
        { key: "user", label: qsTr("用户账号"), icon: "●" },
        { key: "rbac", label: qsTr("角色权限"), icon: "●" },

        { group: qsTr("系统") },
        { key: "events", label: qsTr("事件中心"), icon: "◆" },
        { key: "history", label: qsTr("调用历史"), icon: "≡" },
        { key: "settings", label: qsTr("连接与预设"), icon: "⚙" }
    ]

    function canAccessNav(key, sessionManager) {
        if (!sessionManager || !sessionManager.isLoggedIn)
            return false
        switch (key) {
        case "dashboard":
        case "history":
        case "settings":
            return true
        case "person":
            return sessionManager.hasPermission("person.read")
        case "device":
            return sessionManager.hasPermission("device.read")
        case "attendance":
            return sessionManager.hasPermission("attendance.read")
                || sessionManager.hasPermission("attendance.archive.read")
        case "face":
            return sessionManager.hasPermission("face.read")
        case "user":
            return sessionManager.hasPermission("user.read")
        case "rbac":
            return sessionManager.hasRole("super_admin")
        case "events":
            return sessionManager.hasPermission("event.subscribe")
        default:
            return false
        }
    }

    function filteredNavItems(sessionManager) {
        const out = []
        for (let i = 0; i < allNavItems.length; ++i) {
            const item = allNavItems[i]
            if (item.group) {
                const next = allNavItems[i + 1]
                if (next && next.key && canAccessNav(next.key, sessionManager))
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
                && !sessionManager.hasRole(requiredRole))
            return false
        if (requiredPermission && requiredPermission.length > 0
                && !sessionManager.hasPermission(requiredPermission))
            return false
        return true
    }

    function invoke(sessionManager, requiredPermission, requiredRole, callback, deniedDialog) {
        if (!isAllowed(sessionManager, requiredPermission, requiredRole)) {
            const hint = requiredRole && requiredRole.length
                ? ("role:" + requiredRole)
                : (requiredPermission || "login")
            if (deniedDialog)
                deniedDialog.openFor(hint)
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
        if (sessionManager.hasPermission("person.read")) labels.push(qsTr("人员"))
        if (sessionManager.hasPermission("device.read")) labels.push(qsTr("设备"))
        if (sessionManager.hasPermission("attendance.read")) labels.push(qsTr("考勤"))
        if (sessionManager.hasPermission("face.read")) labels.push(qsTr("人脸"))
        if (sessionManager.hasPermission("user.read")) labels.push(qsTr("用户"))
        if (sessionManager.hasRole("super_admin")) labels.push(qsTr("RBAC"))
        if (sessionManager.hasPermission("event.subscribe")) labels.push(qsTr("事件"))
        return labels.join(" · ")
    }
}
