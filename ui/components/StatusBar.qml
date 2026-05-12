import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceAdmin

Rectangle {
    id: root
    color: Theme.surface
    border.color: Theme.border
    border.width: 0
    implicitHeight: Theme.statusBarHeight

    property string lastError: ""
    property string lastInfo: ""

    Rectangle { width: parent.width; height: 1; color: Theme.border; anchors.bottom: parent.bottom }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: Theme.spacingMd
        anchors.rightMargin: Theme.spacingMd
        spacing: Theme.spacingLg

        Label {
            text: qsTr("考勤管理端")
            color: Theme.text
            font.pixelSize: Theme.fontSm
            font.family: Theme.fontFamily
            font.bold: true
        }

        Item { Layout.fillWidth: true }

        Label {
            text: root.lastError.length
                  ? (qsTr("错误：") + root.lastError)
                  : (root.lastInfo.length ? (qsTr("提示：") + root.lastInfo) : "")
            color: root.lastError.length ? Theme.danger : Theme.textMuted
            font.pixelSize: Theme.fontSm
            font.family: Theme.fontFamily
            elide: Text.ElideRight
            Layout.maximumWidth: 600
            horizontalAlignment: Text.AlignRight
        }
    }
}
