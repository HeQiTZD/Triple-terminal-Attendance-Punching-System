import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceAdmin

Dialog {
    id: root
    modal: true
    closePolicy: Popup.NoAutoClose
    focus: true
    title: qsTr("登录考勤管理端")
    standardButtons: Dialog.NoButton
    anchors.centerIn: Overlay.overlay

    property var sessionManager
    property bool loggingIn: false
    property string loginError: ""

    readonly property real _maxW: Overlay.overlay && Overlay.overlay.width > 0
        ? Math.max(360, Overlay.overlay.width - 80)
        : 440

    implicitWidth: Math.min(420, _maxW)

    background: Rectangle {
        color: Theme.surface
        border.color: Theme.border
        border.width: 1
        radius: Theme.radiusMd
    }

    onOpened: {
        hostField.text = Presets.serverHost
        portSpin.value = Presets.serverPort
        userField.text = Presets.defaultEmployeeId
        clientField.text = "admin_001"
        passField.text = ""
        root.loginError = ""
    }

    contentItem: ColumnLayout {
        spacing: Theme.spacingMd
        width: root.implicitWidth - Theme.spacingLg * 2

        Label {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            text: qsTr("连接 attendanceServer 并完成认证后进入管理界面。")
            color: Theme.textMuted
            font.pixelSize: Theme.fontSm
            font.family: Theme.fontFamily
        }

        GridLayout {
            Layout.fillWidth: true
            columns: 2
            rowSpacing: Theme.spacingSm
            columnSpacing: Theme.spacingMd

            LabeledField {
                label: qsTr("服务器地址")
                Layout.fillWidth: true
                TextField {
                    id: hostField
                    Layout.fillWidth: true
                    placeholderText: "127.0.0.1"
                    enabled: !root.loggingIn
                }
            }
            LabeledField {
                label: qsTr("端口")
                Layout.fillWidth: true
                SpinBox {
                    id: portSpin
                    from: 1
                    to: 65535
                    editable: true
                    enabled: !root.loggingIn
                    Layout.fillWidth: true
                }
            }
            LabeledField {
                label: qsTr("工号 / 用户名")
                Layout.fillWidth: true
                Layout.columnSpan: 2
                TextField {
                    id: userField
                    Layout.fillWidth: true
                    placeholderText: qsTr("admin")
                    enabled: !root.loggingIn
                }
            }
            LabeledField {
                label: qsTr("密码")
                Layout.fillWidth: true
                Layout.columnSpan: 2
                TextField {
                    id: passField
                    Layout.fillWidth: true
                    echoMode: TextInput.Password
                    placeholderText: qsTr("请输入密码")
                    enabled: !root.loggingIn
                    onAccepted: loginBtn.clicked()
                }
            }
            LabeledField {
                label: qsTr("客户端 ID")
                Layout.fillWidth: true
                Layout.columnSpan: 2
                TextField {
                    id: clientField
                    Layout.fillWidth: true
                    placeholderText: "admin_001"
                    enabled: !root.loggingIn
                }
            }
        }

        Label {
            Layout.fillWidth: true
            visible: root.loginError.length > 0
            wrapMode: Text.WordWrap
            text: root.loginError
            color: Theme.danger
            font.pixelSize: Theme.fontSm
            font.family: Theme.fontFamily
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingSm

            Item { Layout.fillWidth: true }

            BusyIndicator {
                visible: root.loggingIn
                running: root.loggingIn
            }

            Button {
                id: loginBtn
                text: root.loggingIn ? qsTr("连接中…") : qsTr("连接并登录")
                highlighted: true
                enabled: !root.loggingIn
                       && hostField.text.trim().length > 0
                       && userField.text.trim().length > 0
                       && passField.text.length > 0
                onClicked: {
                    root.loginError = ""
                    root.loggingIn = true
                    Presets.serverHost = hostField.text.trim()
                    Presets.serverPort = portSpin.value
                    Presets.notifyChanged()
                    root.sessionManager.login(
                        hostField.text.trim(),
                        portSpin.value,
                        userField.text.trim(),
                        passField.text,
                        clientField.text.trim() || "admin_001"
                    )
                }
            }
        }
    }

    Connections {
        target: root.sessionManager
        function onLoggedIn() {
            root.loggingIn = false
            root.loginError = ""
            root.close()
            Logger.ok(qsTr("登录成功"))
        }
        function onLoginFailed(code, message) {
            root.loggingIn = false
            root.loginError = ErrorCatalog.messageForCode(code, message)
            Logger.error(root.loginError)
        }
        function onErrorOccurred(error) {
            if (root.loggingIn && !root.sessionManager.isLoggedIn)
                root.loginError = error
        }
    }
}
