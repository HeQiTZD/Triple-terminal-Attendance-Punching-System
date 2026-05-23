import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceAdmin

Item {
    id: root

    property var records: []
    property string emptyText: qsTr("暂无打卡记录")

    readonly property string todayStr: Qt.formatDateTime(new Date(), "yyyy-MM-dd")
    readonly property string yesterdayStr: {
        const y = new Date()
        y.setDate(y.getDate() - 1)
        return Qt.formatDateTime(y, "yyyy-MM-dd")
    }

    function formatDateGroup(dateStr) {
        if (dateStr === todayStr) return qsTr("今天")
        if (dateStr === yesterdayStr) return qsTr("昨天")
        return dateStr
    }

    function formatTime(checkTime) {
        if (!checkTime) return ""
        const idx = checkTime.indexOf(" ")
        return idx >= 0 ? checkTime.substring(idx + 1) : checkTime
    }

    function statusDotColor(status) {
        return Theme.attendancePieColor(status)
    }

    ListView {
        id: listView
        anchors.fill: parent
        model: root.records
        clip: true
        spacing: 0
        ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }

        section.property: "dateGroup"
        section.criteria: ViewSection.FullString
        section.delegate: Rectangle {
            required property string section
            width: listView.width
            height: 28
            color: Theme.surfaceAlt

            Label {
                anchors.left: parent.left
                anchors.leftMargin: Theme.spacingMd
                anchors.verticalCenter: parent.verticalCenter
                text: root.formatDateGroup(section)
                font.pixelSize: Theme.fontSm
                font.bold: true
                font.family: Theme.fontFamily
                color: Theme.text
            }
        }

        delegate: Rectangle {
            id: rowItem
            required property var modelData
            required property int index
            width: listView.width
            height: 30
            color: mouseArea.containsMouse ? Theme.hover : "transparent"

            Rectangle {
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                height: 1
                color: Theme.border
                opacity: 0.5
            }

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: Theme.spacingMd
                anchors.rightMargin: Theme.spacingMd
                spacing: Theme.spacingSm

                Label {
                    Layout.preferredWidth: 56
                    text: root.formatTime(rowItem.modelData.checkTime)
                    font.pixelSize: Theme.fontSm
                    font.family: Theme.fontFamily
                    color: Theme.textMuted
                }

                Rectangle {
                    Layout.preferredWidth: 1
                    implicitHeight: 14
                    color: Theme.border
                }

                Label {
                    Layout.minimumWidth: 56
                    Layout.maximumWidth: 100
                    text: rowItem.modelData.personName || "—"
                    font.pixelSize: Theme.fontSm
                    font.family: Theme.fontFamily
                    color: Theme.text
                }

                Label {
                    Layout.minimumWidth: 72
                    Layout.maximumWidth: 120
                    text: rowItem.modelData.employeeId || "—"
                    font.pixelSize: Theme.fontSm
                    font.family: Theme.fontFamily
                    color: Theme.textMuted
                }

                Rectangle {
                    Layout.preferredWidth: 1
                    implicitHeight: 14
                    color: Theme.border
                }

                Item { Layout.fillWidth: true }

                RowLayout {
                    spacing: 4

                    Rectangle {
                        implicitWidth: 8
                        implicitHeight: 8
                        radius: 4
                        color: root.statusDotColor(rowItem.modelData.status)
                    }

                    Label {
                        text: rowItem.modelData.statusLabel
                              || Theme.formatAttendanceStatus(rowItem.modelData.status)
                        font.pixelSize: Theme.fontSm
                        font.family: Theme.fontFamily
                        color: Theme.text
                    }
                }
            }

            MouseArea {
                id: mouseArea
                anchors.fill: parent
                hoverEnabled: true
            }
        }

        Label {
            anchors.centerIn: parent
            visible: listView.count === 0
            text: root.emptyText
            font.pixelSize: Theme.fontMd
            font.family: Theme.fontFamily
            color: Theme.textSubtle
        }
    }
}
