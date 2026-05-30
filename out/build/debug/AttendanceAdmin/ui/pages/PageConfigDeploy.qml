import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceAdmin

Item {
    id: page

    required property var configDeployServer
    required property var deviceServer
    required property var sessionManager
    property var deniedDialog: null
    property var selectedDevices: []

    signal serviceResult(string apiType, int code, string message)

    function _queryDevices() {
        deviceServer.queryDevices("", "", "")
    }

    function _isSelected(deviceId) {
        return selectedDevices.indexOf(deviceId) >= 0
    }

    function _setSelected(deviceId, selected) {
        if (!deviceId)
            return
        const next = selectedDevices.slice()
        const idx = next.indexOf(deviceId)
        if (selected && idx < 0)
            next.push(deviceId)
        if (!selected && idx >= 0)
            next.splice(idx, 1)
        selectedDevices = next
    }

    function _selectOnlineDevices() {
        const next = []
        const rows = deviceServer.records || []
        for (let i = 0; i < rows.length; ++i) {
            const row = rows[i]
            if ((row.status || "").toLowerCase() === "online" && row.deviceId)
                next.push(row.deviceId)
        }
        selectedDevices = next
    }

    function _clearSelectedDevices() {
        selectedDevices = []
    }

    function _line(key, value) {
        return key + "=" + String(value === undefined || value === null ? "" : value)
    }

    function _boolText(value) {
        return value ? "true" : "false"
    }

    function _buildIni() {
        const lines = []
        lines.push("[FaceRecognition]")
        lines.push(_line("FaceThreshold", faceThreshold.value))
        lines.push(_line("MaxFaceCount", maxFaceCount.value))
        lines.push(_line("RecognizeTimeout", recognizeTimeout.value))
        lines.push(_line("AppId", appId.text.trim()))
        lines.push(_line("SdkKey", sdkKey.text.trim()))
        lines.push("")
        lines.push("[Attendance]")
        lines.push(_line("WorkStartTime", workStart.text.trim()))
        lines.push(_line("WorkEndTime", workEnd.text.trim()))
        lines.push(_line("CheckInStartOffset", checkInOffset.value))
        lines.push(_line("LateAllowance", lateAllowance.value))
        lines.push(_line("EarlyLeaveAllowance", earlyLeaveAllowance.value))
        lines.push(_line("CheckOutEndOffset", checkOutOffset.value))
        lines.push(_line("FlexibleWorkEnabled", _boolText(flexibleEnabled.checked)))
        lines.push(_line("FlexibleRange", flexibleRange.value))
        lines.push(_line("CoreWorkStart", coreStart.text.trim()))
        lines.push(_line("CoreWorkEnd", coreEnd.text.trim()))
        lines.push(_line("LunchBreakEnabled", _boolText(lunchEnabled.checked)))
        lines.push(_line("LunchBreakStart", lunchStart.text.trim()))
        lines.push(_line("LunchBreakEnd", lunchEnd.text.trim()))
        lines.push(_line("MinCheckInterval", minCheckInterval.value))
        lines.push(_line("AllowCrossDay", _boolText(allowCrossDay.checked)))
        lines.push(_line("MaxWorkHours", maxWorkHours.value))
        lines.push(_line("MaxRetryCount", maxRetryCount.value))
        lines.push(_line("RetryBackoffBaseMs", retryBackoff.value))
        lines.push("")
        lines.push("[Device]")
        lines.push(_line("DeviceId", deviceIdField.text.trim()))
        lines.push(_line("DeviceKey", deviceKey.text.trim()))
        lines.push(_line("FwVersion", fwVersion.text.trim()))
        lines.push(_line("DeviceName", deviceName.text.trim()))
        return lines.join("\n") + "\n"
    }

    function _validTime(value) {
        return /^([01]\d|2[0-3]):[0-5]\d$/.test(String(value || ""))
    }

    function _validate() {
        if (selectedDevices.length <= 0)
            return qsTr("至少选择一台目标设备")
        if (!_validTime(workStart.text.trim()) || !_validTime(workEnd.text.trim())
                || !_validTime(coreStart.text.trim()) || !_validTime(coreEnd.text.trim())
                || !_validTime(lunchStart.text.trim()) || !_validTime(lunchEnd.text.trim()))
            return qsTr("时间字段必须为 HH:mm，且范围为 00:00-23:59")
        if (fwVersion.text.trim().length === 0)
            return qsTr("固件版本不能为空")
        return ""
    }

    function _deploy() {
        const error = _validate()
        if (error.length > 0) {
            serviceResult("config.deploy", -1, error)
            Logger.warn(error)
            return
        }
        configDeployServer.deployConfig(selectedDevices, _buildIni(), "", "")
    }

    Component.onCompleted: {
        if (PermissionCatalog.hasPerm(sessionManager, "device.read"))
            _queryDevices()
    }

    Connections {
        target: configDeployServer
        function onOperationSucceeded(apiType, message) {
            page.serviceResult(apiType, 0, message)
        }
        function onOperationFailed(apiType, code, message) {
            page.serviceResult(apiType, code, message)
        }
    }

    Connections {
        target: deviceServer
        function onOperationFailed(apiType, code, message) {
            if (apiType === "device.query")
                page.serviceResult(apiType, code, message)
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingLg
        spacing: Theme.spacingMd

        ToolBarRow {
            Layout.fillWidth: true
            title: qsTr("配置下发")
            actions: [
                PermissionButton {
                    sessionManager: page.sessionManager
                    requiredPermission: "device.read"
                    deniedDialog: page.deniedDialog
                    text: qsTr("刷新设备")
                    enabled: !deviceServer.busy
                    onClicked: guardedClick(page._queryDevices)
                },
                PermissionButton {
                    sessionManager: page.sessionManager
                    requiredPermission: "config.deploy"
                    deniedDialog: page.deniedDialog
                    text: qsTr("下发配置")
                    highlighted: true
                    enabled: !configDeployServer.busy && page.selectedDevices.length > 0
                    onClicked: guardedClick(page._deploy)
                }
            ]
        }

        ScrollView {
            id: scroll
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            ColumnLayout {
                width: Math.max(900, scroll.availableWidth)
                spacing: Theme.spacingMd

                RowLayout {
                    Layout.fillWidth: true
                    spacing: Theme.spacingMd

                    Card {
                        Layout.fillWidth: true
                        title: qsTr("人脸识别配置")
                        GridLayout {
                            width: parent.width
                            columns: 2
                            rowSpacing: Theme.spacingSm
                            columnSpacing: Theme.spacingMd

                            LabeledField { label: qsTr("阈值")
                                SpinBox { id: faceThreshold; from: 0; to: 100; value: 80; editable: true; Layout.fillWidth: true }
                            }
                            LabeledField { label: qsTr("最大人脸数")
                                SpinBox { id: maxFaceCount; from: 1; to: 100; value: 5; editable: true; Layout.fillWidth: true }
                            }
                            LabeledField { label: qsTr("识别超时")
                                SpinBox { id: recognizeTimeout; from: 1; to: 3600; value: 10; editable: true; Layout.fillWidth: true }
                            }
                            LabeledField { label: "AppId"
                                TextField { id: appId; Layout.fillWidth: true }
                            }
                            LabeledField { label: "SdkKey"
                                TextField { id: sdkKey; echoMode: TextInput.Password; Layout.fillWidth: true }
                            }
                        }
                    }

                    Card {
                        Layout.fillWidth: true
                        title: qsTr("设备信息")
                        GridLayout {
                            width: parent.width
                            columns: 2
                            rowSpacing: Theme.spacingSm
                            columnSpacing: Theme.spacingMd

                            LabeledField { label: qsTr("设备 ID")
                                TextField { id: deviceIdField; placeholderText: qsTr("可留空或写默认设备"); Layout.fillWidth: true }
                            }
                            LabeledField { label: qsTr("设备密钥")
                                TextField { id: deviceKey; echoMode: TextInput.Password; Layout.fillWidth: true }
                            }
                            LabeledField { label: qsTr("固件版本")
                                TextField { id: fwVersion; text: "1.0.0"; Layout.fillWidth: true }
                            }
                            LabeledField { label: qsTr("设备名称")
                                TextField { id: deviceName; Layout.fillWidth: true }
                            }
                        }
                    }
                }

                Card {
                    Layout.fillWidth: true
                    title: qsTr("考勤规则配置")
                    GridLayout {
                        width: parent.width
                        columns: 4
                        rowSpacing: Theme.spacingSm
                        columnSpacing: Theme.spacingMd

                        LabeledField { label: qsTr("上班时间")
                            TextField { id: workStart; text: "09:00"; Layout.fillWidth: true }
                        }
                        LabeledField { label: qsTr("下班时间")
                            TextField { id: workEnd; text: "18:00"; Layout.fillWidth: true }
                        }
                        LabeledField { label: qsTr("签到偏移")
                            SpinBox { id: checkInOffset; from: -1440; to: 1440; value: 120; editable: true; Layout.fillWidth: true }
                        }
                        LabeledField { label: qsTr("签退结束")
                            SpinBox { id: checkOutOffset; from: 0; to: 86400; value: 180; editable: true; Layout.fillWidth: true }
                        }
                        LabeledField { label: qsTr("迟到容差")
                            SpinBox { id: lateAllowance; from: 0; to: 86400; value: 15; editable: true; Layout.fillWidth: true }
                        }
                        LabeledField { label: qsTr("早退容差")
                            SpinBox { id: earlyLeaveAllowance; from: 0; to: 86400; value: 15; editable: true; Layout.fillWidth: true }
                        }
                        LabeledField { label: qsTr("弹性范围")
                            SpinBox { id: flexibleRange; from: 0; to: 86400; value: 30; editable: true; Layout.fillWidth: true }
                        }
                        LabeledField { label: qsTr("最小间隔")
                            SpinBox { id: minCheckInterval; from: 0; to: 86400; value: 60; editable: true; Layout.fillWidth: true }
                        }
                        LabeledField { label: qsTr("核心开始")
                            TextField { id: coreStart; text: "10:00"; Layout.fillWidth: true }
                        }
                        LabeledField { label: qsTr("核心结束")
                            TextField { id: coreEnd; text: "16:00"; Layout.fillWidth: true }
                        }
                        LabeledField { label: qsTr("午休开始")
                            TextField { id: lunchStart; text: "12:00"; Layout.fillWidth: true }
                        }
                        LabeledField { label: qsTr("午休结束")
                            TextField { id: lunchEnd; text: "13:00"; Layout.fillWidth: true }
                        }
                        LabeledField { label: qsTr("最大工时")
                            SpinBox { id: maxWorkHours; from: 1; to: 24; value: 12; editable: true; Layout.fillWidth: true }
                        }
                        LabeledField { label: qsTr("重试次数")
                            SpinBox { id: maxRetryCount; from: 0; to: 86400; value: 5; editable: true; Layout.fillWidth: true }
                        }
                        LabeledField { label: qsTr("退避毫秒")
                            SpinBox { id: retryBackoff; from: 0; to: 86400; value: 2000; editable: true; Layout.fillWidth: true }
                        }
                        RowLayout {
                            Layout.fillWidth: true
                            Switch { id: flexibleEnabled; text: qsTr("弹性工作"); checked: false }
                            Switch { id: lunchEnabled; text: qsTr("午休"); checked: true }
                            Switch { id: allowCrossDay; text: qsTr("跨天"); checked: false }
                        }
                    }
                }

                Card {
                    Layout.fillWidth: true
                    Layout.preferredWidth: 520
                    title: qsTr("目标设备")
                    headerRight: [
                        FaceSyncButton {
                            id: faceSyncBtn
                            deviceServer: page.deviceServer
                            sessionManager: page.sessionManager
                            selectedDevices: page.selectedDevices
                            deniedDialog: page.deniedDialog

                            onSyncCompleted: function(success, fail) {
                                const msg = qsTr("人脸同步完成：成功 %1 台，失败 %2 台").arg(success).arg(fail)
                                page.serviceResult("face.sync", fail > 0 ? -1 : 0, msg)
                            }

                            onSyncFailed: function(code, message) {
                                page.serviceResult("face.sync", code, message)
                            }
                        },
                        Button { text: qsTr("全选在线"); onClicked: page._selectOnlineDevices() },
                        Button { text: qsTr("清空"); onClicked: page._clearSelectedDevices() }
                    ]

                    ColumnLayout {
                        width: parent.width
                        spacing: Theme.spacingSm

                        Label {
                            text: qsTr("已选择 %1 台").arg(page.selectedDevices.length)
                            color: Theme.textMuted
                            font.pixelSize: Theme.fontSm
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 210
                            color: Theme.bg
                            border.color: Theme.border
                            radius: Theme.radiusSm
                            clip: true

                            ListView {
                                anchors.fill: parent
                                model: deviceServer.records
                                clip: true
                                ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }
                                delegate: CheckBox {
                                    required property var modelData
                                    width: ListView.view.width
                                    text: (modelData.deviceId || "") + "  " + (modelData.deviceName || "") + "  [" + (modelData.status || "") + "]"
                                    checked: page._isSelected(modelData.deviceId || "")
                                    onToggled: page._setSelected(modelData.deviceId || "", checked)
                                }
                            }
                        }
                    }
                }

                Card {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 220
                    stretchContent: true
                    title: qsTr("下发结果")

                    DataTable {
                        anchors.fill: parent
                        rows: configDeployServer.deployTargets
                        emptyText: qsTr("尚未下发")
                        columns: [
                            { key: "deviceId", title: qsTr("设备 ID"), width: 180 },
                            { key: "status", title: qsTr("状态"), width: 120 }
                        ]
                    }
                }
            }
        }
    }

    BusyOverlay { busy: configDeployServer.busy || deviceServer.busy }
}
