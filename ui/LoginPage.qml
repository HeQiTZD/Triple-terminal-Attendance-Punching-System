import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceAdmin

Item {
    id: root

    required property var sessionManager

    property bool loggingIn: false
    property string loginError: ""

    readonly property real cardWidth: Math.min(420, Math.max(320, width - Theme.spacingXl * 2))

    Rectangle {
        anchors.fill: parent
        color: Theme.bg
    }

    // 无边框窗口：顶部拖拽移动
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

    Row {
        z: 1
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: Theme.spacingMd
        spacing: 2

        ToolButton {
            id: settingsBtn
            text: "\u2699"
            font.pixelSize: Theme.fontLg
            enabled: !root.loggingIn
            onClicked: serverSettingsDialog.open()
        }

        ToolButton {
            id: closeBtn
            text: "\u00d7"
            font.pixelSize: Theme.fontXl
            onClicked: {
                const w = Window.window
                if (w)
                    w.close()
            }
        }
    }

    LoginServerSettingsDialog {
        id: serverSettingsDialog
        ownerWindow: Window.window
    }

    // 登录主体：相对整个窗口上下左右居中
    Item {
        id: loginBlock
        anchors.centerIn: parent
        width: root.cardWidth
        height: loginColumn.implicitHeight

        ColumnLayout {
            id: loginColumn
            width: parent.width
            spacing: Theme.spacingLg

            ColumnLayout {
                Layout.alignment: Qt.AlignHCenter
                spacing: Theme.spacingSm

                Rectangle {
                    Layout.alignment: Qt.AlignHCenter
                    width: 48
                    height: 48
                    radius: Theme.radiusLg
                    color: Theme.accent

                    Label {
                        anchors.centerIn: parent
                        text: "A"
                        color: "white"
                        font.bold: true
                        font.pixelSize: 22
                    }
                }

                Label {
                    Layout.alignment: Qt.AlignHCenter
                    text: qsTr("考勤管理系统")
                    color: Theme.text
                    font.pixelSize: Theme.fontXl
                    font.bold: true
                    font.family: Theme.fontFamily
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: Theme.spacingMd

                GridLayout {
                    Layout.fillWidth: true
                    columns: 2
                    rowSpacing: Theme.spacingSm
                    columnSpacing: Theme.spacingMd

                    LabeledField {
                        label: qsTr("工号 / 用户名")
                        Layout.fillWidth: true
                        Layout.columnSpan: 2
                        TextField {
                            id: userField
                            Layout.fillWidth: true
                            placeholderText: qsTr("请输入用户名或工号")
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
                }

                Label {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter
                    horizontalAlignment: Text.AlignHCenter
                    visible: root.loginError.length > 0
                    wrapMode: Text.WordWrap
                    text: root.loginError
                    color: Theme.danger
                    font.pixelSize: Theme.fontSm
                    font.family: Theme.fontFamily
                }

                Item {
                    Layout.fillWidth: true
                    Layout.preferredHeight: loginBtnRow.implicitHeight

                    Row {
                        id: loginBtnRow
                        anchors.horizontalCenter: parent.horizontalCenter
                        spacing: Theme.spacingSm

                        BusyIndicator {
                            visible: root.loggingIn
                            running: root.loggingIn
                        }

                        Button {
                            id: loginBtn
                            text: root.loggingIn ? qsTr("登录中…") : qsTr("登录")
                            highlighted: true
                            enabled: !root.loggingIn
                                   && userField.text.trim().length > 0
                                   && passField.text.length > 0
                            onClicked: root._submitLogin()
                        }
                    }
                }
            }
        }
    }

    function _resetForm() {
        userField.text = ""
        passField.text = ""
        root.loginError = ""
        root.loggingIn = false
    }

    function _submitLogin() {
        root.loginError = ""
        root.loggingIn = true
        root.sessionManager.login(
            Presets.serverHost,
            Presets.serverPort,
            userField.text.trim(),
            passField.text
        )
    }

    Component.onCompleted: _resetForm()

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
            if (root.loggingIn && !root.sessionManager.isLoggedIn)
                root.loginError = error
        }
        function onLoggedOut() {
            root._resetForm()
        }
    }
}
