import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceAdmin

Item {
    id: page

    required property var attendanceService
    required property var sessionManager
    property var deniedDialog: null
    signal serviceResult(string apiType, int code, string message)

    property int tabIndex: 0

    readonly property bool hasLiveRead: PermissionCatalog.hasPerm(sessionManager, "attendance.read")
    readonly property bool hasArchiveRead: PermissionCatalog.hasPerm(sessionManager, "attendance.archive.read")

    readonly property var archiveStatusChartData: {
        if (!page.hasArchiveRead || !attendanceService)
            return []
        return page.buildArchiveStatusChart()
    }

    function _liveQuery() {
        attendanceService.query(-1, empId.text.trim(), checkTime.text.trim(),
                                devId.text.trim(), status.currentValue, "")
    }

    function buildArchiveStatusChart() {
        if (!attendanceService || !attendanceService.archiveRecords)
            return []

        const records = attendanceService.archiveRecords
        const statusCounts = {}

        for (let i = 0; i < records.length; ++i) {
            const status = records[i].status || "unknown"
            if (!statusCounts[status]) {
                statusCounts[status] = 0
            }
            statusCounts[status]++
        }

        const result = []
        const statusOrder = ["normal", "late", "early", "absent", "manual"]

        for (let i = 0; i < statusOrder.length; i++) {
            const status = statusOrder[i]
            if (statusCounts[status] && statusCounts[status] > 0) {
                result.push({
                    label: Theme.formatAttendanceStatus(status),
                    value: statusCounts[status],
                    color: Theme.attendancePieColor(status)
                })
            }
        }

        return result
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingLg
        spacing: Theme.spacingMd

        ToolBarRow {
            Layout.fillWidth: true
            title: qsTr("考勤记录")
            subtitle: qsTr("实时考勤与归档查询")
        }

        TabBar {
            id: tabs
            Layout.fillWidth: true
            currentIndex: page.tabIndex
            onCurrentIndexChanged: page.tabIndex = currentIndex
            TabButton { text: qsTr("实时考勤"); visible: page.hasLiveRead }
            TabButton {
                text: qsTr("归档记录")
                visible: page.hasArchiveRead
            }
        }

        StackLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: page.tabIndex

            // Tab A — 实时考勤
            ColumnLayout {
                visible: page.hasLiveRead
                spacing: Theme.spacingMd

                Card {
                    Layout.fillWidth: true
                    title: qsTr("筛选 & 新增")

                    ColumnLayout {
                        width: parent.width
                        spacing: Theme.spacingSm

                        GridLayout {
                            Layout.fillWidth: true
                            columns: 4
                            rowSpacing: Theme.spacingSm
                            columnSpacing: Theme.spacingMd

                            LabeledField { label: qsTr("员工工号"); Layout.fillWidth: true
                                TextField { id: empId; text: Presets.defaultEmployeeId; Layout.fillWidth: true }
                            }
                            LabeledField { label: qsTr("打卡时间"); Layout.fillWidth: true
                                TextField {
                                    id: checkTime
                                    text: Qt.formatDateTime(new Date(), "yyyy-MM-dd HH:mm:ss")
                                    Layout.fillWidth: true
                                }
                            }
                            LabeledField { label: qsTr("设备 ID"); Layout.fillWidth: true
                                TextField { id: devId; text: Presets.defaultDeviceId; Layout.fillWidth: true }
                            }
                            LabeledField { label: qsTr("状态"); Layout.fillWidth: true
                                ComboBox {
                                    id: status
                                    Layout.fillWidth: true
                                    model: [
                                        { text: qsTr("正常"), value: "normal" },
                                        { text: qsTr("迟到"), value: "late" },
                                        { text: qsTr("早退"), value: "early" },
                                        { text: qsTr("缺勤"), value: "absent" },
                                        { text: qsTr("补签"), value: "manual" }
                                    ]
                                    textRole: "text"
                                    valueRole: "value"
                                }
                            }
                        }

                        Row {
                            spacing: Theme.spacingSm
                            PermissionButton {
                                sessionManager: page.sessionManager
                                requiredPermission: "attendance.create"
                                deniedDialog: page.deniedDialog
                                text: qsTr("新增打卡记录")
                                highlighted: true
                                enabled: !attendanceService.busy
                                onClicked: guardedClick(function() {
                                    attendanceService.createRecord(
                                        empId.text.trim(), checkTime.text.trim(),
                                        status.currentValue, devId.text.trim())
                                })
                            }
                            PermissionButton {
                                sessionManager: page.sessionManager
                                requiredPermission: "attendance.read"
                                deniedDialog: page.deniedDialog
                                text: qsTr("查询")
                                enabled: !attendanceService.busy
                                onClicked: guardedClick(page._liveQuery)
                            }
                            PermissionButton {
                                sessionManager: page.sessionManager
                                requiredPermission: "attendance.create"
                                deniedDialog: page.deniedDialog
                                text: qsTr("使用当前时间")
                                flat: true
                                onClicked: guardedClick(function() {
                                    checkTime.text = Qt.formatDateTime(new Date(), "yyyy-MM-dd HH:mm:ss")
                                })
                            }
                        }
                    }
                }

                Card {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    stretchContent: true
                    title: qsTr("实时记录")

                    DataTable {
                        anchors.fill: parent
                        rows: attendanceService.records
                        columns: [
                            { key: "id", title: "ID", width: 60, align: "right" },
                            { key: "employeeId", title: qsTr("工号"), width: 100 },
                            { key: "checkTime", title: qsTr("打卡时间"), width: 170 },
                            { key: "deviceId", title: qsTr("设备 ID"), width: 130 },
                            {
                                key: "status",
                                title: qsTr("状态"),
                                width: 80,
                                formatter: function(v) { return Theme.formatAttendanceStatus(v) }
                            },
                            { key: "receivedTime", title: qsTr("接收时间") }
                        ]
                    }
                }
            }

            // Tab B — 归档
            ColumnLayout {
                visible: page.hasArchiveRead
                spacing: Theme.spacingMd

                Card {
                    Layout.fillWidth: true
                    title: qsTr("归档筛选")

                    ColumnLayout {
                        width: parent.width
                        spacing: Theme.spacingSm

                        GridLayout {
                            Layout.fillWidth: true
                            columns: 3
                            rowSpacing: Theme.spacingSm
                            columnSpacing: Theme.spacingMd

                            LabeledField { label: qsTr("工号"); Layout.fillWidth: true
                                TextField { id: archEmp; Layout.fillWidth: true }
                            }
                            LabeledField { label: qsTr("姓名"); Layout.fillWidth: true
                                TextField { id: archName; Layout.fillWidth: true }
                            }
                            LabeledField { label: qsTr("部门"); Layout.fillWidth: true
                                TextField { id: archDept; Layout.fillWidth: true }
                            }
                        }

                        Row {
                            spacing: Theme.spacingSm
                            PermissionButton {
                                sessionManager: page.sessionManager
                                requiredPermission: "attendance.archive.read"
                                deniedDialog: page.deniedDialog
                                text: qsTr("查询归档")
                                enabled: !attendanceService.busy
                                onClicked: guardedClick(function() {
                                    attendanceService.queryArchive(
                                        -1, archEmp.text.trim(), archName.text.trim(),
                                        archDept.text.trim(), "", "", "", "", "", "", "")
                                })
                            }
                        }
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    spacing: Theme.spacingMd

                    Card {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        stretchContent: true
                        title: qsTr("归档列表")

                        DataTable {
                            anchors.fill: parent
                            rows: attendanceService.archiveRecords
                            columns: [
                                { key: "id", title: "ID", width: 60, align: "right" },
                                { key: "employeeId", title: qsTr("工号"), width: 100 },
                                { key: "personName", title: qsTr("姓名"), width: 100 },
                                { key: "department", title: qsTr("部门"), width: 100 },
                                { key: "checkTime", title: qsTr("打卡时间"), width: 160 },
                                { key: "archivedAt", title: qsTr("归档时间"), width: 160 },
                                { key: "archiveReason", title: qsTr("原因") }
                            ]
                        }
                    }

                    Card {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.preferredWidth: 300
                        title: qsTr("归档状态分布")

                        Item {
                            anchors.fill: parent

                            Label {
                                anchors.centerIn: parent
                                visible: page.archiveStatusChartData.length === 0
                                text: qsTr("暂无归档数据")
                                color: Theme.textMuted
                                font.pixelSize: Theme.fontMd
                                font.family: Theme.fontFamily
                            }

                            AttendanceStatusPieChart {
                                id: archivePieChart
                                anchors.fill: parent
                                anchors.margins: Theme.spacingMd
                                visible: page.archiveStatusChartData.length > 0
                                slices: page.archiveStatusChartData
                            }
                        }
                    }
                }
            }
        }
    }

    BusyOverlay { busy: attendanceService.busy }

    Connections {
        target: attendanceService
        function onOperationSucceeded(apiType, message) {
            page.serviceResult(apiType, 0, message)
        }
        function onOperationFailed(apiType, code, message) {
            page.serviceResult(apiType, code, message)
        }
    }
}
