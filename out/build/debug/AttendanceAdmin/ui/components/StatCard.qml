import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: card

    property string title: ""
    property string value: "—"
    property string subtitle: ""
    property string trend: ""
    property bool trendPositive: true
    property string icon: ""
    property color iconColor: Theme.primary
    property color valueColor: Theme.text
    property int animationDuration: 300

    color: Theme.surface
    radius: Theme.radiusMd
    border.color: Theme.border
    border.width: 1

    implicitWidth: parent ? parent.width : 200
    implicitHeight: 120

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingLg
        spacing: Theme.spacingSm

        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingSm

            Item {
                Layout.preferredWidth: 32
                Layout.preferredHeight: 32
                visible: card.icon.length > 0

                Rectangle {
                    anchors.fill: parent
                    radius: Theme.radiusSm
                    color: card.iconColor + "20"
                }

                Label {
                    anchors.centerIn: parent
                    text: card.icon
                    font.pixelSize: Theme.fontLg
                    color: card.iconColor
                }
            }

            Label {
                Layout.fillWidth: true
                text: card.title
                font.pixelSize: Theme.fontSm
                font.family: Theme.fontFamily
                color: Theme.textMuted
                elide: Text.ElideRight
                verticalAlignment: Text.AlignVCenter
            }

            Rectangle {
                id: trendBadge
                visible: card.trend.length > 0
                radius: Theme.radiusSm
                color: card.trendPositive ? Theme.success + "20" : Theme.danger + "20"
                Layout.preferredHeight: 22
                Layout.preferredWidth: trendBadgeContent.implicitWidth + 12

                Label {
                    id: trendBadgeContent
                    anchors.centerIn: parent
                    text: card.trendPositive ? "↑ " + card.trend : "↓ " + card.trend
                    font.pixelSize: Theme.fontXs
                    font.family: Theme.fontFamily
                    font.bold: true
                    color: card.trendPositive ? Theme.success : Theme.danger
                }
            }
        }

        Label {
            id: valueLabel
            Layout.fillWidth: true
            text: card.value
            font.pixelSize: Theme.fontXxl
            font.family: Theme.fontFamily
            font.bold: true
            color: card.valueColor
            elide: Text.ElideRight
            verticalAlignment: Text.AlignTop
        }

        Label {
            visible: card.subtitle.length > 0
            Layout.fillWidth: true
            text: card.subtitle
            font.pixelSize: Theme.fontSm
            font.family: Theme.fontFamily
            color: Theme.textSubtle
            elide: Text.ElideRight
            verticalAlignment: Text.AlignTop
        }

        Item {
            Layout.fillHeight: true
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true

        onEntered: {
            card.color = Theme.hover
        }

        onExited: {
            card.color = Theme.surface
        }

        onClicked: {
            card.clicked()
        }
    }

    signal clicked()

    Behavior on color {
        ColorAnimation {
            duration: 150
        }
    }

    Behavior on value {
        SequentialAnimation {
            NumberAnimation {
                target: valueLabel
                property: "opacity"
                from: 1
                to: 0.5
                duration: card.animationDuration / 2
            }
            NumberAnimation {
                target: valueLabel
                property: "opacity"
                from: 0.5
                to: 1
                duration: card.animationDuration / 2
            }
        }
    }
}
