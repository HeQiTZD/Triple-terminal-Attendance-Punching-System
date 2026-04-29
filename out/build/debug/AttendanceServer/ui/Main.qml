pragma ComponentBehavior: Bound

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    id: win
    width: 1360
    height: 860
    visible: true
    title: qsTr("AttendanceServer - QML 测试工作台")

    // 主题锁定：避免跟随系统浅色/深色主题切换
    // 可选值： "light" / "dark"
    property string lockedTheme: "light"
    readonly property bool isDarkTheme: lockedTheme === "dark"

    // 不实例化 `Palette {}`，直接设置 window 自带 palette 字段，避免启动时报 QML 类型错误
    palette.window: win.isDarkTheme ? "#121212" : "#FFFFFF"
    palette.windowText: win.isDarkTheme ? "#EDEDED" : "#1F2328"
    palette.base: win.isDarkTheme ? "#1E1E1E" : "#FFFFFF"
    palette.alternateBase: win.isDarkTheme ? "#232323" : "#F6F8FA"
    palette.text: win.isDarkTheme ? "#EDEDED" : "#1F2328"
    palette.button: win.isDarkTheme ? "#2B2B2B" : "#F3F4F6"
    palette.buttonText: win.isDarkTheme ? "#EDEDED" : "#1F2328"
    palette.highlight: "#2D7FF9"
    palette.highlightedText: "#FFFFFF"
    palette.toolTipBase: win.isDarkTheme ? "#2B2B2B" : "#FFFFFF"
    palette.toolTipText: win.isDarkTheme ? "#EDEDED" : "#1F2328"

    property string lastAction: ""
    property string lastError: ""
    property bool faceEngineOk: false
    property bool feature1Ok: false
    property bool feature2Ok: false
    property real similarityValue: -1
    property var personModel: []
    property var deviceModel: []
    property var attendanceModel: []
    property var faceModel: []
    property var selectedPerson: null
    property var selectedDevice: null
    property var selectedFaceData: null

    function appendLog(message) {
        const ts = Qt.formatDateTime(new Date(), "HH:mm:ss")
        lastAction = message
        console.log("[UI]", message)
        if (!logArea)
            return
        logArea.text += "[" + ts + "] " + message + "\n"
        logArea.cursorPosition = logArea.text.length
    }

    function rememberError(message) {
        if (!message || message.length === 0)
            return
        lastError = message
        appendLog("错误/提示: " + message)
    }

    function logResult(name, ok, detail) {
        const text = "操作=" + name + " | 结果=" + (ok ? "成功" : "失败")
                   + ((detail && detail.length) ? (" | " + detail) : "")
        appendLog(text)
    }

    function parseIntOrZero(text) {
        const n = parseInt(text)
        return isNaN(n) ? 0 : n
    }

    function parseDateTime(text) {
        const d = Date.fromLocaleString(Qt.locale(), text, "yyyy-MM-dd HH:mm:ss")
        return isNaN(d.getTime()) ? new Date() : d
    }

    function parseDate(text) {
        const d = Date.fromLocaleString(Qt.locale(), text, "yyyy-MM-dd")
        return isNaN(d.getTime()) ? new Date() : d
    }

    function maskSecret(text) {
        if (!text || text.length === 0)
            return ""
        if (text.length <= 4)
            return "****"
        return text.slice(0, 2) + "****" + text.slice(text.length - 2)
    }

    function jsonText(value) {
        try {
            return JSON.stringify(value, null, 2)
        } catch (e) {
            return String(value)
        }
    }

    function describePerson(person) {
        if (!person)
            return "未找到人员"
        return "#" + person.id + " | " + person.employeeId + " | " + person.name
             + " | " + person.department + " | " + person.position
    }

    function describeDevice(device) {
        if (!device)
            return "未找到设备"
        return "#" + device.id + " | " + device.deviceId + " | " + device.deviceName
             + " | " + device.ipAddress + " | " + device.status
    }

    function describeRecord(record) {
        if (!record)
            return "空记录"
        return "#" + record.id + " | personId=" + record.personId
             + " | " + Qt.formatDateTime(record.checkTime, "yyyy-MM-dd HH:mm:ss")
             + " | " + record.deviceId + " | " + record.status
    }

    function describeFaceData(faceData) {
        if (!faceData)
            return "未找到人脸数据"
        return "#" + faceData.id + " | personId=" + faceData.personId
             + " | size=" + faceData.featureSize + " | " + faceData.status
    }

    function refreshPersons() {
        personModel = dataManager.getAllPerson()
        logResult("加载人员列表", true, "数量=" + personModel.length)
    }

    function refreshDevices() {
        deviceModel = dataManager.getAllDevices()
        logResult("加载设备列表", true, "数量=" + deviceModel.length)
    }

    function refreshFaces() {
        faceModel = dataManager.getAllFaceData()
        logResult("加载人脸库", true, "数量=" + faceModel.length)
    }

    Component.onCompleted: appendLog("QML 测试工作台初始化完成")

    Connections {
        target: testApi
        function onLastErrorChanged() {
            rememberError(testApi.lastError)
        }
    }

    Connections {
        target: exportManager
        function onLastErrorChanged() {
            rememberError(exportManager.lastError)
        }
    }

    Connections {
        target: tcpServer
        function onClientConnected(deviceId, ipAddress) { appendLog("客户端连接: " + deviceId + " @ " + ipAddress) }
        function onClientDisconnected(deviceId) { appendLog("客户端断开: " + deviceId) }
        function onAttendanceRecordReceived(record) { appendLog("收到考勤上报: " + jsonText(record)) }
        function onDeviceStatusReceived(deviceId, status) { appendLog("设备状态上报: " + deviceId + " => " + jsonText(status)) }
        function onSyncRequested(deviceId) { appendLog("收到同步请求: " + deviceId) }
        function onErrorOccurred(errorString) { rememberError("TCP: " + errorString) }
        function onIsRunningChanged() { appendLog("TCP 运行状态=" + tcpServer.isRunning) }
        function onClientCountChanged() { appendLog("在线客户端数=" + tcpServer.clientCount) }
    }

    Connections {
        target: dataManager
        function onConnectionStateChanged() { appendLog("数据库连接状态=" + dataManager.isConnected) }
        function onErrorOccurred(errorString) { rememberError("数据库: " + errorString) }
        function onPersonAdded(id) { appendLog("人员已新增: id=" + id) }
        function onPersonUpdated(id) { appendLog("人员已更新: id=" + id) }
        function onPersonDeleted(id) { appendLog("人员已删除: id=" + id) }
        function onAttendanceRecordAdded(id) { appendLog("考勤记录已新增: id=" + id) }
        function onDeviceStatusChanged(deviceId, status) { appendLog("设备状态变更: " + deviceId + " => " + status) }
        function onFaceDataAdded(id, personId) { appendLog("人脸数据已新增: id=" + id + " personId=" + personId) }
        function onFaceDataUpdated(id, personId) { appendLog("人脸数据已更新: id=" + id + " personId=" + personId) }
        function onFaceDataDeleted(id) { appendLog("人脸数据已删除: id=" + id) }
    }

    SplitView {
        anchors.fill: parent
        orientation: Qt.Horizontal

        Rectangle {
            SplitView.fillWidth: true
            SplitView.preferredWidth: 910
            color: "transparent"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 8

                GroupBox {
                    title: qsTr("运行状态")
                    Layout.fillWidth: true

                    GridLayout {
                        anchors.fill: parent
                        anchors.margins: 8
                        columns: 8
                        columnSpacing: 10
                        rowSpacing: 6

                        Label { text: qsTr("数据库") }
                        Label { text: dataManager && dataManager.isConnected ? qsTr("已连接") : qsTr("未连接") }
                        Label { text: qsTr("TCP") }
                        Label { text: tcpServer && tcpServer.isRunning ? qsTr("运行中") : qsTr("已停止") }
                        Label { text: qsTr("客户端") }
                        Label { text: tcpServer ? tcpServer.clientCount : 0 }
                        Label { text: qsTr("最后操作") }
                        Label {
                            Layout.fillWidth: true
                            text: win.lastAction
                            elide: Text.ElideRight
                        }

                        Label { text: qsTr("最后错误") }
                        Label {
                            Layout.columnSpan: 7
                            Layout.fillWidth: true
                            text: win.lastError.length ? win.lastError : qsTr("无")
                            color: win.lastError.length ? "tomato" : palette.text
                            elide: Text.ElideRight
                        }
                    }
                }

                TabBar {
                    id: tabs
                    Layout.fillWidth: true
                    TabButton { text: qsTr("概览") }
                    TabButton { text: qsTr("TCP/协议") }
                    TabButton { text: qsTr("人员") }
                    TabButton { text: qsTr("设备") }
                    TabButton { text: qsTr("考勤") }
                    TabButton { text: qsTr("人脸") }
                    TabButton { text: qsTr("同步/导出") }
                    TabButton { text: qsTr("统计分析") }
                }

                StackLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    currentIndex: tabs.currentIndex

                    ScrollView {
                        clip: true
                        ColumnLayout {
                            width: parent.width
                            spacing: 10

                            GroupBox {
                                title: qsTr("快速操作")
                                Layout.fillWidth: true
                                RowLayout {
                                    anchors.fill: parent
                                    anchors.margins: 8
                                    spacing: 8
                                    Button {
                                        text: qsTr("刷新人员")
                                        onClicked: refreshPersons()
                                    }
                                    Button {
                                        text: qsTr("刷新设备")
                                        onClicked: refreshDevices()
                                    }
                                    Button {
                                        text: qsTr("刷新人脸库")
                                        onClicked: refreshFaces()
                                    }
                                    Button {
                                        text: qsTr("清空错误")
                                        onClicked: {
                                            win.lastError = ""
                                            appendLog("已清空最后错误")
                                        }
                                    }
                                    Item { Layout.fillWidth: true }
                                }
                            }

                            GroupBox {
                                title: qsTr("测试说明")
                                Layout.fillWidth: true
                                Label {
                                    anchors.fill: parent
                                    anchors.margins: 8
                                    wrapMode: Text.WordWrap
                                    text: qsTr("本界面用于手动验证 AttendanceServer 的 TCP 服务、数据库 CRUD、考勤记录、人脸特征、人员同步、CSV 导出和统计分析。需要 MySQL 可连接、ArcFace 授权有效、以及在线考勤客户端时，对应测试才会返回成功。")
                                }
                            }
                        }
                    }

                    ScrollView {
                        clip: true
                        ColumnLayout {
                            width: parent.width
                            spacing: 10

                            GroupBox {
                                title: qsTr("TCP 服务")
                                Layout.fillWidth: true
                                RowLayout {
                                    anchors.fill: parent
                                    anchors.margins: 8
                                    spacing: 8

                                    Label { text: qsTr("端口") }
                                    TextField {
                                        id: tcpPort
                                        text: "8080"
                                        Layout.preferredWidth: 120
                                    }
                                    Button {
                                        text: qsTr("启动")
                                        onClicked: {
                                            const port = parseIntOrZero(tcpPort.text)
                                            const ok = tcpServer.startServer(port)
                                            logResult("启动 TCP 服务", ok, "端口=" + port)
                                        }
                                    }
                                    Button {
                                        text: qsTr("停止")
                                        onClicked: {
                                            tcpServer.stopServer()
                                            logResult("停止 TCP 服务", true, "")
                                        }
                                    }
                                    Label { text: qsTr("当前客户端数: ") + (tcpServer ? tcpServer.clientCount : 0) }
                                    Item { Layout.fillWidth: true }
                                }
                            }

                            GroupBox {
                                title: qsTr("JSON 单发/广播")
                                Layout.fillWidth: true
                                ColumnLayout {
                                    anchors.fill: parent
                                    anchors.margins: 8
                                    spacing: 8

                                    RowLayout {
                                        Label { text: qsTr("设备 ID") }
                                        TextField {
                                            id: tcpTargetDevice
                                            text: "dev001"
                                            Layout.preferredWidth: 180
                                        }
                                        Button {
                                            text: qsTr("发送到设备")
                                            onClicked: {
                                                const ok = testApi.sendToClientJson(tcpTargetDevice.text, tcpJson.text)
                                                logResult("发送 JSON 到设备", ok, "设备=" + tcpTargetDevice.text)
                                            }
                                        }
                                        Button {
                                            text: qsTr("广播")
                                            onClicked: {
                                                const ok = testApi.broadcastJson(tcpJson.text)
                                                logResult("广播 JSON", ok, "长度=" + tcpJson.text.length)
                                            }
                                        }
                                        Item { Layout.fillWidth: true }
                                    }

                                    TextArea {
                                        id: tcpJson
                                        Layout.fillWidth: true
                                        Layout.preferredHeight: 150
                                        wrapMode: TextArea.WrapAnywhere
                                        text: "{\n  \"type\": \"ping\",\n  \"timestamp\": \"manual-test\"\n}"
                                    }
                                }
                            }
                        }
                    }

                    ScrollView {
                        clip: true
                        ColumnLayout {
                            width: parent.width
                            spacing: 10

                            GroupBox {
                                title: qsTr("人员 CRUD")
                                Layout.fillWidth: true
                                ColumnLayout {
                                    anchors.fill: parent
                                    anchors.margins: 8
                                    spacing: 8

                                    GridLayout {
                                        Layout.fillWidth: true
                                        columns: 8
                                        Label { text: qsTr("ID") }
                                        TextField { id: personId; placeholderText: qsTr("查询用"); Layout.preferredWidth: 90 }
                                        Label { text: qsTr("姓名") }
                                        TextField { id: personName; placeholderText: qsTr("张三"); Layout.preferredWidth: 140 }
                                        Label { text: qsTr("工号") }
                                        TextField { id: personEmployeeId; placeholderText: qsTr("EMP001"); Layout.preferredWidth: 140 }
                                        Label { text: qsTr("部门") }
                                        TextField { id: personDepartment; placeholderText: qsTr("研发部"); Layout.preferredWidth: 140 }
                                        Label { text: qsTr("岗位") }
                                        TextField { id: personPosition; placeholderText: qsTr("工程师"); Layout.preferredWidth: 140 }
                                    }

                                    RowLayout {
                                        spacing: 8
                                        Button {
                                            text: qsTr("新增")
                                            onClicked: {
                                                const ok = dataManager.addPerson(personName.text, personEmployeeId.text, personDepartment.text, personPosition.text)
                                                logResult("新增人员", ok, "工号=" + personEmployeeId.text)
                                                if (ok) refreshPersons()
                                            }
                                        }
                                        Button {
                                            text: qsTr("修改")
                                            onClicked: {
                                                const ok = dataManager.updatedPerson(personName.text, personEmployeeId.text, personDepartment.text, personPosition.text)
                                                logResult("修改人员", ok, "工号=" + personEmployeeId.text)
                                                if (ok) refreshPersons()
                                            }
                                        }
                                        Button {
                                            text: qsTr("删除")
                                            onClicked: {
                                                const ok = dataManager.deletePerson(personName.text, personEmployeeId.text)
                                                logResult("删除人员", ok, "姓名=" + personName.text + " 工号=" + personEmployeeId.text)
                                                if (ok) refreshPersons()
                                            }
                                        }
                                        Button {
                                            text: qsTr("按 ID 查询")
                                            onClicked: {
                                                selectedPerson = dataManager.getPersonById(parseIntOrZero(personId.text))
                                                logResult("按 ID 查询人员", selectedPerson !== null, describePerson(selectedPerson))
                                            }
                                        }
                                        Button {
                                            text: qsTr("按工号查询")
                                            onClicked: {
                                                selectedPerson = dataManager.getPersonByEmployeeId(personEmployeeId.text)
                                                logResult("按工号查询人员", selectedPerson !== null, describePerson(selectedPerson))
                                            }
                                        }
                                        Button {
                                            text: qsTr("加载全部")
                                            onClicked: refreshPersons()
                                        }
                                        Item { Layout.fillWidth: true }
                                    }

                                    Label { text: qsTr("查询结果: ") + describePerson(selectedPerson) }
                                }
                            }

                            GroupBox {
                                title: qsTr("人员列表")
                                Layout.fillWidth: true
                                ListView {
                                    Layout.fillWidth: true
                                    Layout.preferredHeight: 260
                                    clip: true
                                    model: personModel
                                    delegate: ItemDelegate {
                                        required property var modelData
                                        width: ListView.view.width
                                        text: describePerson(modelData)
                                        onClicked: {
                                            selectedPerson = modelData
                                            personId.text = String(modelData.id)
                                            personName.text = modelData.name
                                            personEmployeeId.text = modelData.employeeId
                                            personDepartment.text = modelData.department
                                            personPosition.text = modelData.position
                                        }
                                    }
                                }
                            }
                        }
                    }

                    ScrollView {
                        clip: true
                        ColumnLayout {
                            width: parent.width
                            spacing: 10

                            GroupBox {
                                title: qsTr("设备 CRUD")
                                Layout.fillWidth: true
                                ColumnLayout {
                                    anchors.fill: parent
                                    anchors.margins: 8
                                    spacing: 8

                                    GridLayout {
                                        Layout.fillWidth: true
                                        columns: 8
                                        Label { text: qsTr("设备 ID") }
                                        TextField { id: deviceId; text: "dev001"; Layout.preferredWidth: 140 }
                                        Label { text: qsTr("名称") }
                                        TextField { id: deviceName; placeholderText: qsTr("一号考勤机"); Layout.preferredWidth: 160 }
                                        Label { text: qsTr("IP") }
                                        TextField { id: deviceIp; placeholderText: qsTr("127.0.0.1"); Layout.preferredWidth: 160 }
                                        Label { text: qsTr("状态") }
                                        ComboBox {
                                            id: deviceStatus
                                            model: ["online", "offline", "maintenance"]
                                            Layout.preferredWidth: 150
                                        }
                                    }

                                    RowLayout {
                                        spacing: 8
                                        Button {
                                            text: qsTr("新增/更新设备")
                                            onClicked: {
                                                const ok = dataManager.addOrUpdateDevice(deviceId.text, deviceName.text, deviceIp.text, deviceStatus.currentText)
                                                logResult("新增/更新设备", ok, "设备=" + deviceId.text)
                                                if (ok) refreshDevices()
                                            }
                                        }
                                        Button {
                                            text: qsTr("更新状态")
                                            onClicked: {
                                                const ok = dataManager.updateDeviceStatus(deviceId.text, deviceStatus.currentText)
                                                logResult("更新设备状态", ok, "设备=" + deviceId.text + " 状态=" + deviceStatus.currentText)
                                                if (ok) refreshDevices()
                                            }
                                        }
                                        Button {
                                            text: qsTr("按设备 ID 查询")
                                            onClicked: {
                                                selectedDevice = dataManager.getDeviceById(deviceId.text)
                                                logResult("查询设备", selectedDevice !== null, describeDevice(selectedDevice))
                                            }
                                        }
                                        Button {
                                            text: qsTr("加载全部")
                                            onClicked: refreshDevices()
                                        }
                                        Item { Layout.fillWidth: true }
                                    }

                                    Label { text: qsTr("查询结果: ") + describeDevice(selectedDevice) }
                                }
                            }

                            GroupBox {
                                title: qsTr("设备列表")
                                Layout.fillWidth: true
                                ListView {
                                    Layout.fillWidth: true
                                    Layout.preferredHeight: 280
                                    clip: true
                                    model: deviceModel
                                    delegate: ItemDelegate {
                                        required property var modelData
                                        width: ListView.view.width
                                        text: describeDevice(modelData)
                                        onClicked: {
                                            selectedDevice = modelData
                                            deviceId.text = modelData.deviceId
                                            deviceName.text = modelData.deviceName
                                            deviceIp.text = modelData.ipAddress
                                            deviceStatus.currentIndex = Math.max(0, deviceStatus.indexOfValue(modelData.status))
                                        }
                                    }
                                }
                            }
                        }
                    }

                    ScrollView {
                        clip: true
                        ColumnLayout {
                            width: parent.width
                            spacing: 10

                            GroupBox {
                                title: qsTr("考勤记录")
                                Layout.fillWidth: true
                                ColumnLayout {
                                    anchors.fill: parent
                                    anchors.margins: 8
                                    spacing: 8

                                    GridLayout {
                                        Layout.fillWidth: true
                                        columns: 8
                                        Label { text: qsTr("人员 ID") }
                                        TextField { id: attendancePersonId; text: "1"; Layout.preferredWidth: 100 }
                                        Label { text: qsTr("打卡时间") }
                                        TextField { id: attendanceCheckTime; text: Qt.formatDateTime(new Date(), "yyyy-MM-dd HH:mm:ss"); Layout.preferredWidth: 180 }
                                        Label { text: qsTr("设备 ID") }
                                        TextField { id: attendanceDeviceId; text: "dev001"; Layout.preferredWidth: 140 }
                                        Label { text: qsTr("状态") }
                                        ComboBox { id: attendanceStatus; model: ["normal", "late", "early", "manual"]; Layout.preferredWidth: 130 }
                                        Label { text: qsTr("开始") }
                                        TextField { id: attendanceStart; text: "2026-04-01 00:00:00"; Layout.preferredWidth: 180 }
                                        Label { text: qsTr("结束") }
                                        TextField { id: attendanceEnd; text: "2026-04-30 23:59:59"; Layout.preferredWidth: 180 }
                                    }

                                    RowLayout {
                                        spacing: 8
                                        Button {
                                            text: qsTr("新增记录")
                                            onClicked: {
                                                const ok = dataManager.addAttendanceRecore(
                                                    parseIntOrZero(attendancePersonId.text),
                                                    parseDateTime(attendanceCheckTime.text),
                                                    attendanceDeviceId.text,
                                                    attendanceStatus.currentText
                                                )
                                                logResult("新增考勤记录", ok, "personId=" + attendancePersonId.text)
                                            }
                                        }
                                        Button {
                                            text: qsTr("按时间查询")
                                            onClicked: {
                                                attendanceModel = dataManager.getAttendanceRecords(parseDateTime(attendanceStart.text), parseDateTime(attendanceEnd.text))
                                                logResult("查询考勤记录", true, "数量=" + attendanceModel.length)
                                            }
                                        }
                                        Button {
                                            text: qsTr("按人员查询")
                                            onClicked: {
                                                attendanceModel = dataManager.getAttendanceRecordsByPerson(parseIntOrZero(attendancePersonId.text), parseDateTime(attendanceStart.text), parseDateTime(attendanceEnd.text))
                                                logResult("按人员查询考勤", true, "数量=" + attendanceModel.length)
                                            }
                                        }
                                        Item { Layout.fillWidth: true }
                                    }
                                }
                            }

                            GroupBox {
                                title: qsTr("考勤列表")
                                Layout.fillWidth: true
                                ListView {
                                    Layout.fillWidth: true
                                    Layout.preferredHeight: 300
                                    clip: true
                                    model: attendanceModel
                                    delegate: ItemDelegate {
                                        required property var modelData
                                        width: ListView.view.width
                                        text: describeRecord(modelData)
                                    }
                                }
                            }
                        }
                    }

                    ScrollView {
                        clip: true
                        ColumnLayout {
                            width: parent.width
                            spacing: 10

                            GroupBox {
                                title: qsTr("ArcFace 引擎与特征")
                                Layout.fillWidth: true
                                ColumnLayout {
                                    anchors.fill: parent
                                    anchors.margins: 8
                                    spacing: 8

                                    RowLayout {
                                        Label { text: qsTr("AppId") }
                                        TextField { id: faceAppId; Layout.fillWidth: true }
                                        Label { text: qsTr("SdkKey") }
                                        TextField { id: faceSdkKey; Layout.fillWidth: true; echoMode: TextInput.Password }
                                        Button {
                                            text: qsTr("初始化")
                                            onClicked: {
                                                const ok = testApi.initFaceEngine(faceAppId.text, faceSdkKey.text)
                                                faceEngineOk = ok
                                                logResult("初始化人脸引擎", ok, ok ? ("SdkKey=" + maskSecret(faceSdkKey.text)) : testApi.lastError)
                                            }
                                        }
                                        Label { text: faceEngineOk ? qsTr("已初始化") : qsTr("未初始化") }
                                    }

                                    RowLayout {
                                        Label { text: qsTr("图片 1") }
                                        TextField { id: image1Path; Layout.fillWidth: true }
                                        Button {
                                            text: qsTr("选择")
                                            onClicked: {
                                                const path = testApi.selectImageFile()
                                                if (path.length)
                                                    image1Path.text = path
                                                logResult("选择图片 1", path.length > 0, path)
                                            }
                                        }
                                        Button {
                                            text: qsTr("提取特征 1")
                                            onClicked: {
                                                feature1Text.text = testApi.extractFeatureBase64(image1Path.text)
                                                feature1Ok = feature1Text.text.length > 0
                                                logResult("提取特征 1", feature1Ok, feature1Ok ? ("长度=" + feature1Text.text.length) : testApi.lastError)
                                            }
                                        }
                                    }

                                    RowLayout {
                                        Label { text: qsTr("图片 2") }
                                        TextField { id: image2Path; Layout.fillWidth: true }
                                        Button {
                                            text: qsTr("选择")
                                            onClicked: {
                                                const path = testApi.selectImageFile()
                                                if (path.length)
                                                    image2Path.text = path
                                                logResult("选择图片 2", path.length > 0, path)
                                            }
                                        }
                                        Button {
                                            text: qsTr("提取特征 2")
                                            onClicked: {
                                                feature2Text.text = testApi.extractFeatureBase64(image2Path.text)
                                                feature2Ok = feature2Text.text.length > 0
                                                logResult("提取特征 2", feature2Ok, feature2Ok ? ("长度=" + feature2Text.text.length) : testApi.lastError)
                                            }
                                        }
                                    }

                                    RowLayout {
                                        Button {
                                            text: qsTr("比对相似度")
                                            onClicked: {
                                                similarityValue = testApi.compareFeatureBase64(feature1Text.text, feature2Text.text)
                                                logResult("比对相似度", similarityValue >= 0, similarityValue >= 0 ? ("相似度=" + similarityValue) : testApi.lastError)
                                            }
                                        }
                                        Label { text: qsTr("特征1: ") + (feature1Ok ? qsTr("已提取") : qsTr("未提取")) }
                                        Label { text: qsTr("特征2: ") + (feature2Ok ? qsTr("已提取") : qsTr("未提取")) }
                                        Label { text: qsTr("相似度: ") + (similarityValue >= 0 ? String(similarityValue) : qsTr("暂无")) }
                                        Item { Layout.fillWidth: true }
                                    }

                                    TextArea { id: feature1Text; visible: false }
                                    TextArea { id: feature2Text; visible: false }
                                }
                            }

                            GroupBox {
                                title: qsTr("人脸库（按工号）")
                                Layout.fillWidth: true
                                ColumnLayout {
                                    anchors.fill: parent
                                    anchors.margins: 8
                                    spacing: 8

                                    RowLayout {
                                        Label { text: qsTr("工号") }
                                        TextField { id: faceEmployeeId; text: "EMP001"; Layout.preferredWidth: 160 }
                                        Button {
                                            text: qsTr("新增特征1")
                                            onClicked: {
                                                const ok = testApi.addFaceDataBase64(faceEmployeeId.text, feature1Text.text)
                                                logResult("新增人脸特征", ok, "工号=" + faceEmployeeId.text)
                                                if (ok) refreshFaces()
                                            }
                                        }
                                        Button {
                                            text: qsTr("更新特征1")
                                            onClicked: {
                                                const ok = testApi.updateFaceDataBase64(faceEmployeeId.text, feature1Text.text)
                                                logResult("更新人脸特征", ok, "工号=" + faceEmployeeId.text)
                                                if (ok) refreshFaces()
                                            }
                                        }
                                        Button {
                                            text: qsTr("删除")
                                            onClicked: {
                                                const ok = testApi.deleteFaceDataByEmployeeId(faceEmployeeId.text)
                                                logResult("删除人脸特征", ok, "工号=" + faceEmployeeId.text)
                                                if (ok) refreshFaces()
                                            }
                                        }
                                        Button {
                                            text: qsTr("查询")
                                            onClicked: {
                                                selectedFaceData = dataManager.getFaceDataByEmployeeId(faceEmployeeId.text)
                                                logResult("查询人脸特征", selectedFaceData !== null, describeFaceData(selectedFaceData))
                                            }
                                        }
                                        Button {
                                            text: qsTr("加载全部")
                                            onClicked: refreshFaces()
                                        }
                                        Item { Layout.fillWidth: true }
                                    }

                                    Label { text: qsTr("查询结果: ") + describeFaceData(selectedFaceData) }

                                    ListView {
                                        Layout.fillWidth: true
                                        Layout.preferredHeight: 180
                                        clip: true
                                        model: faceModel
                                        delegate: ItemDelegate {
                                            required property var modelData
                                            width: ListView.view.width
                                            text: describeFaceData(modelData)
                                            onClicked: selectedFaceData = modelData
                                        }
                                    }
                                }
                            }
                        }
                    }

                    ScrollView {
                        clip: true
                        ColumnLayout {
                            width: parent.width
                            spacing: 10

                            GroupBox {
                                title: qsTr("人员同步")
                                Layout.fillWidth: true
                                RowLayout {
                                    anchors.fill: parent
                                    anchors.margins: 8
                                    spacing: 8
                                    Label { text: qsTr("设备 ID") }
                                    TextField { id: syncDeviceId; text: "dev001"; Layout.preferredWidth: 160 }
                                    Button {
                                        text: qsTr("通过 TestApi 下发")
                                        onClicked: {
                                            testApi.requestPersonSync(syncDeviceId.text)
                                            logResult("下发人员同步(TestApi)", testApi.lastError.length === 0, "设备=" + syncDeviceId.text)
                                        }
                                    }
                                    Button {
                                        text: qsTr("直接调用 SyncManager")
                                        onClicked: {
                                            syncManager.sendPersonSyncNow(syncDeviceId.text)
                                            logResult("下发人员同步(SyncManager)", true, "设备=" + syncDeviceId.text)
                                        }
                                    }
                                    Item { Layout.fillWidth: true }
                                }
                            }

                            GroupBox {
                                title: qsTr("CSV 导出")
                                Layout.fillWidth: true
                                ColumnLayout {
                                    anchors.fill: parent
                                    anchors.margins: 8
                                    spacing: 8

                                    RowLayout {
                                        Label { text: qsTr("文件路径") }
                                        TextField {
                                            id: exportPath
                                            Layout.fillWidth: true
                                            placeholderText: qsTr("例如：E:/temp/attendance.csv")
                                        }
                                    }

                                    RowLayout {
                                        Label { text: qsTr("开始") }
                                        TextField { id: exportStart; text: "2026-04-01 00:00:00"; Layout.preferredWidth: 180 }
                                        Label { text: qsTr("结束") }
                                        TextField { id: exportEnd; text: "2026-04-30 23:59:59"; Layout.preferredWidth: 180 }
                                        Button {
                                            text: qsTr("导出人员")
                                            onClicked: {
                                                const ok = exportManager.exportPersonsCsv(exportPath.text)
                                                logResult("导出人员 CSV", ok, ok ? exportPath.text : exportManager.lastError)
                                            }
                                        }
                                        Button {
                                            text: qsTr("导出设备")
                                            onClicked: {
                                                const ok = exportManager.exportDeviceCsv(exportPath.text)
                                                logResult("导出设备 CSV", ok, ok ? exportPath.text : exportManager.lastError)
                                            }
                                        }
                                        Button {
                                            text: qsTr("导出考勤")
                                            onClicked: {
                                                const ok = exportManager.exportAttendanceRecordsCsv(exportPath.text, parseDateTime(exportStart.text), parseDateTime(exportEnd.text))
                                                logResult("导出考勤 CSV", ok, ok ? exportPath.text : exportManager.lastError)
                                            }
                                        }
                                        Item { Layout.fillWidth: true }
                                    }
                                }
                            }
                        }
                    }

                    ScrollView {
                        clip: true
                        ColumnLayout {
                            width: parent.width
                            spacing: 10

                            GroupBox {
                                title: qsTr("统计分析")
                                Layout.fillWidth: true
                                ColumnLayout {
                                    anchors.fill: parent
                                    anchors.margins: 8
                                    spacing: 8

                                    GridLayout {
                                        Layout.fillWidth: true
                                        columns: 8
                                        Label { text: qsTr("开始日期") }
                                        TextField { id: summaryStart; text: "2026-04-01"; Layout.preferredWidth: 140 }
                                        Label { text: qsTr("结束日期") }
                                        TextField { id: summaryEnd; text: "2026-04-30"; Layout.preferredWidth: 140 }
                                        Label { text: qsTr("部门") }
                                        TextField { id: summaryDepartment; placeholderText: qsTr("可空"); Layout.preferredWidth: 140 }
                                        Label { text: qsTr("工号") }
                                        TextField { id: summaryEmployeeId; placeholderText: qsTr("日报可空"); Layout.preferredWidth: 140 }
                                    }

                                    RowLayout {
                                        spacing: 8
                                        Button {
                                            text: qsTr("日报汇总")
                                            onClicked: {
                                                const result = attendanceAnalyzer.dailySummary(parseDate(summaryStart.text), parseDate(summaryEnd.text), summaryDepartment.text, summaryEmployeeId.text)
                                                summaryOutput.text = jsonText(result)
                                                logResult("日报汇总", true, "长度=" + summaryOutput.text.length)
                                            }
                                        }
                                        Button {
                                            text: qsTr("人员汇总")
                                            onClicked: {
                                                const result = attendanceAnalyzer.personSummary(parseDate(summaryStart.text), parseDate(summaryEnd.text), summaryDepartment.text)
                                                summaryOutput.text = jsonText(result)
                                                logResult("人员汇总", true, "长度=" + summaryOutput.text.length)
                                            }
                                        }
                                        Button {
                                            text: qsTr("部门汇总")
                                            onClicked: {
                                                const result = attendanceAnalyzer.departmentSummary(parseDate(summaryStart.text), parseDate(summaryEnd.text))
                                                summaryOutput.text = jsonText(result)
                                                logResult("部门汇总", true, "长度=" + summaryOutput.text.length)
                                            }
                                        }
                                        Item { Layout.fillWidth: true }
                                    }

                                    TextArea {
                                        id: summaryOutput
                                        Layout.fillWidth: true
                                        Layout.preferredHeight: 420
                                        readOnly: true
                                        wrapMode: TextArea.WrapAnywhere
                                        text: qsTr("统计结果会显示在这里")
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        GroupBox {
            title: qsTr("日志")
            SplitView.fillWidth: true
            SplitView.preferredWidth: 450

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 8
                spacing: 8

                RowLayout {
                    Layout.fillWidth: true
                    Button {
                        text: qsTr("清空日志")
                        onClicked: logArea.text = ""
                    }
                    Button {
                        text: qsTr("记录状态快照")
                        onClicked: appendLog("状态快照: db=" + dataManager.isConnected + ", tcp=" + tcpServer.isRunning + ", clients=" + tcpServer.clientCount)
                    }
                    Item { Layout.fillWidth: true }
                }

                TextArea {
                    id: logArea
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    readOnly: true
                    wrapMode: TextArea.WrapAnywhere
                }
            }
        }
    }
}
