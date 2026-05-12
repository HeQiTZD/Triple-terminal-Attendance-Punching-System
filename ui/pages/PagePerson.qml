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
            category: "person"
        })
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingLg
        spacing: Theme.spacingMd

        ToolBarRow {
            Layout.fillWidth: true
            title: qsTr("人员管理")
            subtitle: qsTr("新增 / 修改 / 删除 / 查询人员（需连接服务端）")
        }

        Card {
            Layout.fillWidth: true
            title: qsTr("人员信息")

            GridLayout {
                width: parent.width
                columns: 4
                rowSpacing: Theme.spacingSm
                columnSpacing: Theme.spacingMd

                LabeledField { label: qsTr("姓名"); Layout.fillWidth: true
                    TextField { id: pName; placeholderText: qsTr("张三"); Layout.fillWidth: true }
                }
                LabeledField { label: qsTr("工号"); Layout.fillWidth: true
                    TextField { id: pEmp; placeholderText: qsTr("EMP001"); Layout.fillWidth: true }
                }
                LabeledField { label: qsTr("部门"); Layout.fillWidth: true
                    TextField { id: pDept; placeholderText: qsTr("研发部"); Layout.fillWidth: true }
                }
                LabeledField { label: qsTr("岗位"); Layout.fillWidth: true
                    TextField { id: pPos; placeholderText: qsTr("工程师"); Layout.fillWidth: true }
                }

                Row {
                    Layout.columnSpan: 4
                    spacing: Theme.spacingSm
                    Button {
                        text: qsTr("新增")
                        highlighted: true
                        onClicked: Logger.info("TODO: person.create via TCP")
                    }
                    Button {
                        text: qsTr("修改")
                        onClicked: Logger.info("TODO: person.update via TCP")
                    }
                    Button {
                        text: qsTr("删除")
                        onClicked: confirm.open()
                    }
                    Button {
                        text: qsTr("查询")
                        onClicked: Logger.info("TODO: person.query via TCP")
                    }
                    Button {
                        text: qsTr("清空表单")
                        flat: true
                        onClicked: { pName.text=""; pEmp.text=""; pDept.text=""; pPos.text="" }
                    }
                }
            }
        }

        Card {
            Layout.fillWidth: true
            Layout.fillHeight: true
            stretchContent: true
            title: qsTr("人员列表")
            subtitle: qsTr("点击行加载到表单")

            DataTable {
                id: table
                anchors.fill: parent
                rows: page.rows
                columns: [
                    { key: "id", title: "ID", width: 60, align: "right" },
                    { key: "employeeId", title: qsTr("工号"), width: 120 },
                    { key: "name", title: qsTr("姓名"), width: 120 },
                    { key: "department", title: qsTr("部门"), width: 140 },
                    { key: "position", title: qsTr("岗位"), width: 140 },
                    { key: "createdAt", title: qsTr("创建时间"), width: 170 },
                    { key: "updatedAt", title: qsTr("更新时间") }
                ]
                onRowClicked: function(idx, row) {
                    pName.text = row.name || ""
                    pEmp.text = row.employeeId || ""
                    pDept.text = row.department || ""
                    pPos.text = row.position || ""
                }
            }
        }
    }

    ConfirmDialog {
        id: confirm
        message: qsTr("确认删除工号 ") + pEmp.text + qsTr(" 的人员？")
        onAccepted: Logger.info("TODO: person.delete via TCP")
    }
}
