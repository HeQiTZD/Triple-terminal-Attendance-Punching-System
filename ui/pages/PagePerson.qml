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

    property string confirmAction: ""

    function _openConfirm(action) {
        confirmAction = action
        confirm.open()
    }

    function _confirmText() {
        const emp = pEmp.text.trim() || qsTr("(空)")
        const name = pName.text.trim()
        const info = name ? emp + " " + name : emp
        switch (confirmAction) {
        case "create": return qsTr("确认新增人员 ") + info + "？"
        case "update": return qsTr("确认修改人员 ") + info + "？"
        case "delete": return qsTr("确认删除人员 ") + info + "？"
        default: return ""
        }
    }

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
                        Layout.fillWidth: true
                    }
                }
                LabeledField { label: qsTr("工号"); Layout.fillWidth: true
                    TextField {
                        id: pEmp
                        readOnly: !page.canUpdate
                        Layout.fillWidth: true
                    }
                }
                LabeledField { label: qsTr("部门"); Layout.fillWidth: true
                    TextField {
                        id: pDept
                        readOnly: !page.canUpdate
                        Layout.fillWidth: true
                    }
                }
                LabeledField { label: qsTr("岗位"); Layout.fillWidth: true
                    TextField {
                        id: pPos
                        readOnly: !page.canUpdate
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
                            page._openConfirm("create")
                        })
                    }
                    PermissionButton {
                        sessionManager: page.sessionManager
                        requiredPermission: "person.update"
                        deniedDialog: page.deniedDialog
                        text: qsTr("修改")
                        enabled: !personServer.busy
                        onClicked: guardedClick(function() {
                            page._openConfirm("update")
                        })
                    }
                    PermissionButton {
                        sessionManager: page.sessionManager
                        requiredPermission: "person.delete"
                        deniedDialog: page.deniedDialog
                        text: qsTr("删除")
                        enabled: !personServer.busy
                        onClicked: guardedClick(function() { page._openConfirm("delete") })
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
                    { key: "id", title: qsTr("序号"), width: 60, align: "right",
                      formatter: function(v, row, idx) { return String(idx + 1) } },
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

    Popup {
        id: confirm
        parent: page
        modal: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        padding: Theme.spacingLg

        readonly property real _w: Math.min(420, parent ? parent.width - 48 : 372)

        x: parent ? Math.round((parent.width - _w) / 2) : 0
        y: parent ? Math.round((parent.height - (_h > 0 ? _h : 120)) / 2) : 0
        width: _w

        readonly property real _h: implicitHeight > 0 ? implicitHeight : height

        background: Rectangle {
            color: Theme.surface
            border.color: Theme.border
            border.width: 1
            radius: Theme.radiusMd
        }

        contentItem: ColumnLayout {
            spacing: Theme.spacingMd
            Label {
                Layout.fillWidth: true
                text: page._confirmText()
                wrapMode: Text.WordWrap
                color: Theme.text
                font.pixelSize: Theme.fontMd
                font.family: Theme.fontFamily
            }
            RowLayout {
                Layout.alignment: Qt.AlignRight
                spacing: Theme.spacingSm
                Button {
                    text: qsTr("取消")
                    flat: true
                    onClicked: {
                        confirm.close()
                        page.confirmAction = ""
                    }
                }
                Button {
                    text: qsTr("确认")
                    highlighted: true
                    onClicked: {
                        const emp = pEmp.text.trim()
                        if (page.confirmAction === "create")
                            personServer.createPerson(pName.text.trim(), emp,
                                                      pDept.text.trim(), pPos.text.trim())
                        else if (page.confirmAction === "update")
                            personServer.updatePerson(emp, pName.text.trim(),
                                                      pDept.text.trim(), pPos.text.trim())
                        else if (page.confirmAction === "delete")
                            personServer.deletePerson(emp)
                        confirm.close()
                        page.confirmAction = ""
                    }
                }
            }
        }
    }

    Popup {
        id: toast
        parent: page
        modal: false
        closePolicy: Popup.NoAutoClose
        padding: Theme.spacingMd

        property bool isError: false
        readonly property real _w: Math.min(360, parent ? parent.width - 48 : 312)

        x: parent ? Math.round((parent.width - _w) / 2) : 0
        y: parent ? Theme.spacingMd : 0
        width: _w

        background: Rectangle {
            color: toast.isError ? Theme.danger : Theme.success
            radius: Theme.radiusMd
            opacity: 0.92
        }

        contentItem: Label {
            id: toastLabel
            text: ""
            color: "white"
            font.pixelSize: Theme.fontMd
            font.family: Theme.fontFamily
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            Layout.fillWidth: true
        }

        Timer {
            id: toastTimer
            interval: 2200
            onTriggered: toast.close()
        }

        function show(msg, isErr) {
            toastLabel.text = msg
            toast.isError = isErr
            toast.open()
            toastTimer.restart()
        }
    }

    Connections {
        target: personServer
        function onOperationSucceeded(apiType, message) {
            page.serviceResult(apiType, 0, message)
            if (apiType.indexOf("create") >= 0 || apiType.indexOf("update") >= 0
                    || apiType.indexOf("delete") >= 0) {
                page._query()
                toast.show(message || qsTr("操作成功"), false)
            }
        }
        function onOperationFailed(apiType, code, message) {
            page.serviceResult(apiType, code, message)
            toast.show(ErrorCatalog.messageForCode(code, message) || message || qsTr("操作失败"), true)
        }
    }
}
