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

    readonly property int connState: sessionManager ? sessionManager.connectionState : 0

    readonly property bool canReadPerson: PermissionCatalog.hasPerm(sessionManager, "person.read")
    readonly property bool canReadDevice: PermissionCatalog.hasPerm(sessionManager, "device.read")
    readonly property bool canReadAttendance: PermissionCatalog.hasPerm(sessionManager, "attendance.read")

    readonly property string todayDatePrefix: Qt.formatDateTime(new Date(), "yyyy-MM-dd")

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

    readonly property var statusPieSlices: {
        if (!page.canReadAttendance || !attendanceService)
            return []
        const _records = attendanceService.records
        void _records
        if (page.canReadPerson && personServer) {
            const _persons = personServer.records
            void _persons
        }
        return page.buildStatusPieSlices()
    }

    readonly property bool hasTodayAttendanceData: page.statusPieSlices.length > 0

    readonly property var recentPunchRecords: {
        if (!page.canReadAttendance || !attendanceService)
            return []
        const _records = attendanceService.records
        void _records
        if (page.canReadPerson && personServer) {
            const _persons = personServer.records
            void _persons
        }
        return page.buildRecentPunchRecords()
    }

    function todayRecords(records) {
        const prefix = page.todayDatePrefix
        const out = []
        for (let i = 0; i < records.length; ++i) {
            const r = records[i]
            if ((r.checkTime || "").startsWith(prefix))
                out.push(r)
        }
        return out
    }

    function personNameByEmployeeId(empId) {
        if (!empId || !page.canReadPerson || !personServer)
            return "—"
        const rec = personServer.records
        for (let i = 0; i < rec.length; ++i) {
            if (rec[i].employeeId === empId)
                return rec[i].name || "—"
        }
        return "—"
    }

    function pieChartStatusKey(status) {
        const s = String(status || "unknown").toLowerCase()
        if (s === "manual")
            return "normal"
        return s
    }

    function buildStatusPieSlices() {
        if (!page.canReadAttendance || !attendanceService)
            return []
        const today = page.todayRecords(attendanceService.records)
        const latestByEmp = {}
        for (let i = 0; i < today.length; ++i) {
            const r = today[i]
            const eid = r.employeeId || ""
            if (!eid)
                continue
            if (!latestByEmp[eid] || (r.checkTime || "") > (latestByEmp[eid].checkTime || ""))
                latestByEmp[eid] = r
        }
        const counts = {}
        const empIds = Object.keys(latestByEmp)
        for (let i = 0; i < empIds.length; ++i) {
            const key = page.pieChartStatusKey(latestByEmp[empIds[i]].status)
            counts[key] = (counts[key] || 0) + 1
        }
        const order = ["normal", "late", "early", "absent", "unknown"]
        const slices = []
        for (let i = 0; i < order.length; ++i) {
            const status = order[i]
            const n = counts[status]
            if (!n)
                continue
            slices.push({
                status: status,
                label: Theme.formatAttendanceStatus(status),
                value: n,
                color: Theme.attendancePieColor(status)
            })
        }
        const statusKeys = Object.keys(counts)
        for (let i = 0; i < statusKeys.length; ++i) {
            const status = statusKeys[i]
            if (order.indexOf(status) >= 0)
                continue
            slices.push({
                status: status,
                label: Theme.formatAttendanceStatus(status),
                value: counts[status],
                color: Theme.attendancePieColor(status)
            })
        }
        return slices
    }

    function countTodayUniquePunches(records) {
        const prefix = page.todayDatePrefix
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

    function buildRecentPunchRecords() {
        if (!page.canReadAttendance || !attendanceService)
            return []
        const rec = attendanceService.records.slice()
        rec.sort(function(a, b) {
            return (b.checkTime || "").localeCompare(a.checkTime || "")
        })
        const limited = rec.slice(0, 100)
        const rows = []
        for (let i = 0; i < limited.length; ++i) {
            const r = limited[i]
            const checkTime = r.checkTime || ""
            const dateGroup = checkTime.length >= 10 ? checkTime.substring(0, 10) : ""
            rows.push({
                employeeId: r.employeeId || "",
                personName: page.personNameByEmployeeId(r.employeeId || ""),
                checkTime: checkTime,
                dateGroup: dateGroup,
                status: r.status || "",
                statusLabel: Theme.formatAttendanceStatus(r.status),
                deviceId: r.deviceId || ""
            })
        }
        return rows
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

    function attendanceSectionPlaceholder() {
        return page.statDisplayText(page.canReadAttendance)
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
            id: trendRow
            Layout.fillWidth: true
            Layout.preferredHeight: 280
            spacing: Theme.spacingMd
            visible: page.canReadAttendance

            Card {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredWidth: trendRow.width * 0.55
                stretchContent: true
                title: qsTr("最近打卡记录")

                RecentPunchList {
                    anchors.fill: parent
                    anchors.margins: Theme.spacingMd
                    records: page.recentPunchRecords
                }
            }

            Card {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredWidth: trendRow.width * 0.45
                stretchContent: true
                title: qsTr("今日打卡类型")

                Item {
                    anchors.fill: parent

                    Label {
                        anchors.centerIn: parent
                        visible: page.attendanceSectionPlaceholder().length > 0
                        text: page.attendanceSectionPlaceholder()
                        color: Theme.textMuted
                        font.pixelSize: Theme.fontMd
                        font.family: Theme.fontFamily
                    }

                    Label {
                        anchors.centerIn: parent
                        visible: page.canReadAttendance && !page.hasTodayAttendanceData
                        text: qsTr("今日暂无打卡数据")
                        color: Theme.textMuted
                        font.pixelSize: Theme.fontMd
                        font.family: Theme.fontFamily
                    }

                    AttendanceStatusPieChart {
                        anchors.fill: parent
                        visible: page.canReadAttendance && page.hasTodayAttendanceData
                        slices: page.statusPieSlices
                    }
                }
            }
        }
    }
}
