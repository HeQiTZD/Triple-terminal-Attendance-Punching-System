import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceAdmin

Item {
    id: page

    required property var sessionManager
    property var personServer
    property var deviceServer
    property var attendanceService
    property var eventService

    readonly property int connState: sessionManager ? sessionManager.connectionState : 0

    readonly property bool canReadPerson: PermissionCatalog.hasPerm(sessionManager, "person.read")
    readonly property bool canReadDevice: PermissionCatalog.hasPerm(sessionManager, "device.read")
    readonly property bool canReadAttendance: PermissionCatalog.hasPerm(sessionManager, "attendance.read")

    readonly property int employeeCount: {
        if (!canReadPerson || !personServer)
            return 0
        return personServer.records.length
    }

    readonly property int onlineDeviceCount: {
        if (!canReadDevice || !deviceServer)
            return 0
        let n = 0
        const rec = deviceServer.records
        for (let i = 0; i < rec.length; ++i) {
            if (rec[i].status === "online")
                n++
        }
        return n
    }

    readonly property int todayPunchCount: countTodayUniquePunches(
        attendanceService ? attendanceService.records : [])

    readonly property bool canViewHistory: {
        if (sessionManager) {
            const _p = sessionManager.permissions
            void _p
        }
        return PermissionCatalog.canAccessNav("history", sessionManager)
    }

    readonly property bool canViewEvents: {
        if (sessionManager) {
            const _p = sessionManager.permissions
            void _p
        }
        return PermissionCatalog.canAccessNav("events", sessionManager)
    }

    function countTodayUniquePunches(records) {
        const prefix = Qt.formatDateTime(new Date(), "yyyy-MM-dd")
        const seen = {}
        let n = 0
        for (let i = 0; i < records.length; ++i) {
            const t = records[i].checkTime || ""
            if (!t.startsWith(prefix))
                continue
            const eid = records[i].employeeId || ""
            if (!eid || seen[eid])
                continue
            seen[eid] = true
            n++
        }
        return n
    }

    function refreshStats() {
        if (!sessionManager || !sessionManager.isLoggedIn || page.connState !== 3)
            return
        if (page.canReadPerson && personServer)
            personServer.queryPersons("", "", "", "", "", "")
        if (page.canReadDevice && deviceServer)
            deviceServer.queryDevices("", "", "")
        if (page.canReadAttendance && attendanceService)
            attendanceService.query(-1, "", "", "", "", "")
    }

    function statDisplayText(hasPermission) {
        if (!sessionManager || !sessionManager.isLoggedIn)
            return "—"
        if (!hasPermission)
            return qsTr("无权限")
        return ""
    }

    Component.onCompleted: page.refreshStats()

    Connections {
        target: sessionManager
        function onLoggedInChanged() {
            if (sessionManager && sessionManager.isLoggedIn)
                page.refreshStats()
        }
        function onConnectionStateChanged() {
            if (sessionManager && sessionManager.connectionState === 3)
                page.refreshStats()
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingLg
        spacing: Theme.spacingLg

        ToolBarRow {
            Layout.fillWidth: true
            title: PermissionCatalog.welcomeTitle(sessionManager)
        }

        GridLayout {
            Layout.fillWidth: true
            columns: 3
            rowSpacing: Theme.spacingMd
            columnSpacing: Theme.spacingMd

            Card {
                Layout.fillWidth: true
                Layout.preferredHeight: 110
                title: qsTr("员工人数")

                Label {
                    text: {
                        const placeholder = page.statDisplayText(page.canReadPerson)
                        if (placeholder.length)
                            return placeholder
                        return String(page.employeeCount)
                    }
                    color: Theme.text
                    font.pixelSize: Theme.fontXl
                    font.bold: true
                }
            }

            Card {
                Layout.fillWidth: true
                Layout.preferredHeight: 110
                title: qsTr("今日打卡人数")

                Label {
                    text: {
                        const placeholder = page.statDisplayText(page.canReadAttendance)
                        if (placeholder.length)
                            return placeholder
                        return String(page.todayPunchCount)
                    }
                    color: Theme.text
                    font.pixelSize: Theme.fontXl
                    font.bold: true
                }
            }

            Card {
                Layout.fillWidth: true
                Layout.preferredHeight: 110
                title: qsTr("设备连接数量")

                Label {
                    text: {
                        const placeholder = page.statDisplayText(page.canReadDevice)
                        if (placeholder.length)
                            return placeholder
                        return String(page.onlineDeviceCount)
                    }
                    color: Theme.text
                    font.pixelSize: Theme.fontXl
                    font.bold: true
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: Theme.spacingMd
            visible: page.canViewHistory || page.canViewEvents

            Card {
                Layout.fillWidth: true
                Layout.fillHeight: true
                stretchContent: true
                title: qsTr("最近调用")
                visible: page.canViewHistory

                ListView {
                    anchors.fill: parent
                    clip: true
                    model: History.model
                    orientation: ListView.Vertical
                    delegate: Item {
                        required property int index
                        width: ListView.view.width
                        height: rowLbl.implicitHeight + 8
                        readonly property var entry: History.get(index)
                        Label {
                            id: rowLbl
                            width: parent.width - Theme.spacingMd * 2
                            x: Theme.spacingMd
                            anchors.verticalCenter: parent.verticalCenter
                            text: entry ? ("[" + entry.time + "] " + entry.direction + " " + entry.target) : ""
                            color: entry && entry.ok ? Theme.textMuted : Theme.danger
                            font.pixelSize: Theme.fontXs
                            font.family: Theme.fontMono
                            elide: Text.ElideRight
                        }
                    }
                }
            }

            Card {
                Layout.fillWidth: true
                Layout.fillHeight: true
                stretchContent: true
                title: qsTr("最近推送")
                visible: page.canViewEvents

                ListView {
                    anchors.fill: parent
                    clip: true
                    model: PushFeed.model
                    delegate: Item {
                        required property int index
                        width: ListView.view.width
                        height: lbl.implicitHeight + 8
                        readonly property var entry: PushFeed.model.get(index)
                        Label {
                            id: lbl
                            width: parent.width - Theme.spacingMd * 2
                            x: Theme.spacingMd
                            anchors.verticalCenter: parent.verticalCenter
                            text: entry ? ("[" + entry.time + "] " + entry.type) : ""
                            color: Theme.textMuted
                            font.pixelSize: Theme.fontXs
                            font.family: Theme.fontMono
                            elide: Text.ElideRight
                        }
                    }
                }
            }
        }
    }
}
