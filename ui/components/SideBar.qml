import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceAdmin

/*
  侧边栏导航：
    items: [{ key, label, group?, icon? }]
  发出 currentIndex 变化信号。
*/
Rectangle {
    id: root
    color: Theme.sideBar

    property var items: []
    property int currentIndex: 0
    signal navigated(int index)

    implicitWidth: Theme.sideBarWidth

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // ===== Logo / 标题 =====
        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: 56

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: Theme.spacingMd
                anchors.rightMargin: Theme.spacingMd
                spacing: Theme.spacingSm

                Rectangle {
                    width: 28; height: 28
                    radius: 6
                    color: Theme.accent
                    Label {
                        anchors.centerIn: parent
                        text: "A"
                        color: "white"
                        font.bold: true
                        font.pixelSize: 16
                    }
                }
                ColumnLayout {
                    spacing: 0
                    Layout.fillWidth: true
                    Label {
                        text: qsTr("Attendance")
                        color: Theme.text
                        font.pixelSize: Theme.fontMd
                        font.bold: true
                        font.family: Theme.fontFamily
                    }
                    Label {
                        text: qsTr("Test Workbench")
                        color: Theme.textMuted
                        font.pixelSize: Theme.fontXs
                        font.family: Theme.fontFamily
                    }
                }
            }
        }

        Rectangle { Layout.fillWidth: true; height: 1; color: Theme.border }

        ListView {
            id: listView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: root.items
            currentIndex: root.currentIndex
            spacing: 1
            interactive: true

            delegate: Item {
                id: row
                required property var modelData
                required property int index
                width: ListView.view.width
                height: modelData.group ? 26 : 36

                // 分组小标题
                Label {
                    visible: !!row.modelData.group
                    text: row.modelData.group ? row.modelData.group : ""
                    color: Theme.textSubtle
                    font.pixelSize: Theme.fontXs
                    font.family: Theme.fontFamily
                    font.capitalization: Font.AllUppercase
                    anchors.left: parent.left
                    anchors.leftMargin: Theme.spacingMd
                    anchors.verticalCenter: parent.verticalCenter
                }

                // 普通条目
                Rectangle {
                    visible: !row.modelData.group
                    anchors.fill: parent
                    anchors.leftMargin: 4
                    anchors.rightMargin: 4
                    radius: Theme.radiusSm
                    color: row.index === root.currentIndex
                           ? Theme.selected
                           : (mouseArea.containsMouse ? Theme.hover : "transparent")

                    Rectangle {
                        width: 3
                        height: parent.height - 6
                        anchors.left: parent.left
                        anchors.verticalCenter: parent.verticalCenter
                        radius: 1.5
                        color: row.index === root.currentIndex ? Theme.accent : "transparent"
                    }

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 14
                        anchors.rightMargin: 8
                        spacing: 8

                        Label {
                            text: row.modelData.icon ? row.modelData.icon : "•"
                            color: row.index === root.currentIndex ? Theme.accentHover : Theme.textMuted
                            font.pixelSize: Theme.fontMd
                            Layout.preferredWidth: 16
                        }
                        Label {
                            text: row.modelData.label
                            color: row.index === root.currentIndex ? Theme.text : Theme.textMuted
                            font.pixelSize: Theme.fontMd
                            font.family: Theme.fontFamily
                            font.bold: row.index === root.currentIndex
                            Layout.fillWidth: true
                            elide: Text.ElideRight
                        }
                    }

                    MouseArea {
                        id: mouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            root.currentIndex = row.index
                            root.navigated(row.index)
                        }
                    }
                }
            }
        }

        Rectangle { Layout.fillWidth: true; height: 1; color: Theme.border }

        Label {
            Layout.fillWidth: true
            Layout.margins: Theme.spacingMd
            text: qsTr("v0.1 · Qt 6 · QML")
            color: Theme.textSubtle
            font.pixelSize: Theme.fontXs
            font.family: Theme.fontFamily
            horizontalAlignment: Text.AlignHCenter
        }
    }
}
