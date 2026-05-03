import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceServer

Item {
    id: page

    property var rows: []

    function _parseDateTime(text) {
        const d = Date.fromLocaleString(Qt.locale(), text, "yyyy-MM-dd HH:mm:ss")
        return isNaN(d.getTime()) ? new Date() : d
    }

    function _record(target, args, ok, detail) {
        History.record({
            direction: "INVOKE",
            target: target,
            payload: args,
            ok: ok,
            result: ok ? ("OK | " + (detail || "")) : ("FAIL: " + (detail || "")),
            category: "attendance"
        })
    }

    Connections {
        target: dataManager
        function onAttendanceRecordAdded() { /* 不自动刷新，用户手动查询 */ }
    }

    Connections {
        target: tcpServer
        function onAttendanceRecordReceived(record) {
            Logger.info("收到考勤上报: " + JSON.stringify(record))
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingLg
        spacing: Theme.spacingMd

        ToolBarRow {
            Layout.fillWidth: true
            title: qsTr("考勤记录")
            subtitle: qsTr("手工补卡 · 多条件查询 · 实时接收 TCP 上报")
        }

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
                        ComboBox { id: status; model: ["normal", "late", "early", "manual"]; Layout.fillWidth: true }
                    }

                    LabeledField { label: qsTr("起始时间"); Layout.fillWidth: true
                        TextField { id: startTime; text: Presets.defaultStartTime; Layout.fillWidth: true }
                    }
                    LabeledField { label: qsTr("结束时间"); Layout.fillWidth: true
                        TextField { id: endTime; text: Presets.defaultEndTime; Layout.fillWidth: true }
                    }
                    LabeledField { label: qsTr("姓名(模糊)"); Layout.fillWidth: true; Layout.columnSpan: 2
                        TextField { id: personNameFilter; placeholderText: qsTr("可选"); Layout.fillWidth: true }
                    }
                }

                Row {
                    spacing: Theme.spacingSm

                    Button {
                        text: qsTr("新增打卡记录")
                        highlighted: true
                        onClicked: {
                            const ok = dataService.addAttendanceRecord(
                                empId.text.trim(),
                                page._parseDateTime(checkTime.text),
                                devId.text,
                                status.currentText)
                            Logger.logResult("新增考勤记录", ok, "工号=" + empId.text)
                            page._record("dataService.addAttendanceRecord",
                                { employeeId: empId.text, checkTime: checkTime.text, deviceId: devId.text, status: status.currentText }, ok)
                        }
                    }
                    Button {
                        text: qsTr("按时间查询")
                        onClicked: {
                            const list = dataService.selectAttendanceRecord("",
                                page._parseDateTime(startTime.text),
                                page._parseDateTime(endTime.text),
                                "", "", "")
                            page.rows = list
                            Logger.logResult("按时间查询考勤", true, "数量=" + list.length)
                            page._record("dataService.selectAttendanceRecord",
                                { startTime: startTime.text, endTime: endTime.text }, true, "count=" + list.length)
                        }
                    }
                    Button {
                        text: qsTr("按人员查询")
                        onClicked: {
                            const list = dataService.selectAttendanceRecord(empId.text.trim(),
                                page._parseDateTime(startTime.text),
                                page._parseDateTime(endTime.text),
                                "", "", "")
                            page.rows = list
                            Logger.logResult("按人员查询考勤", true, "数量=" + list.length)
                            page._record("dataService.selectAttendanceRecord",
                                { employeeId: empId.text, startTime: startTime.text, endTime: endTime.text }, true, "count=" + list.length)
                        }
                    }
                    Button {
                        text: qsTr("组合查询")
                        onClicked: {
                            const list = dataService.selectAttendanceRecord(empId.text.trim(),
                                page._parseDateTime(startTime.text),
                                page._parseDateTime(endTime.text),
                                devId.text.trim(),
                                status.currentText,
                                personNameFilter.text.trim())
                            page.rows = list
                            Logger.logResult("组合查询考勤", true, "数量=" + list.length)
                            page._record("dataService.selectAttendanceRecord",
                                { employeeId: empId.text, startTime: startTime.text, endTime: endTime.text,
                                  deviceId: devId.text, status: status.currentText, personName: personNameFilter.text }, true, "count=" + list.length)
                        }
                    }
                    Button {
                        text: qsTr("使用当前时间")
                        flat: true
                        onClicked: checkTime.text = Qt.formatDateTime(new Date(), "yyyy-MM-dd HH:mm:ss")
                    }
                }
            }
        }

        Card {
            Layout.fillWidth: true
            Layout.fillHeight: true
            stretchContent: true
            title: qsTr("查询结果")

            DataTable {
                id: table
                anchors.fill: parent
                rows: page.rows
                columns: [
                    { key: "id", title: "ID", width: 60, align: "right" },
                    { key: "employeeId", title: qsTr("工号"), width: 100 },
                    { key: "personName", title: qsTr("姓名"), width: 100 },
                    { key: "checkTime", title: qsTr("打卡时间"), width: 170,
                      formatter: function(v) { return v ? Qt.formatDateTime(v, "yyyy-MM-dd HH:mm:ss") : "" } },
                    { key: "deviceId", title: qsTr("设备 ID"), width: 130 },
                    { key: "status", title: qsTr("状态"), width: 80 },
                    { key: "receivedTime", title: qsTr("接收时间"),
                      formatter: function(v) { return v && !isNaN(v.getTime()) ? Qt.formatDateTime(v, "yyyy-MM-dd HH:mm:ss") : "—" } }
                ]
            }
        }
    }
}
