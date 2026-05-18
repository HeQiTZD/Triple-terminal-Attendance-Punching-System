import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

import AttendanceAdmin

Item {
    id: page

    required property var faceServer
    required property var sessionManager
    property var deniedDialog: null
    signal serviceResult(string apiType, int code, string message)

    property bool overwriteFace: false

    function _rowsFromServer() {
        if (faceServer.lastFound && faceServer.lastRecord)
            return [faceServer.lastRecord]
        return faceServer.records
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingLg
        spacing: Theme.spacingMd

        ToolBarRow {
            Layout.fillWidth: true
            title: qsTr("人脸数据管理")
            subtitle: qsTr("按工号查询 / 注册 / 删除（Base64 照片）")
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
                        label: qsTr("工号")
                        Layout.fillWidth: true
                        TextField {
                            id: faceEmp
                            text: Presets.defaultEmployeeId
                            Layout.fillWidth: true
                        }
                    }
                    CheckBox {
                        id: overwriteCb
                        visible: PermissionCatalog.hasPerm(sessionManager, "face.register")
                        text: qsTr("覆盖已有")
                        checked: page.overwriteFace
                        onCheckedChanged: page.overwriteFace = checked
                    }
                    PermissionButton {
                        sessionManager: page.sessionManager
                        requiredPermission: "face.read"
                        deniedDialog: page.deniedDialog
                        text: qsTr("查询")
                        enabled: !faceServer.busy
                        onClicked: guardedClick(function() {
                            faceServer.queryFace(faceEmp.text.trim())
                        })
                    }
                    PermissionButton {
                        sessionManager: page.sessionManager
                        requiredPermission: "face.register"
                        deniedDialog: page.deniedDialog
                        text: qsTr("注册人脸")
                        highlighted: true
                        enabled: !faceServer.busy
                        onClicked: guardedClick(function() { photoDialog.open() })
                    }
                    PermissionButton {
                        sessionManager: page.sessionManager
                        requiredPermission: "face.delete"
                        deniedDialog: page.deniedDialog
                        text: qsTr("删除")
                        enabled: !faceServer.busy
                        onClicked: guardedClick(function() { confirm.open() })
                    }
                }

                Label {
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    visible: faceServer.lastFound
                    text: qsTr("最近查询：特征长度 %1 · 更新 %2")
                          .arg(faceServer.lastRecord.featureSize || "-")
                          .arg(faceServer.lastRecord.updatedAt || "-")
                    color: Theme.textMuted
                    font.pixelSize: Theme.fontSm
                }

                DataTable {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 120
                    rows: page._rowsFromServer()
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

    BusyOverlay { busy: faceServer.busy }

    FileDialog {
        id: photoDialog
        title: qsTr("选择人脸照片")
        nameFilters: [qsTr("图片 (*.jpg *.jpeg *.png *.bmp)")]
        onAccepted: {
            const path = selectedFile.toString()
            faceServer.registerFaceFromFile(faceEmp.text.trim(), path, page.overwriteFace)
        }
    }

    ConfirmDialog {
        id: confirm
        message: qsTr("确认删除工号 ") + faceEmp.text + qsTr(" 的人脸特征？")
        onAccepted: faceServer.deleteFace(faceEmp.text.trim())
    }

    Connections {
        target: faceServer
        function onOperationSucceeded(apiType, message) {
            page.serviceResult(apiType, 0, message)
        }
        function onOperationFailed(apiType, code, message) {
            page.serviceResult(apiType, code, message)
        }
        function onQueryCompleted(found, record) {
            if (found)
                Logger.info(qsTr("已找到人脸：") + (record.employeeId || ""))
            else
                Logger.warn(qsTr("未找到该工号的人脸数据"))
        }
    }
}
