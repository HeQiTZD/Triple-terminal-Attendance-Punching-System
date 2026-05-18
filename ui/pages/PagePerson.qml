import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceAdmin

Item {
    id: page

    required property var personServer
    required property var sessionManager
    property var deniedDialog: null
    signal serviceResult(string apiType, int code, string message)

    readonly property bool canUpdate: PermissionCatalog.hasPerm(sessionManager, "person.update")

    function _query() {
        personServer.queryPersons(pName.text.trim(), pEmp.text.trim(),
                                  pDept.text.trim(), pPos.text.trim(), "", "")
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingLg
        spacing: Theme.spacingMd

        ToolBarRow {
            Layout.fillWidth: true
            title: qsTr("人员管理")
            subtitle: qsTr("新增 / 修改 / 删除 / 查询人员")
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
                    TextField {
                        id: pName
                        readOnly: !page.canUpdate
                        placeholderText: qsTr("张三")
                        Layout.fillWidth: true
                    }
                }
                LabeledField { label: qsTr("工号"); Layout.fillWidth: true
                    TextField {
                        id: pEmp
                        readOnly: !page.canUpdate
                        placeholderText: qsTr("EMP001")
                        Layout.fillWidth: true
                    }
                }
                LabeledField { label: qsTr("部门"); Layout.fillWidth: true
                    TextField {
                        id: pDept
                        readOnly: !page.canUpdate
                        placeholderText: qsTr("研发部")
                        Layout.fillWidth: true
                    }
                }
                LabeledField { label: qsTr("岗位"); Layout.fillWidth: true
                    TextField {
                        id: pPos
                        readOnly: !page.canUpdate
                        placeholderText: qsTr("工程师")
                        Layout.fillWidth: true
                    }
                }

                Row {
                    Layout.columnSpan: 4
                    spacing: Theme.spacingSm
                    PermissionButton {
                        sessionManager: page.sessionManager
                        requiredPermission: "person.create"
                        deniedDialog: page.deniedDialog
                        text: qsTr("新增")
                        highlighted: true
                        enabled: !personServer.busy
                        onClicked: guardedClick(function() {
                            personServer.createPerson(pName.text.trim(), pEmp.text.trim(),
                                                      pDept.text.trim(), pPos.text.trim())
                        })
                    }
                    PermissionButton {
                        sessionManager: page.sessionManager
                        requiredPermission: "person.update"
                        deniedDialog: page.deniedDialog
                        text: qsTr("修改")
                        enabled: !personServer.busy
                        onClicked: guardedClick(function() {
                            personServer.updatePerson(pEmp.text.trim(), pName.text.trim(),
                                                      pDept.text.trim(), pPos.text.trim())
                        })
                    }
                    PermissionButton {
                        sessionManager: page.sessionManager
                        requiredPermission: "person.delete"
                        deniedDialog: page.deniedDialog
                        text: qsTr("删除")
                        enabled: !personServer.busy
                        onClicked: guardedClick(function() { confirm.open() })
                    }
                    PermissionButton {
                        sessionManager: page.sessionManager
                        requiredPermission: "person.read"
                        deniedDialog: page.deniedDialog
                        text: qsTr("查询")
                        enabled: !personServer.busy
                        onClicked: guardedClick(page._query)
                    }
                    PermissionButton {
                        sessionManager: page.sessionManager
                        requiredPermission: "person.read"
                        deniedDialog: page.deniedDialog
                        text: qsTr("清空表单")
                        flat: true
                        onClicked: guardedClick(function() {
                            pName.text = ""; pEmp.text = ""; pDept.text = ""; pPos.text = ""
                        })
                    }
                }
            }
        }

        Card {
            Layout.fillWidth: true
            Layout.fillHeight: true
            stretchContent: true
            title: qsTr("人员列表")
            subtitle: page.canUpdate ? qsTr("点击行加载到表单") : qsTr("只读列表")

            DataTable {
                id: table
                anchors.fill: parent
                rows: personServer.records
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
                    if (!page.canUpdate) return
                    pName.text = row.name || ""
                    pEmp.text = row.employeeId || ""
                    pDept.text = row.department || ""
                    pPos.text = row.position || ""
                }
            }
        }
    }

    BusyOverlay { busy: personServer.busy }

    ConfirmDialog {
        id: confirm
        message: qsTr("确认删除工号 ") + pEmp.text + qsTr(" 的人员？")
        onAccepted: personServer.deletePerson(pEmp.text.trim())
    }

    Connections {
        target: personServer
        function onOperationSucceeded(apiType, message) {
            page.serviceResult(apiType, 0, message)
            if (apiType.indexOf("create") >= 0 || apiType.indexOf("update") >= 0
                    || apiType.indexOf("delete") >= 0)
                page._query()
        }
        function onOperationFailed(apiType, code, message) {
            page.serviceResult(apiType, code, message)
        }
    }
}
