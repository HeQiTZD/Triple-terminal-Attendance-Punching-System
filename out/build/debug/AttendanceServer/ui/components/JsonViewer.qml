import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceServer

/*
  JSON 只读展示：等宽字体 + 复制 / 保存到文件按钮。
  设置 jsonObject 或 text 之一即可。
*/
Rectangle {
    id: root
    color: Theme.bg
    border.color: Theme.border
    border.width: 1
    radius: Theme.radiusSm
    clip: true

    property var jsonObject: null
    property string text: ""
    property string title: ""

    implicitHeight: 200

    onJsonObjectChanged: {
        if (jsonObject !== null && jsonObject !== undefined) {
            try { ta.text = JSON.stringify(jsonObject, null, 2) }
            catch (e) { ta.text = String(jsonObject) }
        }
    }
    onTextChanged: {
        if (text.length && jsonObject === null)
            ta.text = text
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 30
            color: Theme.surfaceAlt
            visible: root.title.length > 0 || ta.text.length > 0

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: Theme.spacingSm
                anchors.rightMargin: Theme.spacingSm
                spacing: Theme.spacingSm

                Label {
                    text: root.title
                    color: Theme.textMuted
                    font.pixelSize: Theme.fontXs
                    font.family: Theme.fontFamily
                    Layout.fillWidth: true
                    elide: Text.ElideRight
                }
                Button {
                    text: qsTr("复制")
                    flat: true
                    enabled: ta.text.length > 0
                    onClicked: { ta.selectAll(); ta.copy(); ta.deselect() }
                }
                Button {
                    text: qsTr("保存…")
                    flat: true
                    enabled: ta.text.length > 0
                    onClicked: {
                        const path = testApi.selectSaveFile(qsTr("另存 JSON"), "result.json", "JSON (*.json);;所有文件 (*)")
                        if (!path) return
                        testApi.writeTextFile(path, ta.text)
                    }
                }
            }
        }

        Rectangle { Layout.fillWidth: true; height: 1; color: Theme.border; visible: root.title.length > 0 }

        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            TextArea {
                id: ta
                readOnly: true
                wrapMode: TextArea.NoWrap
                color: Theme.text
                selectByMouse: true
                font.family: Theme.fontMono
                font.pixelSize: Theme.fontSm
                background: Rectangle { color: "transparent" }
                placeholderText: qsTr("（无内容）")
            }
        }
    }
}
