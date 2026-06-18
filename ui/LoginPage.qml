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
                opacity: settingsBtn.enabled ? (settingsBtn.hovered ? 0.8 : 0.4) : 0.2
                Behavior on opacity { NumberAnimation { duration: 120 } }
            }
            background: Rectangle {
                radius: 4
                color: settingsBtn.hovered ? Qt.rgba(0, 0, 0, 0.05) : "transparent"
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
                opacity: minimizeBtn.hovered ? 0.8 : 0.4
                Behavior on opacity { NumberAnimation { duration: 120 } }
            }
            background: Rectangle {
                radius: 4
                color: minimizeBtn.hovered ? Qt.rgba(0, 0, 0, 0.05) : "transparent"
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
                opacity: closeBtn.hovered ? 0.8 : 0.4
                Behavior on opacity { NumberAnimation { duration: 120 } }
            }
            background: Rectangle {
                radius: 4
                color: closeBtn.hovered ? Qt.rgba(0.78, 0.16, 0.16, 0.1) : "transparent"
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
                    spacing: 8

                    // Logo图标
                    Rectangle {
                        Layout.alignment: Qt.AlignHCenter
                        width: 64
                        height: 64
                        radius: 16
                        color: Theme.primarySubtle

                        Text {
                            anchors.centerIn: parent
                            text: "⏱"
                            font.pixelSize: 28
                        }
                    }

                    Label {
                        Layout.alignment: Qt.AlignHCenter
                        text: qsTr("考勤管理系统")
                        color: Theme.loginTextPrimary
                        font.pixelSize: 24
                        font.bold: true
                        font.family: Theme.fontFamily
                    }

                    Label {
                        Layout.alignment: Qt.AlignHCenter
                        text: qsTr("管理端登录")
                        color: Theme.loginTextSecondary
                        font.pixelSize: 13
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
                        spacing: 6

                        Label {
                            text: qsTr("工号 / 用户名")
                            color: Theme.loginTextSecondary
                            font.pixelSize: 13
                            font.family: Theme.fontFamily
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            height: 44
                            radius: 8
                            color: userField.activeFocus ? "#FFFFFF" : Theme.loginInputBg
                            border.width: userField.activeFocus ? 2 : 1
                            border.color: userField.activeFocus ? Theme.loginInputFocus : Theme.loginInputBorder

                            Behavior on border.color { ColorAnimation { duration: 150 } }
                            Behavior on border.width { NumberAnimation { duration: 150 } }

                            RowLayout {
                                anchors.fill: parent
                                anchors.leftMargin: 12
                                anchors.rightMargin: 12
                                spacing: 8

                                Text {
                                    text: "👤"
                                    font.pixelSize: 16
                                    color: Theme.loginTextMuted
                                }

                                TextField {
                                    id: userField
                                    Layout.fillWidth: true
                                    placeholderText: qsTr("请输入用户名或工号")
                                    enabled: !root.loggingIn
                                    background: null
                                    font.pixelSize: 14
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
                        spacing: 6

                        Label {
                            text: qsTr("密码")
                            color: Theme.loginTextSecondary
                            font.pixelSize: 13
                            font.family: Theme.fontFamily
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            height: 44
                            radius: 8
                            color: passField.activeFocus ? "#FFFFFF" : Theme.loginInputBg
                            border.width: passField.activeFocus ? 2 : 1
                            border.color: passField.activeFocus ? Theme.loginInputFocus : Theme.loginInputBorder

                            Behavior on border.color { ColorAnimation { duration: 150 } }
                            Behavior on border.width { NumberAnimation { duration: 150 } }

                            RowLayout {
                                anchors.fill: parent
                                anchors.leftMargin: 12
                                anchors.rightMargin: 8
                                spacing: 8

                                Text {
                                    text: "🔒"
                                    font.pixelSize: 16
                                    color: Theme.loginTextMuted
                                }

                                TextField {
                                    id: passField
                                    Layout.fillWidth: true
                                    echoMode: root.passwordVisible ? TextInput.Normal : TextInput.Password
                                    placeholderText: qsTr("请输入密码")
                                    enabled: !root.loggingIn
                                    background: null
                                    font.pixelSize: 14
                                    font.family: Theme.fontFamily
                                    color: Theme.loginTextPrimary
                                    placeholderTextColor: Theme.loginTextMuted
                                    onAccepted: loginBtn.clicked()
                                }

                                // 密码可见性切换
                                Rectangle {
                                    width: 32
                                    height: 32
                                    radius: 6
                                    color: passwordToggleMouse.containsMouse ? Qt.rgba(0, 0, 0, 0.05) : "transparent"

                                    Image {
                                        anchors.centerIn: parent
                                        source: root.passwordVisible ? "qrc:/images/visibility.svg" : "qrc:/images/visibility_off.svg"
                                        sourceSize: Qt.size(18, 18)
                                        opacity: passwordToggleMouse.containsMouse ? 0.8 : 0.4
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
                    height: errorLabel.implicitHeight + 16
                    radius: 8
                    color: "#FEF2F2"
                    border.width: 1
                    border.color: "#FECACA"
                    visible: root.loginError.length > 0

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 12
                        anchors.rightMargin: 12
                        spacing: 8

                        Text {
                            text: "⚠️"
                            font.pixelSize: 14
                            color: "#DC2626"
                        }

                        Label {
                            id: errorLabel
                            Layout.fillWidth: true
                            wrapMode: Text.WordWrap
                            text: root.loginError
                            color: "#DC2626"
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
                        height: 44
                        radius: 8
                        enabled: !root.loggingIn
                               && userField.text.trim().length > 0
                               && passField.text.length > 0
                        opacity: enabled ? 1.0 : 0.5

                        Behavior on opacity { NumberAnimation { duration: 150 } }

                        gradient: Gradient {
                            GradientStop {
                                position: 0.0;
                                color: loginBtnMouse.containsMouse ? "#3B82F6" : Theme.primary
                            }
                            GradientStop {
                                position: 1.0;
                                color: loginBtnMouse.containsMouse ? "#2563EB" : "#1D4ED8"
                            }
                        }

                        // 按下效果
                        Rectangle {
                            anchors.fill: parent
                            radius: 8
                            color: Qt.rgba(0, 0, 0, 0.1)
                            visible: loginBtnMouse.pressed
                        }

                        RowLayout {
                            anchors.centerIn: parent
                            spacing: 8

                            // 加载动画
                            Rectangle {
                                width: 16; height: 16
                                radius: 8
                                border.width: 2
                                border.color: "#FFFFFF"
                                color: "transparent"
                                visible: root.loggingIn

                                Rectangle {
                                    x: parent.width / 2 - 1
                                    y: 0
                                    width: 2; height: 6
                                    radius: 1
                                    color: "#FFFFFF"

                                    NumberAnimation on rotation {
                                        from: 0; to: 360
                                        duration: 800
                                        loops: Animation.Infinite
                                        running: root.loggingIn
                                    }
                                    transformOrigin: Item.Bottom
                                }
                            }

                            Label {
                                text: root.loggingIn ? qsTr("登录中…") : qsTr("登 录")
                                color: "#FFFFFF"
                                font.pixelSize: 15
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
                        height: 36
                        radius: 6
                        color: devLoginBtnMouse.containsMouse ? "#F3F4F6" : "transparent"
                        enabled: !root.loggingIn

                        Behavior on color { ColorAnimation { duration: 150 } }

                        Label {
                            anchors.centerIn: parent
                            text: qsTr("跳过登录（开发模式）")
                            color: devLoginBtnMouse.containsMouse ? Theme.primary : Theme.loginTextMuted
                            font.pixelSize: 12
                            font.family: Theme.fontFamily

                            Behavior on color { ColorAnimation { duration: 150 } }
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
        anchors.bottomMargin: 16
        anchors.horizontalCenter: parent.horizontalCenter
        text: "© 2026 AttendanceAdmin"
        color: Theme.loginTextMuted
        font.pixelSize: 11
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

    // ── 键盘快捷键 ──
    Keys.onEscapePressed: {
        root.loginError = ""
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
