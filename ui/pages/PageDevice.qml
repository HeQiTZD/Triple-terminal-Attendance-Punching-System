import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceAdmin

Item {
    id: page

    property var rows: []

    function _record(target, args, ok) {
        History.record({
            direction: "INVOKE",
            target: target,
            payload: args,
            ok: ok,
            result: ok ? "OK" : "FAIL",
            category: "device"
        })
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingLg
        spacing: Theme.spacingMd

        ToolBarRow {
            Layout.fillWidth: true
            title: qsTr("设备管理")
            subtitle: qsTr("注册 / 修改设备 · 状态查看 · 多条件筛选（需连接服务端）")
            actions: [
                Button { text: qsTr("刷新"); onClicked: Logger.info("TODO: device.query via TCP") }
            ]
        }

        Card {
            Layout.fillWidth: true
            title: qsTr("设备信息")

            ColumnLayout {
                width: parent.width
                spacing: Theme.spacingSm

                GridLayout {
                    Layout.fillWidth: true
                    columns: 4
                    rowSpacing: Theme.spacingSm
                    columnSpacing: Theme.spacingMd

                    LabeledField { label: qsTr("设备 ID"); Layout.fillWidth: true
                        TextField { id: dId; placeholderText: qsTr("DEV001"); Layout.fillWidth: true }
                    }
                    LabeledField { label: qsTr("名称"); Layout.fillWidth: true
                        TextField { id: dName; placeholderText: qsTr("一号考勤机"); Layout.fillWidth: true }
                    }
                    LabeledField { label: qsTr("IP"); Layout.fillWidth: true
                        TextField { id: dIp; placeholderText: qsTr("192.168.1.100"); Layout.fillWidth: true }
                    }
                    LabeledField { label: qsTr("状态"); Layout.fillWidth: true
                        ComboBox {
                            id: dStatus
                            model: ["online", "offline", "maintenance"]
                            Layout.fillWidth: true
                        }
                    }
                }

                Row {
                    spacing: Theme.spacingSm
                    Button {
                        text: qsTr("新增设备")
                        highlighted: true
                        onClicked: Logger.info("TODO: device.create via TCP")
                    }
                    Button {
                        text: qsTr("修改设备")
                        onClicked: Logger.info("TODO: device.update via TCP")
                    }
                    Button {
                        text: qsTr("查询")
                        onClicked: Logger.info("TODO: device.query via TCP")
                    }
                    Button {
                        text: qsTr("删除")
                        onClicked: Logger.info("TODO: device.delete via TCP")
                    }
                }
            }
        }

        Card {
            Layout.fillWidth: true
            Layout.fillHeight: true
            stretchContent: true
            title: qsTr("设备列表")

            DataTable {
                id: table
                anchors.fill: parent
                rows: page.rows
                columns: [
                    { key: "id", title: "ID", width: 60, align: "right" },
                    { key: "deviceId", title: qsTr("设备 ID"), width: 130 },
                    { key: "deviceName", title: qsTr("名称"), width: 160 },
                    { key: "ipAddress", title: qsTr("IP"), width: 130 },
                    { key: "status", title: qsTr("状态"), width: 100 },
                    { key: "lastOnline", title: qsTr("最近在线") }
                ]
                onRowClicked: function(idx, row) {
                    dId.text = row.deviceId || ""
                    dName.text = row.deviceName || ""
                    dIp.text = row.ipAddress || ""
                }
            }
        }
    }
}
