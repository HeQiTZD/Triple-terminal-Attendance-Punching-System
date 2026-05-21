import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceAdmin

Item {
    id: page

    required property var eventService
    required property var sessionManager
    property var deniedDialog: null
    signal serviceResult(string apiType, int code, string message)

    readonly property var topicOptions: ["attendance"]

    function _selectedTopics() {
        const out = []
        for (let i = 0; i < topicRepeater.count; ++i) {
            const item = topicRepeater.itemAt(i)
            if (item && item.checked)
                out.push(page.topicOptions[i])
        }
        return out
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingLg
        spacing: Theme.spacingMd

        ToolBarRow {
            Layout.fillWidth: true
            title: qsTr("事件中心")
            subtitle: qsTr("订阅服务端推送 · 已订阅：")
                      + (eventService ? eventService.subscribedTopics.join(", ") : "")
        }

        Card {
            Layout.fillWidth: true
            title: qsTr("主题订阅")

            ColumnLayout {
                width: parent.width
                spacing: Theme.spacingSm

                ColumnLayout {
                    spacing: Theme.spacingXs

                    Repeater {
                        id: topicRepeater
                        model: page.topicOptions
                        delegate: CheckBox {
                            required property string modelData
                            text: modelData
                            checked: eventService.subscribedTopics.indexOf(modelData) >= 0
                        }
                    }
                }

                Row {
                    spacing: Theme.spacingSm
                    PermissionButton {
                        sessionManager: page.sessionManager
                        requiredPermission: "event.subscribe"
                        deniedDialog: page.deniedDialog
                        text: qsTr("订阅")
                        highlighted: true
                        enabled: !eventService.busy
                        onClicked: guardedClick(function() {
                            const topics = page._selectedTopics()
                            if (!topics.length) {
                                Logger.warn(qsTr("请至少选择一个主题"))
                                return
                            }
                            eventService.subscribe(topics)
                        })
                    }
                    PermissionButton {
                        sessionManager: page.sessionManager
                        requiredPermission: "event.subscribe"
                        deniedDialog: page.deniedDialog
                        text: qsTr("取消订阅")
                        enabled: !eventService.busy
                        onClicked: guardedClick(function() {
                            const topics = page._selectedTopics()
                            if (!topics.length)
                                eventService.unsubscribe(eventService.subscribedTopics)
                            else
                                eventService.unsubscribe(topics)
                        })
                    }
                }
            }
        }

        Card {
            Layout.fillWidth: true
            Layout.fillHeight: true
            stretchContent: true
            title: qsTr("推送流水")
            subtitle: qsTr("今日 ") + PushFeed.todayCount + qsTr(" 条")

            ListView {
                anchors.fill: parent
                clip: true
                model: PushFeed.model
                delegate: Item {
                    required property int index
                    width: ListView.view.width
                    height: rowLbl.implicitHeight + 8

                    readonly property var entry: PushFeed.model.get(index)

                    Label {
                        id: rowLbl
                        width: parent.width - Theme.spacingMd * 2
                        x: Theme.spacingMd
                        anchors.verticalCenter: parent.verticalCenter
                        text: "[" + entry.time + "] " + entry.type + " — " + entry.summary
                        color: Theme.text
                        font.pixelSize: Theme.fontXs
                        font.family: Theme.fontMono
                        wrapMode: Text.WordWrap
                    }
                }
            }
        }
    }

    BusyOverlay { busy: eventService.busy }

    Connections {
        target: eventService
        function onOperationSucceeded(apiType, message) {
            page.serviceResult(apiType, 0, message)
        }
        function onOperationFailed(apiType, code, message) {
            page.serviceResult(apiType, code, message)
        }
    }
}
