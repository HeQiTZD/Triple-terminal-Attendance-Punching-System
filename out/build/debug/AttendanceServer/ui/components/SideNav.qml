pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    AppTheme { id: theme }

    property string currentKey: "dashboard"
    signal navigate(string key)

    Rectangle { anchors.fill: parent; color: theme.surface }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 14
        spacing: 10

        Label {
            text: qsTr("Attendance Admin")
            color: theme.text
            font.pixelSize: 14
            font.weight: Font.DemiBold
        }

        Label {
            text: qsTr("导航")
            color: theme.textMuted
            font.pixelSize: 11
        }

        ListView {
            id: navList
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: 6
            boundsBehavior: Flickable.StopAtBounds

            model: [
                { key: "dashboard", text: qsTr("概览") },
                { key: "people", text: qsTr("人员管理") },
                { key: "face", text: qsTr("人脸数据") },
                { key: "attendance", text: qsTr("打卡记录") },
                { key: "analyzer", text: qsTr("统计分析") },
                { key: "devices", text: qsTr("设备管控") },
                { key: "settings", text: qsTr("设置") }
            ]

            delegate: Item {
                id: delegateRoot
                required property var modelData
                width: ListView.view.width
                height: 40

                readonly property bool isActive: root.currentKey === modelData.key

                Rectangle {
                    anchors.fill: parent
                    radius: 10
                    color: delegateRoot.isActive ? theme.primarySoft : "transparent"
                    border.color: delegateRoot.isActive ? Qt.rgba(theme.primary.r, theme.primary.g, theme.primary.b, 0.25) : "transparent"
                    border.width: 1
                }

                Rectangle {
                    width: 3
                    height: parent.height - 12
                    radius: 2
                    anchors.left: parent.left
                    anchors.leftMargin: 6
                    anchors.verticalCenter: parent.verticalCenter
                    color: delegateRoot.isActive ? theme.primary : "transparent"
                }

                Rectangle {
                    width: 8
                    height: 8
                    radius: 4
                    anchors.left: parent.left
                    anchors.leftMargin: 16
                    anchors.verticalCenter: parent.verticalCenter
                    color: delegateRoot.isActive ? theme.primary : theme.border
                }

                Label {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: 34
                    text: delegateRoot.modelData.text
                    color: delegateRoot.isActive ? theme.text : theme.textMuted
                    font.pixelSize: 13
                    font.weight: delegateRoot.isActive ? Font.DemiBold : Font.Normal
                }

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: root.navigate(delegateRoot.modelData.key)
                    cursorShape: Qt.PointingHandCursor
                }
            }
        }

        AppCard {
            Layout.fillWidth: true
            height: 72

            RowLayout {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 10

                Rectangle {
                    width: 10; height: 10; radius: 5
                    color: theme.success
                    Layout.alignment: Qt.AlignVCenter
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 1

                    Label {
                        text: qsTr("服务端状态")
                        color: theme.textMuted
                        font.pixelSize: 11
                    }
                    Label {
                        text: qsTr("运行中")
                        color: theme.text
                        font.pixelSize: 13
                        font.weight: Font.DemiBold
                    }
                }

                Label {
                    text: qsTr("v0.1")
                    color: theme.textMuted
                    font.pixelSize: 11
                    Layout.alignment: Qt.AlignVCenter
                }
            }
        }
    }
}
