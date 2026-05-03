import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceServer

Item {
    id: page

    property var rows: []

    function refresh() {
        rows = dataService.getAllDevices()
        Logger.logResult("加载设备列表", true, "数量=" + rows.length)
    }

    Component.onCompleted: refresh()

    Connections {
        target: dataService
        function onConnectionStateChanged() { if (dataService.isConnected) page.refresh() }
    }
    Connections {
        target: dataManager
        function onDeviceStatusChanged(deviceId, status) { page.refresh() }
    }

    function _record(target, args, ok) {
        History.record({
            direction: "INVOKE",
            target: target,
            payload: args,
            ok: ok,
            result: ok ? "OK" : "FAIL",
            category: "device"
        })
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingLg
        spacing: Theme.spacingMd

        ToolBarRow {
            Layout.fillWidth: true
            title: qsTr("设备管理")
            subtitle: qsTr("注册 / 修改设备 · 状态切换 · 多条件筛选")
            actions: [
                Button { text: qsTr("刷新"); onClicked: page.refresh() }
            ]
        }

        Card {
            Layout.fillWidth: true
            title: qsTr("设备信息")

            ColumnLayout {
                width: parent.width
                spacing: Theme.spacingSm

                GridLayout {
                    Layout.fillWidth: true
                    columns: 4
                    rowSpacing: Theme.spacingSm
                    columnSpacing: Theme.spacingMd

                    LabeledField { label: qsTr("设备 ID"); Layout.fillWidth: true
                        TextField { id: dId; text: Presets.defaultDeviceId; Layout.fillWidth: true }
                    }
                    LabeledField { label: qsTr("名称"); Layout.fillWidth: true
                        TextField { id: dName; placeholderText: qsTr("一号考勤机"); Layout.fillWidth: true }
                    }
                    LabeledField { label: qsTr("IP"); Layout.fillWidth: true
                        TextField { id: dIp; placeholderText: qsTr("127.0.0.1"); Layout.fillWidth: true }
                    }
                    LabeledField { label: qsTr("状态"); Layout.fillWidth: true
                        ComboBox {
                            id: dStatus
                            model: ["online", "offline", "maintenance"]
                            Layout.fillWidth: true
                        }
                    }
                }

                Row {
                    spacing: Theme.spacingSm
                    Button {
                        text: qsTr("新增 / 更新设备")
                        highlighted: true
                        onClicked: {
                            const ok = dataService.addOrUpdateDevice(dId.text, dName.text, dIp.text, dStatus.currentText)
                            Logger.logResult("新增/更新设备", ok, "设备=" + dId.text)
                            page._record("dataService.addOrUpdateDevice",
                                { deviceId: dId.text, deviceName: dName.text, ipAddress: dIp.text, status: dStatus.currentText }, ok)
                            if (ok) page.refresh()
                        }
                    }
                    Button {
                        text: qsTr("仅更新状态")
                        onClicked: {
                            const ok = dataService.updateDeviceStatus(dId.text, dStatus.currentText)
                            Logger.logResult("更新设备状态", ok, "设备=" + dId.text + " 状态=" + dStatus.currentText)
                            page._record("dataService.updateDeviceStatus",
                                { deviceId: dId.text, status: dStatus.currentText }, ok)
                            if (ok) page.refresh()
                        }
                    }
                    Button {
                        text: qsTr("按 ID 查询")
                        onClicked: {
                            const obj = dataService.getDeviceById(dId.text)
                            const ok = obj !== null
                            Logger.logResult("查询设备", ok, ok ? ("name=" + obj.deviceName) : "未命中")
                            page._record("dataService.getDeviceById", { deviceId: dId.text }, ok)
                            if (ok) {
                                dName.text = obj.deviceName; dIp.text = obj.ipAddress
                                dStatus.currentIndex = Math.max(0, dStatus.indexOfValue(obj.status))
                            }
                        }
                    }
                    Button {
                        text: qsTr("条件筛选")
                        onClicked: {
                            const stFilter = filterStatus.currentIndex === 0 ? "" : filterStatus.currentText
                            const list = dataService.selectDevice(dId.text.trim(), dName.text.trim(), dIp.text.trim(), stFilter)
                            page.rows = list
                            Logger.logResult("条件查询设备", true, "数量=" + list.length)
                            page._record("dataService.selectDevice",
                                { deviceId: dId.text, deviceName: dName.text, ipAddress: dIp.text, status: stFilter }, true)
                        }
                    }
                    LabeledField {
                        label: qsTr("状态筛选")
                        labelWidth: 60
                        ComboBox {
                            id: filterStatus
                            model: [qsTr("全部"), "online", "offline", "maintenance"]
                            Layout.preferredWidth: 130
                        }
                    }
                }
            }
        }

        Card {
            Layout.fillWidth: true
            Layout.fillHeight: true
            stretchContent: true
            title: qsTr("设备列表")

            DataTable {
                id: table
                anchors.fill: parent
                rows: page.rows
                columns: [
                    { key: "id", title: "ID", width: 60, align: "right" },
                    { key: "deviceId", title: qsTr("设备 ID"), width: 130 },
                    { key: "deviceName", title: qsTr("名称"), width: 160 },
                    { key: "ipAddress", title: qsTr("IP"), width: 130 },
                    { key: "status", title: qsTr("状态"), width: 100 },
                    { key: "lastOnline", title: qsTr("最近在线"),
                      formatter: function(v) { return v && !isNaN(v.getTime()) ? Qt.formatDateTime(v, "yyyy-MM-dd HH:mm:ss") : "—" } }
                ]
                onRowClicked: function(idx, row) {
                    dId.text = row.deviceId
                    dName.text = row.deviceName
                    dIp.text = row.ipAddress
                    dStatus.currentIndex = Math.max(0, dStatus.indexOfValue(row.status))
                }
            }
        }
    }
}
