import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceServer

Item {
    id: page

    property var rows: []

    function refresh() {
        rows = dataService.getAllPerson()
        Logger.logResult("加载人员列表", true, "数量=" + rows.length)
    }

    Component.onCompleted: refresh()

    Connections {
        target: dataService
        function onConnectionStateChanged() { if (dataService.isConnected) page.refresh() }
    }
    Connections {
        target: dataManager
        function onPersonAdded() { page.refresh() }
        function onPersonDeleted() { page.refresh() }
    }

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
            subtitle: qsTr("新增 / 修改 / 删除 / 查询人员")
            actions: [
                Button { text: qsTr("刷新"); onClicked: page.refresh() }
            ]
        }

        // ===== 表单 =====
        Card {
            Layout.fillWidth: true
            title: qsTr("人员信息")

            GridLayout {
                anchors.fill: parent
                columns: 4
                rowSpacing: Theme.spacingSm
                columnSpacing: Theme.spacingMd

                LabeledField { label: qsTr("ID（查询）"); Layout.fillWidth: true
                    TextField { id: pId; placeholderText: qsTr("数字"); Layout.fillWidth: true }
                }
                LabeledField { label: qsTr("姓名"); Layout.fillWidth: true
                    TextField { id: pName; placeholderText: qsTr("张三"); Layout.fillWidth: true }
                }
                LabeledField { label: qsTr("工号"); Layout.fillWidth: true
                    TextField { id: pEmp; placeholderText: qsTr("EMP001"); Layout.fillWidth: true }
                }
                LabeledField { label: qsTr("部门"); Layout.fillWidth: true
                    TextField { id: pDept; placeholderText: qsTr("研发部"); Layout.fillWidth: true }
                }
                LabeledField { label: qsTr("岗位"); Layout.fillWidth: true; Layout.columnSpan: 4
                    TextField { id: pPos; placeholderText: qsTr("工程师"); Layout.fillWidth: true }
                }

                Row {
                    Layout.columnSpan: 4
                    spacing: Theme.spacingSm
                    Button {
                        text: qsTr("新增")
                        highlighted: true
                        onClicked: {
                            const ok = dataService.addPerson(pName.text, pEmp.text, pDept.text, pPos.text)
                            Logger.logResult("新增人员", ok, "工号=" + pEmp.text)
                            page._record("dataService.addPerson",
                                { name: pName.text, employeeId: pEmp.text, department: pDept.text, position: pPos.text }, ok)
                            if (ok) page.refresh()
                        }
                    }
                    Button {
                        text: qsTr("修改")
                        onClicked: {
                            const ok = dataService.updatedPerson(pName.text, pEmp.text, pDept.text, pPos.text)
                            Logger.logResult("修改人员", ok, "工号=" + pEmp.text)
                            page._record("dataService.updatedPerson",
                                { name: pName.text, employeeId: pEmp.text, department: pDept.text, position: pPos.text }, ok)
                            if (ok) page.refresh()
                        }
                    }
                    Button {
                        text: qsTr("删除")
                        onClicked: confirm.open()
                    }
                    Button {
                        text: qsTr("按 ID 查询")
                        onClicked: {
                            const id = parseInt(pId.text) || 0
                            const obj = dataService.getPersonById(id)
                            const ok = obj !== null
                            Logger.logResult("按 ID 查询人员", ok, ok ? ("name=" + obj.name) : "未命中")
                            page._record("dataService.getPersonById", { id: id }, ok)
                            if (ok) {
                                pName.text = obj.name; pEmp.text = obj.employeeId
                                pDept.text = obj.department; pPos.text = obj.position
                            }
                        }
                    }
                    Button {
                        text: qsTr("按工号查询")
                        onClicked: {
                            const obj = dataService.getPersonByEmployeeId(pEmp.text)
                            const ok = obj !== null
                            Logger.logResult("按工号查询人员", ok, ok ? ("id=" + obj.id) : "未命中")
                            page._record("dataService.getPersonByEmployeeId", { employeeId: pEmp.text }, ok)
                            if (ok) {
                                pId.text = String(obj.id); pName.text = obj.name
                                pDept.text = obj.department; pPos.text = obj.position
                            }
                        }
                    }
                    Button {
                        text: qsTr("条件筛选")
                        onClicked: {
                            const list = dataService.selectPersons(pName.text.trim(), pEmp.text.trim(),
                                                                   pDept.text.trim(), pPos.text.trim())
                            page.rows = list
                            Logger.logResult("条件筛选人员", true, "数量=" + list.length)
                            page._record("dataService.selectPersons",
                                { name: pName.text, employeeId: pEmp.text, department: pDept.text, position: pPos.text }, true)
                        }
                    }
                    Button {
                        text: qsTr("清空表单")
                        flat: true
                        onClicked: { pId.text=""; pName.text=""; pEmp.text=""; pDept.text=""; pPos.text="" }
                    }
                }
            }
        }

        Card {
            Layout.fillWidth: true
            Layout.fillHeight: true
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
                    { key: "createdAt", title: qsTr("创建时间"), width: 170,
                      formatter: function(v) { return v ? Qt.formatDateTime(v, "yyyy-MM-dd HH:mm:ss") : "" } },
                    { key: "updatedAt", title: qsTr("更新时间"),
                      formatter: function(v) { return v ? Qt.formatDateTime(v, "yyyy-MM-dd HH:mm:ss") : "" } }
                ]
                onRowClicked: function(idx, row) {
                    pId.text = String(row.id)
                    pName.text = row.name
                    pEmp.text = row.employeeId
                    pDept.text = row.department
                    pPos.text = row.position
                }
            }
        }
    }

    ConfirmDialog {
        id: confirm
        message: qsTr("确认删除工号 ") + pEmp.text + qsTr(" 的人员？")
        onAccepted: {
            const ok = dataService.deletePerson(pEmp.text)
            Logger.logResult("删除人员", ok, "工号=" + pEmp.text)
            page._record("dataService.deletePerson", { employeeId: pEmp.text }, ok)
            if (ok) page.refresh()
        }
    }
}
