import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceAdmin

ApplicationWindow {
    id: win

    readonly property int loginWindowWidth: 400
    readonly property int loginWindowHeight: 542
    readonly property int mainWindowWidth: 1480
    readonly property int mainWindowHeight: 920

    width: sessionManager.isLoggedIn ? mainWindowWidth : loginWindowWidth
    height: sessionManager.isLoggedIn ? mainWindowHeight : loginWindowHeight
    minimumWidth: sessionManager.isLoggedIn ? mainWindowWidth : loginWindowWidth
    minimumHeight: sessionManager.isLoggedIn ? mainWindowHeight : loginWindowHeight
    maximumWidth: sessionManager.isLoggedIn ? mainWindowWidth : loginWindowWidth
    maximumHeight: sessionManager.isLoggedIn ? mainWindowHeight : loginWindowHeight
    flags: Qt.Window | Qt.FramelessWindowHint
    visible: true
    title: sessionManager.isLoggedIn
           ? qsTr("AttendanceAdmin · 考勤管理端")
           : qsTr("AttendanceAdmin · 登录")

    required property var sessionManager
    required property var tcpManager
    required property var personServer
    required property var deviceServer
    required property var configDeployServer
    required property var attendanceService
    required property var faceServer
    required property var rbacServer
    required property var eventService
    required property var userServer
    required property var logWriter
    required property var fileService

    color: Theme.bg
    palette.window:           Theme.bg
    palette.windowText:       Theme.text
    palette.base:             Theme.surface
    palette.alternateBase:    Theme.surfaceAlt
    palette.text:             Theme.text
    palette.button:           Theme.surfaceAlt
    palette.buttonText:       Theme.text
    palette.highlight:        Theme.accent
    palette.highlightedText:  "#FFFFFF"
    palette.toolTipBase:      Theme.surface
    palette.toolTipText:      Theme.text
    palette.placeholderText:  Theme.textSubtle
    palette.mid:              Theme.border
    palette.midlight:         Theme.borderStrong
    palette.shadow:           "#A0AAB8"
    palette.dark:             Theme.borderStrong
    palette.light:            Theme.surfaceAlt

    property string currentPageKey: "dashboard"

    readonly property var filteredNavItems: {
        if (sessionManager) {
            const _p = sessionManager.permissions
            const _r = sessionManager.roles
            void _p
            void _r
        }
        return PermissionCatalog.filteredNavItems(sessionManager)
    }

    readonly property int sidebarCurrentIndex: {
        for (let i = 0; i < filteredNavItems.length; ++i) {
            if (filteredNavItems[i].key === currentPageKey)
                return i
        }
        return 0
    }

    function _historyPayload(obj) {
        return JSON.stringify(obj, null, 2)
    }

    function _recordTcp(direction, message, ok, resultText) {
        const type = message.type || message["type"] || ""
        History.record({
            direction: direction,
            target: type,
            payload: _historyPayload(message),
            ok: ok,
            result: resultText || (ok ? "OK" : "FAIL"),
            category: "tcp"
        })
    }

    function _handleServiceOp(apiType, code, message, category) {
        const text = ErrorCatalog.messageForCode(code, message)
        const ok = code === 0
        History.record({
            direction: "INVOKE",
            target: apiType,
            payload: apiType,
            ok: ok,
            result: text,
            category: category || "other"
        })
        if (ok)
            Logger.ok(apiType + ": " + text)
        else
            Logger.error(apiType + ": " + text)
    }

    function navigateTo(key) {
        if (!PermissionCatalog.canAccessNav(key, sessionManager)) {
            permissionDenied.openFor(
                PermissionCatalog.navLabelForKey(key) + " — "
                + PermissionCatalog.accessRequirementHint(key))
            return
        }
        currentPageKey = key
    }

    function _syncPageFromPermissions() {
        if (!PermissionCatalog.canAccessNav(currentPageKey, sessionManager))
            currentPageKey = PermissionCatalog.defaultPageKey(sessionManager)
    }

    function _applyWindowMode() {
        if (sessionManager.isLoggedIn) {
            minimumWidth = mainWindowWidth
            minimumHeight = mainWindowHeight
            maximumWidth = mainWindowWidth
            maximumHeight = mainWindowHeight
            width = mainWindowWidth
            height = mainWindowHeight
        } else {
            minimumWidth = loginWindowWidth
            minimumHeight = loginWindowHeight
            maximumWidth = loginWindowWidth
            maximumHeight = loginWindowHeight
            width = loginWindowWidth
            height = loginWindowHeight
        }
    }

    Component.onCompleted: {
        _applyWindowMode()
        Logger.info("管理端已启动")
    }

    PermissionDeniedDialog { id: permissionDenied }

    Connections {
        target: sessionManager
        function onPermissionsChanged() { win._syncPageFromPermissions() }
        function onRolesChanged() { win._syncPageFromPermissions() }
        function onLoggedInChanged() { win._applyWindowMode() }
        function onLoggedIn() {
            win.currentPageKey = PermissionCatalog.defaultPageKey(win.sessionManager)
            win._syncPageFromPermissions()
        }
        function onLoggedOut() {
            win.currentPageKey = "dashboard"
            win._applyWindowMode()
        }
    }

    Connections {
        target: tcpManager
        function onJsonMessageSent(message) {
            win._recordTcp("SEND", message, true, "sent")
        }
        function onJsonMessageReceived(message) {
            const code = message.code !== undefined ? message.code : -1
            const ok = code === 0 || code === undefined
            win._recordTcp("RECV", message, ok, message.msg || "")
        }
    }

    Connections {
        target: eventService
        function onServerPushReceived(messageType, data) {
            const summary = JSON.stringify(data)
            PushFeed.recordPush(messageType, summary)
            History.record({
                direction: "RECV",
                target: messageType,
                payload: summary,
                ok: true,
                result: "push",
                category: "push"
            })
        }
    }

    StackLayout {
        anchors.fill: parent
        currentIndex: sessionManager.isLoggedIn ? 1 : 0

        LoginPage {
            Layout.fillWidth: true
            Layout.fillHeight: true
            sessionManager: win.sessionManager
        }

        SplitView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            orientation: Qt.Horizontal

            SideBar {
                id: side
                SplitView.preferredWidth: Theme.sideBarWidth
                SplitView.minimumWidth: 180
                SplitView.maximumWidth: 350
                items: win.filteredNavItems
                currentIndex: win.sidebarCurrentIndex
                sessionManager: win.sessionManager
                showLogout: sessionManager.isLoggedIn
                onNavigated: function(idx) {
                    const item = win.filteredNavItems[idx]
                    if (item && item.key)
                        win.navigateTo(item.key)
                }
                onLogoutRequested: sessionManager.logout()
            }

            ColumnLayout {
                SplitView.fillWidth: true
                SplitView.fillHeight: true
                spacing: 0

                StatusBar {
                    Layout.fillWidth: true
                    sessionManager: win.sessionManager
                }

                PageHost {
                    id: pageHost
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    pageKey: win.currentPageKey
                    sessionManager: win.sessionManager
                    personServer: win.personServer
                    deviceServer: win.deviceServer
                    configDeployServer: win.configDeployServer
                    attendanceService: win.attendanceService
                    faceServer: win.faceServer
                    userServer: win.userServer
                    rbacServer: win.rbacServer
                    eventService: win.eventService
                    logWriter: win.logWriter
                    fileService: win.fileService
                    deniedDialog: permissionDenied
                    onServiceResult: (apiType, code, msg, category) =>
                        win._handleServiceOp(apiType, code, msg, category)
                }
            }
        }
    }

}
