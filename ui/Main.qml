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

    Component.onCompleted: Logger.info("管理端已启动")

    readonly property var navItems: [
        { group: qsTr("总览") },
        { key: "dashboard", label: qsTr("仪表盘"),    icon: "■" },

        { group: qsTr("数据管理") },
        { key: "person",     label: qsTr("人员管理"),   icon: "●" },
        { key: "device",     label: qsTr("设备管理"),   icon: "●" },
        { key: "attendance", label: qsTr("考勤记录"),   icon: "●" },
        { key: "face",       label: qsTr("人脸库"),     icon: "●" },

        { group: qsTr("系统") },
        { key: "history",   label: qsTr("调用历史"),   icon: "≡" },
        { key: "settings",  label: qsTr("配置预设"),   icon: "⚙" }
    ]

    function _findKeyIndex(key) {
        for (let i = 0; i < navItems.length; ++i)
            if (navItems[i].key === key) return i
        return -1
    }

    property int currentNavIndex: _findKeyIndex("dashboard")

    function _stackIndexFromNav(navIdx) {
        if (navIdx < 0 || navIdx >= navItems.length) return 0
        if (navItems[navIdx].group) return 0
        let idx = -1
        for (let i = 0; i <= navIdx; ++i)
            if (!navItems[i].group) ++idx
        return Math.max(0, idx)
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
                    lastError: Logger.lastError
                    lastInfo: Logger.lastInfo
                }

                StackLayout {
                    id: stack
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    currentIndex: win._stackIndexFromNav(win.currentNavIndex)

                    PageDashboard   {}
                    PagePerson      {}
                    PageDevice      {}
                    PageAttendance  {}
                    PageFace        {}
                    PageHistory     {}
                    PageSettings    {}
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
