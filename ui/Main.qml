import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceServer

ApplicationWindow {
    id: win
    width: 1480
    height: 920
    minimumWidth: 1100
    minimumHeight: 700
    visible: true
    title: qsTr("AttendanceServer · 测试工作台")

    // ===== 深色调色板 =====
    color: Theme.bg
    palette.window:           Theme.bg
    palette.windowText:       Theme.text
    palette.base:             Theme.surface
    palette.alternateBase:    Theme.surfaceAlt
    palette.text:             Theme.text
    palette.button:           Theme.surface
    palette.buttonText:       Theme.text
    palette.highlight:        Theme.accent
    palette.highlightedText:  "#FFFFFF"
    palette.toolTipBase:      Theme.surface
    palette.toolTipText:      Theme.text
    palette.placeholderText:  Theme.textSubtle
    palette.mid:              Theme.border
    palette.midlight:         Theme.borderStrong
    palette.shadow:           "#000000"
    palette.dark:             Theme.bg
    palette.light:            Theme.surfaceAlt

    // ===== 启动信号桥接（保持原 testApi/tcpServer 等的连接） =====
    Connections {
        target: testApi
        function onLastErrorChanged() {
            if (testApi.lastError.length)
                Logger.error(testApi.lastError)
        }
    }
    Connections {
        target: exportManager
        function onLastErrorChanged() {
            if (exportManager.lastError.length)
                Logger.error("导出错误：" + exportManager.lastError)
        }
    }
    Connections {
        target: tcpServer
        function onClientConnected(deviceId, ipAddress) { Logger.info("客户端连接：" + deviceId + " @ " + ipAddress) }
        function onClientDisconnected(deviceId) { Logger.info("客户端断开：" + deviceId) }
        function onAttendanceRecordReceived(record) {
            try { Logger.info("收到考勤上报：" + JSON.stringify(record)) }
            catch (e) { Logger.info("收到考勤上报") }
        }
        function onDeviceStatusReceived(deviceId, status) {
            try { Logger.info("设备状态上报：" + deviceId + " => " + JSON.stringify(status)) }
            catch (e) { Logger.info("设备状态上报：" + deviceId) }
        }
        function onErrorOccurred(errorString) { Logger.error("TCP：" + errorString) }
        function onIsRunningChanged() { Logger.info("TCP 运行状态=" + tcpServer.isRunning) }
    }
    Connections {
        target: dataService
        function onOperationTip(message) { if (message.length) Logger.info(message) }
        function onOperationResult(ok, message) {
            if (ok) Logger.ok(message)
            else Logger.error(message)
        }
    }
    Connections {
        target: dataManager
        function onConnectionStateChanged() { Logger.info("数据库连接状态=" + dataManager.isConnected) }
        function onErrorOccurred(errorString) { Logger.error("数据库：" + errorString) }
        function onPersonAdded(id) { Logger.info("人员已新增 id=" + id) }
        function onPersonDeleted(id) { Logger.info("人员已删除 id=" + id) }
        function onAttendanceRecordAdded(id) { Logger.info("考勤记录已新增 id=" + id) }
        function onDeviceStatusChanged(deviceId, status) { Logger.info("设备状态变更：" + deviceId + " => " + status) }
        function onFaceDataAdded(id, employeeId) { Logger.info("人脸数据已新增 id=" + id + " 工号=" + employeeId) }
        function onFaceDataUpdated(id, employeeId) { Logger.info("人脸数据已更新 id=" + id + " 工号=" + employeeId) }
        function onFaceDataDeleted(id) { Logger.info("人脸数据已删除 id=" + id) }
    }

    Component.onCompleted: Logger.info("QML 测试工作台已启动")

    // ===== 导航项 =====
    readonly property var navItems: [
        { group: qsTr("总览") },
        { key: "dashboard", label: qsTr("仪表盘"),    icon: "■" },

        { group: qsTr("通信") },
        { key: "tcp",       label: qsTr("TCP 服务器"), icon: "→" },
        { key: "sync",      label: qsTr("人员同步"),   icon: "⇄" },

        { group: qsTr("数据") },
        { key: "person",    label: qsTr("人员管理"),   icon: "●" },
        { key: "device",    label: qsTr("设备管理"),   icon: "●" },
        { key: "attendance",label: qsTr("考勤记录"),   icon: "●" },
        { key: "face",      label: qsTr("人脸库"),     icon: "●" },

        { group: qsTr("分析与导出") },
        { key: "analytics", label: qsTr("统计分析"),   icon: "Σ" },
        { key: "export",    label: qsTr("CSV 导出"),   icon: "↓" },

        { group: qsTr("测试增强") },
        { key: "batch",     label: qsTr("批量测试"),   icon: "▶" },
        { key: "history",   label: qsTr("调用历史"),   icon: "≡" },
        { key: "settings",  label: qsTr("配置预设"),   icon: "⚙" }
    ]

    function _findKeyIndex(key) {
        for (let i = 0; i < navItems.length; ++i)
            if (navItems[i].key === key) return i
        return -1
    }

    property int currentNavIndex: _findKeyIndex("dashboard")

    // 把侧边栏 currentIndex（含 group 占位）映射到 StackLayout 的页面索引
    function _stackIndexFromNav(navIdx) {
        if (navIdx < 0 || navIdx >= navItems.length) return 0
        if (navItems[navIdx].group) return 0
        let idx = -1
        for (let i = 0; i <= navIdx; ++i)
            if (!navItems[i].group) ++idx
        return Math.max(0, idx)
    }

    // 主体
    SplitView {
        anchors.fill: parent
        orientation: Qt.Vertical

        // 上：侧边栏 + 内容
        SplitView {
            SplitView.fillWidth: true
            SplitView.fillHeight: true
            SplitView.preferredHeight: 700
            orientation: Qt.Horizontal

            SideBar {
                id: side
                SplitView.preferredWidth: Theme.sideBarWidth
                SplitView.minimumWidth: 180
                items: win.navItems
                currentIndex: win.currentNavIndex
                onNavigated: function(idx) {
                    if (!win.navItems[idx].group)
                        win.currentNavIndex = idx
                }
            }

            ColumnLayout {
                SplitView.fillWidth: true
                SplitView.fillHeight: true
                spacing: 0

                StatusBar {
                    Layout.fillWidth: true
                    dbConnected: dataService.isConnected
                    tcpRunning: tcpServer.isRunning
                    tcpPort: Presets.tcpPort
                    clientCount: tcpServer.clientCount
                    lastError: Logger.lastError
                    lastInfo: Logger.lastInfo
                }

                StackLayout {
                    id: stack
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    currentIndex: win._stackIndexFromNav(win.currentNavIndex)

                    PageDashboard   {}
                    PageTcpServer   {}
                    PageSync        {}
                    PagePerson      {}
                    PageDevice      {}
                    PageAttendance  {}
                    PageFace        {}
                    PageAnalytics   {}
                    PageExport      {}
                    PageBatchTest   {}
                    PageHistory     {}
                    PageSettings    {}
                }
            }
        }

        // 下：日志面板
        LogPanel {
            SplitView.preferredHeight: 200
            SplitView.minimumHeight: 100
            SplitView.maximumHeight: 600
        }
    }
}
