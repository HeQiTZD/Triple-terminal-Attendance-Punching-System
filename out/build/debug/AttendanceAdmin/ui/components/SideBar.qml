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
    property var sessionManager: null
    property bool showLogout: false
    signal navigated(int index)
    signal logoutRequested()

    implicitWidth: Theme.sideBarWidth

    readonly property string userName: {
        if (!sessionManager || !sessionManager.isLoggedIn)
            return ""
        return sessionManager.currentUsername || ""
    }

    readonly property string roleSummary: {
        if (!sessionManager || !sessionManager.isLoggedIn)
            return ""
        if (sessionManager) {
            const _p = sessionManager.permissions
            const _r = sessionManager.roles
            void _p
            void _r
        }
        return PermissionCatalog.formatRoles(sessionManager)
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: 56

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton
                onPressed: function(mouse) {
                    const w = Window.window
                    if (w && typeof w.startSystemMove === "function")
                        w.startSystemMove()
                }
            }

            RowLayout {
                z: 1
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
                        text: qsTr("管理端")
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
                height: modelData.group ? 26 : 38

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

                Rectangle {
                    visible: !row.modelData.group
                    anchors.fill: parent
                    anchors.leftMargin: 6
                    anchors.rightMargin: 6
                    radius: Theme.radiusSm
                    color: row.index === root.currentIndex
                           ? Theme.selected
                           : (mouseArea.containsMouse ? Theme.hover : "transparent")

                    Rectangle {
                        width: 3
                        height: parent.height - 8
                        anchors.left: parent.left
                        anchors.verticalCenter: parent.verticalCenter
                        radius: 1.5
                        color: row.index === root.currentIndex ? Theme.accent : "transparent"
                    }

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 12
                        anchors.rightMargin: 8
                        spacing: 8

                        Label {
                            text: row.modelData.icon ? row.modelData.icon : "•"
                            color: row.index === root.currentIndex ? Theme.accentHover : Theme.textMuted
                            font.pixelSize: Theme.fontMd
                            Layout.preferredWidth: 20
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

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: Theme.border
            visible: root.showLogout && root.userName.length > 0
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.leftMargin: Theme.spacingSm
            Layout.rightMargin: Theme.spacingSm
            Layout.preferredHeight: userBlock.implicitHeight + Theme.spacingMd * 2
            visible: root.showLogout && root.userName.length > 0
            radius: Theme.radiusSm
            color: Theme.surface
            border.color: Theme.border
            border.width: 1

            ColumnLayout {
                id: userBlock
                anchors.fill: parent
                anchors.margins: Theme.spacingSm
                spacing: 2

                Label {
                    text: root.userName
                    color: Theme.text
                    font.pixelSize: Theme.fontSm
                    font.bold: true
                    font.family: Theme.fontFamily
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }
                Label {
                    text: root.roleSummary
                    color: Theme.textMuted
                    font.pixelSize: Theme.fontXs
                    font.family: Theme.fontFamily
                    wrapMode: Text.WordWrap
                    maximumLineCount: 2
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }
            }
        }

        Rectangle { Layout.fillWidth: true; height: 1; color: Theme.border }

        Button {
            Layout.fillWidth: true
            Layout.leftMargin: Theme.spacingSm
            Layout.rightMargin: Theme.spacingSm
            Layout.bottomMargin: Theme.spacingXs
            visible: root.showLogout
            text: qsTr("退出登录")
            flat: true
            onClicked: root.logoutRequested()
        }

        Label {
            Layout.fillWidth: true
            Layout.margins: Theme.spacingMd
            text: qsTr("v0.1 · Qt 6")
            color: Theme.textSubtle
            font.pixelSize: Theme.fontXs
            font.family: Theme.fontFamily
            horizontalAlignment: Text.AlignHCenter
        }
    }
}
