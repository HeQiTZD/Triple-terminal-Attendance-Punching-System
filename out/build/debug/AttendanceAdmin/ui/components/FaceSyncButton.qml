import QtQuick
import QtQuick.Controls
import AttendanceAdmin

/**
 * 人脸数据同步按钮组件
 * 功能：向选中的设备发送 resync 命令，触发人脸数据同步
 */
Item {
    id: root
    implicitHeight: syncBtn.implicitHeight
    implicitWidth: syncBtn.implicitWidth

    required property var deviceServer
    required property var sessionManager
    required property var selectedDevices
    property var deniedDialog: null

    // 同步状态
    property bool syncing: false
    property int successCount: 0
    property int failCount: 0

    signal syncCompleted(int success, int fail)
    signal syncFailed(int code, string message)

    // 按钮定义
    PermissionButton {
        id: syncBtn
        sessionManager: root.sessionManager
        requiredPermission: "device.command"
        deniedDialog: root.deniedDialog
        text: qsTr("同步人脸数据")

        // 正在同步或无选中设备时禁用
        enabled: !root.syncing && root.selectedDevices.length > 0

        onClicked: guardedClick(root._startSync)
    }

    // 同步逻辑
    function _startSync() {
        if (root.selectedDevices.length === 0) {
            root.syncFailed(-1, qsTr("请先选择目标设备"))
            return
        }

        root.syncing = true
        root.successCount = 0
        root.failCount = 0

        // 逐个设备执行同步
        _syncNext(0)
    }

    /**
     * 递归同步下一个设备
     * @param {number} index - 当前设备索引
     */
    function _syncNext(index) {
        if (index >= root.selectedDevices.length) {
            // 全部完成
            root.syncing = false
            root.syncCompleted(root.successCount, root.failCount)
            return
        }

        const deviceId = root.selectedDevices[index]

        // 调用 DeviceServer 发送 resync 命令
        deviceServer.sendCommand(deviceId, "resync", "{}")

        // 监听执行结果
        function onSuccess(apiType, message) {
            if (apiType === "device.command") {
                root.successCount++
                deviceServer.operationSucceeded.disconnect(onSuccess)
                deviceServer.operationFailed.disconnect(onFailed)
                _syncNext(index + 1)
            }
        }
        function onFailed(apiType, code, message) {
            if (apiType === "device.command") {
                root.failCount++
                deviceServer.operationSucceeded.disconnect(onSuccess)
                deviceServer.operationFailed.disconnect(onFailed)
                _syncNext(index + 1)
            }
        }
        deviceServer.operationSucceeded.connect(onSuccess)
        deviceServer.operationFailed.connect(onFailed)
    }
}
