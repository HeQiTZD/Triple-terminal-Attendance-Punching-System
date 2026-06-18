import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtCore

import AttendanceAdmin

Item {
    id: root

    required property var sessionManager

    property bool loggingIn: false
    property string loginError: ""
    property bool passwordVisible: false
    property bool rememberUsername: false

    readonly property real cardWidth: Math.min(420, Math.max(340, width - 60))

    // ── 浅色渐变背景 ──
    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            orientation: Gradient.Vertical
            GradientStop { position: 0.0; color: Theme.loginBgStart }
            GradientStop { position: 1.0; color: Theme.loginBgEnd }
        }
    }



    // ── 无边框窗口拖拽 ──
    MouseArea {
        id: windowDragArea
        z: 0
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 44
        acceptedButtons: Qt.LeftButton
        onPressed: function(mouse) {
            const w = Window.window
            if (w && typeof w.startSystemMove === "function")
                w.startSystemMove()
        }
    }

    // ── 顶部按钮：设置 + 关闭 ──
    Row {
        z: 2
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: Theme.spacingMd
        spacing: 2

        ToolButton {
            id: settingsBtn
            enabled: !root.loggingIn
            onClicked: serverSettingsDialog.open()
            contentItem: Image {
                source: "qrc:/images/settings.svg"
                sourceSize: Qt.size(16, 16)
                opacity: settingsBtn.enabled ? (settingsBtn.hovered ? 1.0 : 0.6) : 0.3
                Behavior on opacity { NumberAnimation { duration: 120 } }
            }
            background: Rectangle {
                radius: 6
                color: settingsBtn.hovered ? Theme.hover : "transparent"
            }
        }

        ToolButton {
            id: minimizeBtn
            onClicked: {
                const w = Window.window
                if (w) w.showMinimized()
            }
            contentItem: Image {
                source: "qrc:/images/minied.svg"
                sourceSize: Qt.size(16, 16)
                opacity: minimizeBtn.hovered ? 1.0 : 0.6
                Behavior on opacity { NumberAnimation { duration: 120 } }
            }
            background: Rectangle {
                radius: 6
                color: minimizeBtn.hovered ? Theme.hover : "transparent"
            }
        }

        ToolButton {
            id: closeBtn
            onClicked: {
                const w = Window.window
                if (w) w.close()
            }
            contentItem: Image {
                source: "qrc:/images/close.svg"
                sourceSize: Qt.size(16, 16)
                opacity: closeBtn.hovered ? 1.0 : 0.6
                Behavior on opacity { NumberAnimation { duration: 120 } }
            }
            background: Rectangle {
                radius: 6
                color: closeBtn.hovered ? Qt.rgba(198, 40, 40, 0.1) : "transparent"
            }
        }
    }

    LoginServerSettingsDialog {
        id: serverSettingsDialog
        ownerWindow: Window.window
    }

    // ── 登录卡片 ──
    Item {
        id: loginBlock
        anchors.centerIn: parent
        width: root.cardWidth
        height: cardBg.height

        // 卡片阴影
        Rectangle {
            anchors.fill: cardBg
            anchors.margins: -1
            radius: 16
            color: Qt.rgba(0, 0, 0, 0.08)
            y: 6
        }

        // 卡片主体
        Rectangle {
            id: cardBg
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width
            height: loginColumn.implicitHeight + 56
            radius: 16
            color: Theme.loginCardBg
            border.width: 1
            border.color: Theme.loginCardBorder

            ColumnLayout {
                id: loginColumn
                anchors.top: parent.top
                anchors.topMargin: 32
                anchors.left: parent.left
                anchors.leftMargin: 32
                anchors.right: parent.right
                anchors.rightMargin: 32
                spacing: 20

                // ── 标题区 ──
                ColumnLayout {
                    Layout.alignment: Qt.AlignHCenter
                    spacing: 12

                    // Logo图标
                    Rectangle {
                        Layout.alignment: Qt.AlignHCenter
                        width: 72
                        height: 72
                        radius: 18
                        color: Theme.accentSubtle

                        Text {
                            anchors.centerIn: parent
                            text: "⏱"
                            font.pixelSize: 32
                        }
                    }

                    Label {
                        Layout.alignment: Qt.AlignHCenter
                        text: qsTr("考勤管理系统")
                        color: Theme.loginTextPrimary
                        font.pixelSize: 26
                        font.bold: true
                        font.family: Theme.fontFamily
                    }

                    Label {
                        Layout.alignment: Qt.AlignHCenter
                        text: qsTr("Attendance Management System")
                        color: Theme.loginTextSecondary
                        font.pixelSize: 12
                        font.family: Theme.fontFamily
                    }
                }

                // ── 表单区 ──
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 16

                    // 用户名
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 8

                        Label {
                            text: qsTr("工号 / 用户名")
                            color: Theme.loginTextSecondary
                            font.pixelSize: 14
                            font.family: Theme.fontFamily
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            height: 50
                            radius: 12
                            color: userField.activeFocus ? Theme.loginInputBg : Theme.loginInputBg
                            border.width: 1.5
                            border.color: userField.activeFocus ? Theme.loginInputFocus : Theme.loginInputBorder

                            Behavior on border.color { ColorAnimation { duration: 200 } }

                            RowLayout {
                                anchors.fill: parent
                                anchors.leftMargin: 16
                                anchors.rightMargin: 16
                                spacing: 12

                                Text {
                                    text: "👤"
                                    font.pixelSize: 18
                                    opacity: 0.7
                                }

                                TextField {
                                    id: userField
                                    Layout.fillWidth: true
                                    placeholderText: qsTr("请输入用户名或工号")
                                    enabled: !root.loggingIn
                                    background: null
                                    font.pixelSize: 15
                                    font.family: Theme.fontFamily
                                    color: Theme.loginTextPrimary
                                    placeholderTextColor: Theme.loginTextMuted
                                }
                            }
                        }
                    }

                    // 密码
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 8

                        Label {
                            text: qsTr("密码")
                            color: Theme.loginTextSecondary
                            font.pixelSize: 14
                            font.family: Theme.fontFamily
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            height: 50
                            radius: 12
                            color: passField.activeFocus ? Theme.loginInputBg : Theme.loginInputBg
                            border.width: 1.5
                            border.color: passField.activeFocus ? Theme.loginInputFocus : Theme.loginInputBorder

                            Behavior on border.color { ColorAnimation { duration: 200 } }

                            RowLayout {
                                anchors.fill: parent
                                anchors.leftMargin: 16
                                anchors.rightMargin: 12
                                spacing: 8

                                Text {
                                    text: "🔒"
                                    font.pixelSize: 18
                                    opacity: 0.7
                                }

                                TextField {
                                    id: passField
                                    Layout.fillWidth: true
                                    echoMode: root.passwordVisible ? TextInput.Normal : TextInput.Password
                                    placeholderText: qsTr("请输入密码")
                                    enabled: !root.loggingIn
                                    background: null
                                    font.pixelSize: 15
                                    font.family: Theme.fontFamily
                                    color: Theme.loginTextPrimary
                                    placeholderTextColor: Theme.loginTextMuted
                                    onAccepted: loginBtn.clicked()
                                }

                                // 密码可见性切换
                                Rectangle {
                                    width: 36
                                    height: 36
                                    radius: 8
                                    color: passwordToggleMouse.containsMouse ? Theme.surfaceAlt : "transparent"

                                    Image {
                                        anchors.centerIn: parent
                                        source: root.passwordVisible ? "qrc:/images/visibility.svg" : "qrc:/images/visibility_off.svg"
                                        sourceSize: Qt.size(20, 20)
                                        opacity: passwordToggleMouse.containsMouse ? 1.0 : 0.5
                                    }

                                    MouseArea {
                                        id: passwordToggleMouse
                                        anchors.fill: parent
                                        hoverEnabled: true
                                        cursorShape: Qt.PointingHandCursor
                                        onClicked: root.passwordVisible = !root.passwordVisible
                                    }
                                }
                            }
                        }
                    }
                }

                // ── 错误提示 ──
                Rectangle {
                    Layout.fillWidth: true
                    height: errorLabel.implicitHeight + 20
                    radius: 10
                    color: Qt.rgba(239, 68, 68, 0.12)
                    border.width: 1
                    border.color: Qt.rgba(239, 68, 68, 0.25)
                    visible: root.loginError.length > 0

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 14
                        anchors.rightMargin: 14
                        spacing: 10

                        Text {
                            text: "⚠️"
                            font.pixelSize: 15
                        }

                        Label {
                            id: errorLabel
                            Layout.fillWidth: true
                            wrapMode: Text.WordWrap
                            text: root.loginError
                            color: Theme.danger
                            font.pixelSize: 13
                            font.family: Theme.fontFamily
                        }
                    }
                }

                // ── 记住工号 ──
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 8

                    Rectangle {
                        width: 18
                        height: 18
                        radius: 4
                        border.width: 1.5
                        border.color: root.rememberUsername ? Theme.loginInputFocus : Theme.loginInputBorder
                        color: root.rememberUsername ? Theme.loginInputFocus : "transparent"

                        Behavior on border.color { ColorAnimation { duration: 150 } }
                        Behavior on color { ColorAnimation { duration: 150 } }

                        Text {
                            anchors.centerIn: parent
                            text: "✓"
                            color: "#FFFFFF"
                            font.pixelSize: 12
                            font.bold: true
                            visible: root.rememberUsername
                        }

                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: root.rememberUsername = !root.rememberUsername
                        }
                    }

                    Label {
                        text: qsTr("记住工号")
                        color: Theme.loginTextMuted
                        font.pixelSize: 13
                        font.family: Theme.fontFamily

                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: root.rememberUsername = !root.rememberUsername
                        }
                    }
                }

                // ── 按钮区 ──
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 12

                    // 登录按钮
                    Rectangle {
                        id: loginBtn
                        Layout.fillWidth: true
                        height: 52
                        radius: 12
                        enabled: !root.loggingIn
                               && userField.text.trim().length > 0
                               && passField.text.length > 0
                        opacity: enabled ? 1.0 : 0.5

                        Behavior on opacity { NumberAnimation { duration: 200 } }

                        gradient: Gradient {
                            GradientStop {
                                position: 0.0;
                                color: loginBtnMouse.containsMouse ? Theme.primaryHover : Theme.primary
                            }
                            GradientStop {
                                position: 1.0;
                                color: loginBtnMouse.containsMouse ? Theme.primary : Theme.accent
                            }
                        }

                        // 按下效果
                        Rectangle {
                            anchors.fill: parent
                            radius: 12
                            color: Qt.rgba(0, 0, 0, 0.15)
                            visible: loginBtnMouse.pressed
                        }

                        RowLayout {
                            anchors.centerIn: parent
                            spacing: 10

                            // 加载动画
                            Rectangle {
                                width: 20; height: 20
                                radius: 10
                                border.width: 2.5
                                border.color: "#FFFFFF"
                                color: "transparent"
                                visible: root.loggingIn

                                Rectangle {
                                    x: parent.width / 2 - 1.25
                                    y: 2
                                    width: 2.5; height: 7
                                    radius: 1.25
                                    color: "#FFFFFF"

                                    NumberAnimation on rotation {
                                        from: 0; to: 360
                                        duration: 750
                                        loops: Animation.Infinite
                                        running: root.loggingIn
                                    }
                                    transformOrigin: Item.Bottom
                                }
                            }

                            Label {
                                text: root.loggingIn ? qsTr("登录中…") : qsTr("登 录")
                                color: "#FFFFFF"
                                font.pixelSize: 16
                                font.bold: true
                                font.family: Theme.fontFamily
                            }
                        }

                        MouseArea {
                            id: loginBtnMouse
                            anchors.fill: parent
                            hoverEnabled: true
                            enabled: loginBtn.enabled
                            cursorShape: Qt.PointingHandCursor
                            onClicked: root._submitLogin()
                        }
                    }

                    // 跳过登录按钮
                    Rectangle {
                        id: devLoginBtn
                        Layout.fillWidth: true
                        height: 44
                        radius: 10
                        color: devLoginBtnMouse.containsMouse ? Theme.surfaceAlt : "transparent"
                        enabled: !root.loggingIn

                        Behavior on color { ColorAnimation { duration: 200 } }

                        Label {
                            anchors.centerIn: parent
                            text: qsTr("跳过登录（开发模式）")
                            color: devLoginBtnMouse.containsMouse ? Theme.primary : Theme.textSubtle
                            font.pixelSize: 13
                            font.family: Theme.fontFamily

                            Behavior on color { ColorAnimation { duration: 200 } }
                        }

                        MouseArea {
                            id: devLoginBtnMouse
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: root.sessionManager.devLogin()
                        }

                        ToolTip.text: qsTr("开发模式：跳过服务器验证，直接进入主界面")
                        ToolTip.visible: devLoginBtnMouse.containsMouse
                    }
                }
            }
        }
    }

    // ── 底部版权信息 ──
    Label {
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        anchors.horizontalCenter: parent.horizontalCenter
        text: "© 2026 AttendanceAdmin"
        color: Theme.textSubtle
        font.pixelSize: 12
        font.family: Theme.fontFamily
    }

    // ── 入场动画 ──
    PropertyAnimation {
        id: cardEnterAnim
        target: loginBlock
        property: "opacity"
        from: 0; to: 1
        duration: 500
        easing.type: Easing.OutCubic
    }
    PropertyAnimation {
        id: cardSlideAnim
        target: loginBlock
        property: "y"
        from: loginBlock.y + 30
        to: loginBlock.y
        duration: 500
        easing.type: Easing.OutCubic
    }

    // ── 记住用户名功能 ──
    function _loadSavedUsername() {
        var settings = Qt.createQmlObject('import QtCore; Settings {}', root)
        var saved = settings.value("rememberedUsername", "")
        if (saved) {
            userField.text = saved
            root.rememberUsername = true
        }
    }

    function _saveUsername() {
        var settings = Qt.createQmlObject('import QtCore; Settings {}', root)
        if (root.rememberUsername && userField.text.trim().length > 0) {
            settings.setValue("rememberedUsername", userField.text.trim())
        } else {
            settings.remove("rememberedUsername")
        }
    }

    // ── 逻辑函数 ──
    function _resetForm() {
        userField.text = ""
        passField.text = ""
        root.loginError = ""
        root.loggingIn = false
        root.passwordVisible = false
        root.rememberUsername = false
        _loadSavedUsername()
    }

    function _submitLogin() {
        root.loginError = ""
        root.loggingIn = true
        _saveUsername()
        root.sessionManager.login(
            Presets.serverHost,
            Presets.serverPort,
            userField.text.trim(),
            passField.text
        )
    }

    Component.onCompleted: {
        _resetForm()
        cardEnterAnim.start()
        cardSlideAnim.start()
    }

    onVisibleChanged: {
        if (visible)
            _resetForm()
    }

    Connections {
        target: root.sessionManager
        function onLoggedIn() {
            root.loggingIn = false
            root.loginError = ""
            Logger.ok(qsTr("登录成功"))
        }
        function onLoginFailed(code, message) {
            root.loggingIn = false
            root.loginError = ErrorCatalog.messageForCode(code, message)
            Logger.error(root.loginError)
        }
        function onErrorOccurred(error) {
            if (root.loggingIn && !root.sessionManager.isLoggedIn
                && root.sessionManager.connectionState === 0) {
                root.loggingIn = false
                root.loginError = qsTr("未连接服务器")
            }
        }
        function onConnectionStateChanged() {
            if (root.loggingIn && !root.sessionManager.isLoggedIn
                && root.sessionManager.connectionState === 0) {
                root.loggingIn = false
                root.loginError = qsTr("未连接服务器")
            }
        }
        function onLoggedOut() {
            root._resetForm()
        }
    }
}
