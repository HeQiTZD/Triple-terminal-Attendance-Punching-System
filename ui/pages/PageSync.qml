import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceServer

Item {
    id: page

    property var clients: []

    function refreshClients() {
        clients = tcpServer.connectedClients()
    }

    Component.onCompleted: refreshClients()

    Connections {
        target: tcpServer
        function onClientsChanged() { page.refreshClients() }
        function onSyncRequested(deviceId) {
            Logger.info("收到设备同步请求：" + deviceId)
            History.record({
                direction: "IN",
                target: deviceId,
                payload: "{ \"type\": \"sync_request\" }",
                ok: true,
                result: "RECV",
                category: "sync"
            })
        }
        function onSyncAckReceived(deviceId, ack) {
            Logger.info("收到同步ACK：" + deviceId)
            History.record({
                direction: "IN",
                target: deviceId,
                payload: ack,
                ok: true,
                result: "ACK",
                category: "sync"
            })
        }
    }

    function _record(target, args, ok) {
        History.record({
            direction: "OUT",
            target: targetDevice.text,
            payload: args,
            ok: ok,
            result: ok ? "OK" : "FAIL",
            category: "sync"
        })
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingLg
        spacing: Theme.spacingMd

        ToolBarRow {
            Layout.fillWidth: true
            title: qsTr("人员同步")
            subtitle: qsTr("向考勤客户端下发人员名单/人脸库 · 监听 sync_request 事件")
        }

        SplitView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            orientation: Qt.Horizontal

            Card {
                SplitView.preferredWidth: 380
                SplitView.minimumWidth: 280
                stretchContent: true
                title: qsTr("在线设备")
                subtitle: qsTr("点击行选择目标")

                DataTable {
                    anchors.fill: parent
                    rows: page.clients
                    columns: [
                        { key: "deviceId", title: qsTr("设备 ID"), width: 130 },
                        { key: "ipAddress", title: qsTr("IP"), width: 130 },
                        { key: "authenticated", title: qsTr("认证"),
                          formatter: function(v) { return v ? qsTr("是") : qsTr("否") } }
                    ]
                    emptyText: qsTr("暂无在线客户端")
                    onRowClicked: function(idx, row) { targetDevice.text = row.deviceId }
                }
            }

            Card {
                SplitView.fillWidth: true
                stretchContent: true
                title: qsTr("下发同步")

                ColumnLayout {
                    width: parent.width
                    spacing: Theme.spacingMd

                    LabeledField {
                        Layout.fillWidth: true
                        label: qsTr("目标设备 ID")

                        TextField {
                            id: targetDevice
                            text: Presets.defaultDeviceId
                            Layout.fillWidth: true
                        }

                        Button {
                            text: qsTr("通过 TestApi 下发")
                            highlighted: true
                            onClicked: {
                                testApi.requestPersonSync(targetDevice.text)
                                const ok = testApi.lastError.length === 0
                                Logger.logResult("下发人员同步(TestApi)", ok, "设备=" + targetDevice.text)
                                page._record({ via: "testApi.requestPersonSync", deviceId: targetDevice.text }, ok)
                            }
                        }
                        Button {
                            text: qsTr("直接调用 SyncManager")
                            onClicked: {
                                syncManager.sendPersonSyncNow(targetDevice.text)
                                Logger.logResult("下发人员同步(SyncManager)", true, "设备=" + targetDevice.text)
                                page._record({ via: "syncManager.sendPersonSyncNow", deviceId: targetDevice.text }, true)
                            }
                        }
                        Button {
                            text: qsTr("下发人脸全量(SyncManager)")
                            onClicked: {
                                syncManager.sendFaceSyncNow(targetDevice.text)
                                Logger.logResult("下发人脸全量(SyncManager)", true, "设备=" + targetDevice.text)
                                page._record({ via: "syncManager.sendFaceSyncNow", deviceId: targetDevice.text }, true)
                            }
                        }
                    }

                    Card {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        stretchContent: true
                        title: qsTr("说明")
                        ColumnLayout {
                            width: parent.width
                            spacing: Theme.spacingSm
                            Label {
                                text: qsTr("• 同步操作会通过 TCP 把全部人员名单按协议下发给目标设备")
                                color: Theme.textMuted
                                wrapMode: Text.WordWrap
                                Layout.fillWidth: true
                            }
                            Label {
                                text: qsTr("• 人脸同步会先发送 JSON 头(face_sync_item_header)，随后发送二进制帧(4字节长度+payload)")
                                color: Theme.textMuted
                                wrapMode: Text.WordWrap
                                Layout.fillWidth: true
                            }
                            Label {
                                text: qsTr("• 当客户端发送 sync_request 时，左下角日志会自动出现接收记录")
                                color: Theme.textMuted
                                wrapMode: Text.WordWrap
                                Layout.fillWidth: true
                            }
                            Label {
                                text: qsTr("• 若目标设备不在线，发送会失败，可在历史页面查看详细原因")
                                color: Theme.textMuted
                                wrapMode: Text.WordWrap
                                Layout.fillWidth: true
                            }
                        }
                    }
                }
            }
        }
    }
}
