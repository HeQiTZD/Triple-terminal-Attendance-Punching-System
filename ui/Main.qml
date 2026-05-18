import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceAdmin

ApplicationWindow {
    id: win
    width: 1480
    height: 920
    minimumWidth: 1100
    minimumHeight: 700
    visible: true
    title: qsTr("AttendanceAdmin · 考勤管理端")

    required property var sessionManager
    required property var tcpManager
    required property var personServer
    required property var deviceServer
    required property var attendanceService
    required property var faceServer
    required property var rbacServer
    required property var eventService
    required property var userServer

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

    readonly property var filteredNavItems: PermissionCatalog.filteredNavItems(sessionManager)

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
            permissionDenied.openFor(key)
            return
        }
        currentPageKey = key
    }

    function _syncPageFromPermissions() {
        if (!PermissionCatalog.canAccessNav(currentPageKey, sessionManager))
            currentPageKey = "dashboard"
    }

    Component.onCompleted: {
        Logger.info("管理端已启动")
        if (!sessionManager.isLoggedIn)
            loginDlg.open()
    }

    PermissionDeniedDialog { id: permissionDenied }

    Connections {
        target: sessionManager
        function onPermissionsChanged() { win._syncPageFromPermissions() }
        function onLoggedOut() { loginDlg.open() }
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

    LoginDialog {
        id: loginDlg
        sessionManager: win.sessionManager
    }

    SplitView {
        anchors.fill: parent
        orientation: Qt.Vertical

        SplitView {
            SplitView.fillWidth: true
            SplitView.fillHeight: true
            SplitView.preferredHeight: 700
            orientation: Qt.Horizontal

            SideBar {
                id: side
                SplitView.preferredWidth: Theme.sideBarWidth
                SplitView.minimumWidth: 180
                items: win.filteredNavItems
                currentIndex: win.sidebarCurrentIndex
                showLogout: sessionManager.isLoggedIn
                onNavigated: function(idx) {
                    const item = win.filteredNavItems[idx]
                    if (item && item.key)
                        win.currentPageKey = item.key
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
                    lastError: Logger.lastError
                    lastInfo: Logger.lastInfo
                }

                StackLayout {
                    id: stack
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    currentIndex: PermissionCatalog.stackIndexForKey(win.currentPageKey)

                    PageDashboard {
                        sessionManager: win.sessionManager
                        eventService: win.eventService
                        onNavigateRequested: function(key) { win.navigateTo(key) }
                    }
                    PagePerson {
                        personServer: win.personServer
                        sessionManager: win.sessionManager
                        deniedDialog: permissionDenied
                        onServiceResult: (apiType, code, msg) =>
                            win._handleServiceOp(apiType, code, msg, "person")
                    }
                    PageDevice {
                        deviceServer: win.deviceServer
                        sessionManager: win.sessionManager
                        deniedDialog: permissionDenied
                        onServiceResult: (apiType, code, msg) =>
                            win._handleServiceOp(apiType, code, msg, "device")
                    }
                    PageAttendance {
                        attendanceService: win.attendanceService
                        sessionManager: win.sessionManager
                        deniedDialog: permissionDenied
                        onServiceResult: (apiType, code, msg) =>
                            win._handleServiceOp(apiType, code, msg, "attendance")
                    }
                    PageFace {
                        faceServer: win.faceServer
                        sessionManager: win.sessionManager
                        deniedDialog: permissionDenied
                        onServiceResult: (apiType, code, msg) =>
                            win._handleServiceOp(apiType, code, msg, "face")
                    }
                    PageUser {
                        userServer: win.userServer
                        sessionManager: win.sessionManager
                        deniedDialog: permissionDenied
                        onServiceResult: (apiType, code, msg) =>
                            win._handleServiceOp(apiType, code, msg, "user")
                    }
                    PageRbac {
                        rbacServer: win.rbacServer
                        sessionManager: win.sessionManager
                        deniedDialog: permissionDenied
                        onServiceResult: (apiType, code, msg) =>
                            win._handleServiceOp(apiType, code, msg, "rbac")
                    }
                    PageEvents {
                        eventService: win.eventService
                        sessionManager: win.sessionManager
                        deniedDialog: permissionDenied
                        onServiceResult: (apiType, code, msg) =>
                            win._handleServiceOp(apiType, code, msg, "event")
                    }
                    PageHistory {}
                    PageSettings {
                        sessionManager: win.sessionManager
                        deniedDialog: permissionDenied
                    }
                }
            }
        }

        LogPanel {
            SplitView.preferredHeight: 200
            SplitView.minimumHeight: 100
            SplitView.maximumHeight: 600
        }
    }
}
