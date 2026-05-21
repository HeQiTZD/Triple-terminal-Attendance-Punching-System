import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceAdmin

/*
  当前页面无访问权限时的占位提示。
*/
Item {
    id: root

    property string pageKey: ""
    property var sessionManager

    ColumnLayout {
        anchors.centerIn: parent
        width: Math.min(480, parent.width - Theme.spacingXl * 2)
        spacing: Theme.spacingMd

        Rectangle {
            Layout.alignment: Qt.AlignHCenter
            width: 64
            height: 64
            radius: 32
            color: Theme.surfaceAlt
            border.color: Theme.border
            border.width: 1

            Label {
                anchors.centerIn: parent
                text: "🔒"
                font.pixelSize: 28
            }
        }

        Label {
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            text: qsTr("无法访问「%1」")
                  .arg(PermissionCatalog.navLabelForKey(root.pageKey))
            color: Theme.text
            font.pixelSize: Theme.fontLg
            font.bold: true
            font.family: Theme.fontFamily
            wrapMode: Text.WordWrap
        }

        Label {
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            text: qsTr("当前账号缺少访问此模块所需的权限或角色。")
            color: Theme.textMuted
            font.pixelSize: Theme.fontMd
            font.family: Theme.fontFamily
            wrapMode: Text.WordWrap
        }

        Card {
            Layout.fillWidth: true
            title: qsTr("访问要求")

            ColumnLayout {
                width: parent.width
                spacing: Theme.spacingSm

                Label {
                    text: PermissionCatalog.accessRequirementHint(root.pageKey)
                    color: Theme.text
                    font.pixelSize: Theme.fontSm
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }

                Label {
                    visible: sessionManager && sessionManager.isLoggedIn
                    text: qsTr("您的角色：") + PermissionCatalog.formatRoles(sessionManager)
                    color: Theme.textMuted
                    font.pixelSize: Theme.fontSm
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }
            }
        }
    }
}
