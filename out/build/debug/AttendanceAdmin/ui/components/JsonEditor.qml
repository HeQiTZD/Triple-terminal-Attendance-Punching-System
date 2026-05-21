import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceAdmin

/*
  JSON 编辑器：等宽字体 + 工具条（格式化、压缩、校验、清空、从文件读、复制）。
  text 属性双向暴露内容；status 显示校验/操作结果。
*/
Rectangle {
    id: root
    color: Theme.bg
    border.color: Theme.border
    border.width: 1
    radius: Theme.radiusSm
    clip: true

    property alias text: ta.text
    property string placeholder: qsTr("请输入 JSON 文本…")
    property bool readOnlyMode: false
    property string status: ""
    property color statusColor: Theme.textMuted

    implicitHeight: 200

    function format() {
        try {
            const obj = JSON.parse(ta.text || "null")
            ta.text = JSON.stringify(obj, null, 2)
            _setStatus("已格式化", Theme.success)
        } catch (e) {
            _setStatus("解析失败：" + e.message, Theme.danger)
        }
    }

    function compact() {
        try {
            const obj = JSON.parse(ta.text || "null")
            ta.text = JSON.stringify(obj)
            _setStatus("已压缩", Theme.success)
        } catch (e) {
            _setStatus("解析失败：" + e.message, Theme.danger)
        }
    }

    function validate() {
        try {
            JSON.parse(ta.text || "null")
            _setStatus("JSON 格式合法", Theme.success)
            return true
        } catch (e) {
            _setStatus("解析失败：" + e.message, Theme.danger)
            return false
        }
    }

    function _setStatus(msg, c) {
        root.status = msg
        root.statusColor = c
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // 工具条
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 30
            color: Theme.surfaceAlt

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: Theme.spacingSm
                anchors.rightMargin: Theme.spacingSm
                spacing: Theme.spacingSm

                Button {
                    text: qsTr("格式化")
                    flat: true
                    onClicked: root.format()
                }
                Button {
                    text: qsTr("压缩")
                    flat: true
                    onClicked: root.compact()
                }
                Button {
                    text: qsTr("校验")
                    flat: true
                    onClicked: root.validate()
                }
                Button {
                    text: qsTr("清空")
                    flat: true
                    onClicked: { ta.text = ""; root._setStatus("已清空", Theme.textMuted) }
                }
                Button {
                    text: qsTr("复制")
                    flat: true
                    onClicked: { ta.selectAll(); ta.copy(); root._setStatus("已复制到剪贴板", Theme.info) }
                }

                Item { Layout.fillWidth: true }

                Label {
                    text: root.status
                    color: root.statusColor
                    font.pixelSize: Theme.fontXs
                    font.family: Theme.fontFamily
                    elide: Text.ElideRight
                    Layout.maximumWidth: 320
                }
            }
        }

        Rectangle { Layout.fillWidth: true; height: 1; color: Theme.border }

        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            TextArea {
                id: ta
                readOnly: root.readOnlyMode
                placeholderText: root.placeholder
                wrapMode: TextArea.NoWrap
                color: Theme.text
                selectByMouse: true
                font.family: Theme.fontMono
                font.pixelSize: Theme.fontSm
                tabStopDistance: 16
            }
        }
    }
}
