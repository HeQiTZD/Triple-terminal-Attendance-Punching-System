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
            result: ok ? "OK" : ("FAIL: " + (detail || "")),
            category: "face"
        })
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingLg
        spacing: Theme.spacingMd

        ToolBarRow {
            Layout.fillWidth: true
            title: qsTr("人脸数据管理")
            subtitle: qsTr("查询 / 注册 / 删除人脸特征（需连接服务端）")
            actions: [
                Button { text: qsTr("刷新"); onClicked: Logger.info("TODO: face.query via TCP") }
            ]
        }

        Card {
            Layout.fillWidth: true
            title: qsTr("人脸操作")

            ColumnLayout {
                width: parent.width
                spacing: Theme.spacingSm

                RowLayout {
                    Layout.fillWidth: true

                    LabeledField {
                        label: qsTr("工号"); Layout.fillWidth: true
                        TextField {
                            id: faceEmp
                            placeholderText: qsTr("EMP001")
                            Layout.fillWidth: true
                        }
                    }
                    Button {
                        text: qsTr("查询")
                        onClicked: Logger.info("TODO: face.query via TCP")
                    }
                    Button {
                        text: qsTr("注册人脸")
                        highlighted: true
                        onClicked: Logger.info("TODO: face.register.header via TCP")
                    }
                    Button {
                        text: qsTr("删除")
                        onClicked: confirm.open()
                    }
                }

                DataTable {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    rows: page.rows
                    columns: [
                        { key: "employeeId", title: qsTr("工号"), width: 120 },
                        { key: "featureSize", title: qsTr("特征长度"), width: 100, align: "right" },
                        { key: "createdAt", title: qsTr("创建时间"), width: 170 },
                        { key: "updatedAt", title: qsTr("更新时间") }
                    ]
                    onRowClicked: function(idx, row) {
                        faceEmp.text = row.employeeId || ""
                    }
                }
            }
        }
    }

    ConfirmDialog {
        id: confirm
        message: qsTr("确认删除工号 ") + faceEmp.text + qsTr(" 的人脸特征？")
        onAccepted: Logger.info("TODO: face.delete via TCP")
    }
}
