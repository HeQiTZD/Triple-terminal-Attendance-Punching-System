import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceAdmin

Window {
    id: root

    property var ownerWindow: null

    readonly property int fixedWidth: 400
    readonly property int fixedHeight: 260

    width: fixedWidth
    height: fixedHeight
    minimumWidth: fixedWidth
    maximumWidth: fixedWidth
    minimumHeight: fixedHeight
    maximumHeight: fixedHeight
    modality: Qt.ApplicationModal
    flags: Qt.Window | Qt.FramelessWindowHint
    color: "transparent"

    function open() {
        if (ownerWindow)
            transientParent = ownerWindow
        hostField.text = Presets.serverHost
        portSpin.value = Presets.serverPort
        _centerOnOwner()
        show()
        requestActivate()
    }

    function _centerOnOwner() {
        const owner = ownerWindow || transientParent
        if (!owner)
            return
        x = owner.x + Math.round((owner.width - width) / 2)
        y = owner.y + Math.round((owner.height - height) / 2)
    }

    Rectangle {
        anchors.fill: parent
        color: Theme.surface
        border.color: Theme.border
        border.width: 1
        radius: Theme.radiusMd

        Item {
            id: titleBar
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            height: 40

            Label {
                anchors.centerIn: parent
                text: qsTr("设置服务器地址和端口")
                color: Theme.text
                font.pixelSize: Theme.fontMd
                font.bold: true
                font.family: Theme.fontFamily
            }

            MouseArea {
                anchors.fill: parent
                anchors.rightMargin: 36
                cursorShape: Qt.ArrowCursor
                onPressed: root.startSystemMove()
            }

            ToolButton {
                anchors.right: parent.right
                anchors.rightMargin: Theme.spacingXs
                anchors.verticalCenter: parent.verticalCenter
                z: 1
                text: "\u00d7"
                font.pixelSize: Theme.fontXl
                onClicked: root.close()
            }
        }

        ColumnLayout {
            id: contentLayout
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: titleBar.bottom
            anchors.bottom: parent.bottom
            anchors.margins: Theme.spacingLg
            spacing: Theme.spacingMd

            GridLayout {
                Layout.fillWidth: true
                columns: 2
                rowSpacing: Theme.spacingSm
                columnSpacing: Theme.spacingMd

                LabeledField {
                    label: qsTr("服务器地址")
                    Layout.fillWidth: true
                    Layout.columnSpan: 2
                    TextField {
                        id: hostField
                        Layout.fillWidth: true
                        placeholderText: "127.0.0.1"
                    }
                }
                LabeledField {
                    label: qsTr("端口")
                    Layout.fillWidth: true
                    Layout.columnSpan: 2
                    SpinBox {
                        id: portSpin
                        from: 1
                        to: 65535
                        editable: true
                        Layout.fillWidth: true
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: Theme.spacingSm

                Item { Layout.fillWidth: true }

                Button {
                    text: qsTr("取消")
                    onClicked: root.close()
                }
                Button {
                    text: qsTr("确定")
                    highlighted: true
                    enabled: hostField.text.trim().length > 0
                    onClicked: {
                        Presets.serverHost = hostField.text.trim()
                        Presets.serverPort = portSpin.value
                        Presets.notifyChanged()
                        root.close()
                    }
                }
            }
        }
    }
}
