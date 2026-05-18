import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceAdmin

Dialog {
    id: dlg
    modal: true
    anchors.centerIn: Overlay.overlay
    title: qsTr("权限不足")
    standardButtons: Dialog.Ok

    property string missingKey: ""

    function openFor(key) {
        missingKey = key || ""
        open()
    }

    contentItem: ColumnLayout {
        width: Math.min(420, Overlay.overlay ? Overlay.overlay.width * 0.85 : 420)
        spacing: Theme.spacingSm

        Label {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            text: qsTr("当前账号缺少所需权限或角色，无法执行此操作。")
            color: Theme.text
            font.pixelSize: Theme.fontMd
            font.family: Theme.fontFamily
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: hintCol.implicitHeight + Theme.spacingMd * 2
            radius: Theme.radiusSm
            color: Theme.surfaceAlt
            border.color: Theme.border
            border.width: 1

            ColumnLayout {
                id: hintCol
                anchors.fill: parent
                anchors.margins: Theme.spacingMd
                spacing: Theme.spacingXs

                Label {
                    visible: dlg.missingKey.length > 0
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    text: qsTr("需要：") + dlg.missingKey
                    color: Theme.danger
                    font.pixelSize: Theme.fontSm
                    font.bold: true
                    font.family: Theme.fontFamily
                }
            }
        }

        Label {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            text: qsTr("请联系管理员在「角色权限」中为您分配相应角色。")
            color: Theme.textMuted
            font.pixelSize: Theme.fontSm
            font.family: Theme.fontFamily
        }
    }
}
