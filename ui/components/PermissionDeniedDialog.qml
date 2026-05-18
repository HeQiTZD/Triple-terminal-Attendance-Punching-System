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

    contentItem: Label {
        width: Math.min(400, Overlay.overlay ? Overlay.overlay.width * 0.8 : 400)
        wrapMode: Text.WordWrap
        text: qsTr("当前账号缺少所需权限或角色，无法执行此操作。")
              + (dlg.missingKey.length ? ("\n\n" + qsTr("需要：") + dlg.missingKey) : "")
              + "\n\n" + qsTr("请联系管理员分配权限。")
        color: Theme.text
        font.pixelSize: Theme.fontMd
    }
}
