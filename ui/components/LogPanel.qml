import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceAdmin

Rectangle {
    id: root
    color: Theme.surface
    border.color: Theme.border
    border.width: 0
    clip: true

    property int filterLevel: -1   // -1 = 全部
    property string keyword: ""

    Rectangle { width: parent.width; height: 1; color: Theme.border }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // ===== 工具条 =====
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 32
            color: Theme.surfaceAlt

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: Theme.spacingSm
                anchors.rightMargin: Theme.spacingSm
                spacing: Theme.spacingSm

                Label {
                    text: qsTr("日志")
                    color: Theme.text
                    font.pixelSize: Theme.fontSm
                    font.family: Theme.fontFamily
                    font.bold: true
                }

                ComboBox {
                    id: levelBox
                    Layout.preferredWidth: 110
                    model: [
                        { text: qsTr("全部级别"), value: -1 },
                        { text: qsTr("DEBUG"), value: 0 },
                        { text: qsTr("INFO"),  value: 1 },
                        { text: qsTr("WARN"),  value: 2 },
                        { text: qsTr("ERROR"), value: 3 },
                        { text: qsTr("OK"),    value: 4 }
                    ]
                    textRole: "text"
                    valueRole: "value"
                    onActivated: root.filterLevel = currentValue
                }

                TextField {
                    Layout.preferredWidth: 200
                    placeholderText: qsTr("关键词过滤…")
                    onTextChanged: root.keyword = text
                }

                Item { Layout.fillWidth: true }

                Label {
                    text: qsTr("条数 ") + Logger.model.count
                    color: Theme.textMuted
                    font.pixelSize: Theme.fontXs
                }

                Button {
                    text: qsTr("清空")
                    flat: true
                    onClicked: Logger.clear()
                }
            }
        }

        Rectangle { Layout.fillWidth: true; height: 1; color: Theme.border }

        ListView {
            id: lv
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: Logger.model
            ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }
            verticalLayoutDirection: ListView.TopToBottom

            delegate: Rectangle {
                required property var model
                required property int index
                width: lv.width
                height: visible ? rowText.implicitHeight + 6 : 0
                visible: (root.filterLevel < 0 || model.level === root.filterLevel)
                         && (root.keyword.length === 0 || model.message.indexOf(root.keyword) >= 0)
                color: index % 2 === 1 ? Theme.surfaceAlt : "transparent"

                Rectangle {
                    width: 3
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    color: {
                        switch (model.level) {
                        case Logger.levelError: return Theme.danger
                        case Logger.levelWarn:  return Theme.warning
                        case Logger.levelOk:    return Theme.success
                        case Logger.levelInfo:  return Theme.info
                        }
                        return Theme.border
                    }
                }

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 12
                    anchors.rightMargin: 8
                    spacing: 8

                    Label {
                        text: model.time
                        color: Theme.textSubtle
                        font.family: Theme.fontMono
                        font.pixelSize: Theme.fontXs
                        Layout.preferredWidth: 70
                    }
                    Label {
                        text: {
                            switch (model.level) {
                            case Logger.levelError: return "ERR"
                            case Logger.levelWarn:  return "WARN"
                            case Logger.levelOk:    return "OK"
                            case Logger.levelInfo:  return "INFO"
                            case Logger.levelDebug: return "DBG"
                            }
                            return "-"
                        }
                        color: {
                            switch (model.level) {
                            case Logger.levelError: return Theme.danger
                            case Logger.levelWarn:  return Theme.warning
                            case Logger.levelOk:    return Theme.success
                            case Logger.levelInfo:  return Theme.info
                            }
                            return Theme.textMuted
                        }
                        font.family: Theme.fontMono
                        font.pixelSize: Theme.fontXs
                        font.bold: true
                        Layout.preferredWidth: 36
                    }
                    Label {
                        id: rowText
                        text: model.message
                        color: Theme.text
                        font.family: Theme.fontMono
                        font.pixelSize: Theme.fontXs
                        Layout.fillWidth: true
                        wrapMode: Text.WrapAnywhere
                    }
                }
            }

            onCountChanged: positionViewAtEnd()
        }
    }
}
