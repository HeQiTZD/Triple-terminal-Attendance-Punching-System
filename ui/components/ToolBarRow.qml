import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceAdmin

/*
  统一页面顶部工具条：标题 + 子标题 + 右侧操作按钮槽。
*/
Rectangle {
    id: root
    color: "transparent"
    implicitHeight: 56

    property string title: ""
    property string subtitle: ""
    property alias actions: actionSlot.data

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: Theme.spacingLg
        anchors.rightMargin: Theme.spacingLg
        spacing: Theme.spacingMd

        ColumnLayout {
            spacing: 0
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter

            Label {
                text: root.title
                color: Theme.text
                font.pixelSize: Theme.fontXl
                font.family: Theme.fontFamily
                font.bold: true
                Layout.fillWidth: true
                elide: Text.ElideRight
            }
            Label {
                text: root.subtitle
                color: Theme.textMuted
                font.pixelSize: Theme.fontSm
                font.family: Theme.fontFamily
                visible: text.length > 0
                Layout.fillWidth: true
                elide: Text.ElideRight
            }
        }

        Row {
            id: actionSlot
            spacing: Theme.spacingSm
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
        }
    }

    Rectangle {
        anchors.bottom: parent.bottom
        width: parent.width
        height: 1
        color: Theme.border
    }
}
