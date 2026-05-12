import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceAdmin

/*
  标签 + 输入控件 RowLayout。
  约定：用户传入的控件应设置 Layout.fillWidth: true（或固定宽度）。
*/
RowLayout {
    id: root
    property string label: ""
    property int labelWidth: 80
    property bool labelBold: false
    spacing: Theme.spacingSm

    Label {
        text: root.label
        color: Theme.textMuted
        font.pixelSize: Theme.fontSm
        font.family: Theme.fontFamily
        font.bold: root.labelBold
        Layout.preferredWidth: root.labelWidth
        elide: Text.ElideRight
    }
}
