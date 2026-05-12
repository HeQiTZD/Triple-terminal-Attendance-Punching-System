import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceAdmin

Item {
    id: page

    property var rows: []

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

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingLg
        spacing: Theme.spacingMd

        ToolBarRow {
            Layout.fillWidth: true
            title: qsTr("考勤记录")
            subtitle: qsTr("查询 / 新增 / 修改 / 删除考勤记录（需连接服务端）")
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
                        TextField { id: empId; placeholderText: qsTr("EMP001"); Layout.fillWidth: true }
                    }
                    LabeledField { label: qsTr("打卡时间"); Layout.fillWidth: true
                        TextField {
                            id: checkTime
                            text: Qt.formatDateTime(new Date(), "yyyy-MM-dd HH:mm:ss")
                            Layout.fillWidth: true
                        }
                    }
                    LabeledField { label: qsTr("设备 ID"); Layout.fillWidth: true
                        TextField { id: devId; placeholderText: qsTr("DEV001"); Layout.fillWidth: true }
                    }
                    LabeledField { label: qsTr("状态"); Layout.fillWidth: true
                        ComboBox { id: status; model: ["normal", "late", "early", "absent", "manual"]; Layout.fillWidth: true }
                    }
                }

                Row {
                    spacing: Theme.spacingSm

                    Button {
                        text: qsTr("新增打卡记录")
                        highlighted: true
                        onClicked: Logger.info("TODO: attendance.create via TCP")
                    }
                    Button {
                        text: qsTr("查询")
                        onClicked: Logger.info("TODO: attendance.query via TCP")
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
                    { key: "checkTime", title: qsTr("打卡时间"), width: 170 },
                    { key: "deviceId", title: qsTr("设备 ID"), width: 130 },
                    { key: "status", title: qsTr("状态"), width: 80 },
                    { key: "receivedTime", title: qsTr("接收时间") }
                ]
            }
        }
    }
}
