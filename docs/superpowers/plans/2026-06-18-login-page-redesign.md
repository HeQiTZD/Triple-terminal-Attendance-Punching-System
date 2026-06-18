# 登录页面重新设计实现计划

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** 重新设计登录页面，实现记住用户名功能、密码可见性切换和简约专业视觉风格

**Architecture:** 基于现有LoginPage.qml进行重构，更新Theme.qml颜色方案，添加QSettings用于记住用户名功能

**Tech Stack:** Qt 6 QML, Qt Quick.Controls, Qt Quick.Layouts, QtCore (QSettings)

---

## 文件结构

### 修改的文件
- `ui/theme/Theme.qml` - 添加登录页面专用颜色定义
- `ui/LoginPage.qml` - 重构登录页面，添加记住用户名功能

### 新增的文件
- 无

---

## Task 1: 更新Theme.qml颜色方案

**Files:**
- Modify: `ui/theme/Theme.qml:7-50`

- [ ] **Step 1: 添加登录页面专用颜色**

在Theme.qml中添加以下颜色属性（在现有颜色定义之后）：

```qml
// ===== 登录页面颜色 =====
readonly property color loginBgStart:      "#F8FAFC"
readonly property color loginBgEnd:        "#F1F5F9"
readonly property color loginCardBg:       "#FFFFFF"
readonly property color loginCardBorder:   "#E2E8F0"
readonly property color loginTextPrimary:  "#1E293B"
readonly property color loginTextSecondary:"#64748B"
readonly property color loginTextMuted:    "#94A3B8"
readonly property color loginInputBg:      "#F9FAFB"
readonly property color loginInputBorder:  "#D1D5DB"
readonly property color loginInputFocus:   "#2563EB"
```

- [ ] **Step 2: 验证Theme.qml语法**

运行以下命令检查QML语法：
```bash
cd "E:\project\AttendanceServer"
# 使用Qt的qmlformat或手动检查语法
```

预期：无语法错误

- [ ] **Step 3: 提交更改**

```bash
cd "E:\project\AttendanceServer"
git add ui/theme/Theme.qml
git commit -m "feat(theme): add login page color scheme"
```

---

## Task 2: 添加记住用户名功能的基础结构

**Files:**
- Modify: `ui/LoginPage.qml:7-16`

- [ ] **Step 1: 添加rememberUsername属性**

在LoginPage.qml的属性声明部分（第14行之后）添加：

```qml
property bool rememberUsername: false
```

- [ ] **Step 2: 添加QSettings导入**

在LoginPage.qml的import部分添加：

```qml
import QtCore
```

- [ ] **Step 3: 添加加载保存用户名的函数**

在逻辑函数部分（第547行之前）添加：

```qml
function _loadSavedUsername() {
    var settings = Qt.createQmlObject('import QtCore; Settings {}', root)
    var saved = settings.value("rememberedUsername", "")
    if (saved) {
        userField.text = saved
        root.rememberUsername = true
    }
}
```

- [ ] **Step 4: 添加保存用户名的函数**

```qml
function _saveUsername() {
    var settings = Qt.createQmlObject('import QtCore; Settings {}', root)
    if (root.rememberUsername && userField.text.trim().length > 0) {
        settings.setValue("rememberedUsername", userField.text.trim())
    } else {
        settings.remove("rememberedUsername")
    }
}
```

- [ ] **Step 5: 修改_resetForm函数**

更新_resetForm函数，加载保存的用户名：

```qml
function _resetForm() {
    userField.text = ""
    passField.text = ""
    root.loginError = ""
    root.loggingIn = false
    root.passwordVisible = false
    root.rememberUsername = false
    _loadSavedUsername()
}
```

- [ ] **Step 6: 修改_submitLogin函数**

在_submitLogin函数中添加保存用户名的逻辑：

```qml
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
```

- [ ] **Step 7: 提交更改**

```bash
cd "E:\project\AttendanceServer"
git add ui/LoginPage.qml
git commit -m "feat(login): add remember username functionality"
```

---

## Task 3: 添加记住工号复选框UI

**Files:**
- Modify: `ui/LoginPage.qml:358-392` (错误提示和按钮区之间)

- [ ] **Step 1: 添加复选框UI**

在错误提示部分（第392行）和按钮区部分（第394行）之间添加：

```qml
// ── 记住工号 ──
RowLayout {
    Layout.fillWidth: true
    spacing: 8

    Rectangle {
        width: 18
        height: 18
        radius: 4
        border.width: 1.5
        border.color: root.rememberUsername ? "#2563EB" : "#D1D5DB"
        color: root.rememberUsername ? "#2563EB" : "transparent"

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
        color: "#94A3B8"
        font.pixelSize: 13
        font.family: Theme.fontFamily

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: root.rememberUsername = !root.rememberUsername
        }
    }
}
```

- [ ] **Step 2: 提交更改**

```bash
cd "E:\project\AttendanceServer"
git add ui/LoginPage.qml
git commit -m "feat(login): add remember username checkbox UI"
```

---

## Task 4: 更新视觉设计 - 背景和卡片

**Files:**
- Modify: `ui/LoginPage.qml:18-27` (背景部分)
- Modify: `ui/LoginPage.qml:164-182` (卡片部分)

- [ ] **Step 1: 更新背景渐变**

将深色背景改为浅色渐变（第19-27行）：

```qml
// ── 浅色渐变背景 ──
Rectangle {
    anchors.fill: parent
    gradient: Gradient {
        orientation: Gradient.Vertical
        GradientStop { position: 0.0; color: Theme.loginBgStart }
        GradientStop { position: 1.0; color: Theme.loginBgEnd }
    }
}
```

- [ ] **Step 2: 移除网格纹理装饰**

删除第30-51行的Canvas元素（网格纹理）

- [ ] **Step 3: 移除浮动光点装饰**

删除第53-73行的Repeater元素（浮动光点）

- [ ] **Step 4: 更新卡片样式**

更新卡片阴影和背景（第164-182行）：

```qml
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
```

- [ ] **Step 5: 更新卡片内边距**

更新ColumnLayout的内边距（第185-192行）：

```qml
ColumnLayout {
    id: loginColumn
    anchors.top: parent.top
    anchors.topMargin: 32
    anchors.left: parent.left
    anchors.leftMargin: 32
    anchors.right: parent.right
    anchors.rightMargin: 32
    spacing: 20
```

- [ ] **Step 6: 提交更改**

```bash
cd "E:\project\AttendanceServer"
git add ui/LoginPage.qml
git commit -m "feat(login): update visual design to light theme"
```

---

## Task 5: 更新视觉设计 - 标题和表单

**Files:**
- Modify: `ui/LoginPage.qml:194-230` (标题区)
- Modify: `ui/LoginPage.qml:232-358` (表单区)

- [ ] **Step 1: 更新标题区样式**

更新标题区（第194-230行）：

```qml
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
        color: Theme.primaryLight

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
```

- [ ] **Step 2: 更新用户名输入框样式**

更新用户名输入框（第237-284行）：

```qml
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
```

- [ ] **Step 3: 更新密码输入框样式**

更新密码输入框（第286-358行）：

```qml
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
```

- [ ] **Step 4: 提交更改**

```bash
cd "E:\project\AttendanceServer"
git add ui/LoginPage.qml
git commit -m "feat(login): update title and form styling"
```

---

## Task 6: 更新视觉设计 - 按钮和错误提示

**Files:**
- Modify: `ui/LoginPage.qml:361-392` (错误提示)
- Modify: `ui/LoginPage.qml:394-512` (按钮区)

- [ ] **Step 1: 更新错误提示样式**

更新错误提示（第361-392行）：

```qml
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
```

- [ ] **Step 2: 更新登录按钮样式**

更新登录按钮（第399-478行）：

```qml
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
```

- [ ] **Step 3: 更新跳过登录按钮样式**

更新跳过登录按钮（第480-512行）：

```qml
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
```

- [ ] **Step 4: 提交更改**

```bash
cd "E:\project\AttendanceServer"
git add ui/LoginPage.qml
git commit -m "feat(login): update button and error styling"
```

---

## Task 7: 更新视觉设计 - 顶部按钮和版权信息

**Files:**
- Modify: `ui/LoginPage.qml:91-150` (顶部按钮)
- Modify: `ui/LoginPage.qml:517-526` (版权信息)

- [ ] **Step 1: 更新顶部按钮样式**

更新顶部按钮（第91-150行）：

```qml
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
            color: closeBtn.hovered ? Qt.rgba(220, 38, 38, 0.1) : "transparent"
        }
    }
}
```

- [ ] **Step 2: 更新版权信息样式**

更新版权信息（第517-526行）：

```qml
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
```

- [ ] **Step 3: 提交更改**

```bash
cd "E:\project\AttendanceServer"
git add ui/LoginPage.qml
git commit -m "feat(login): update top buttons and footer styling"
```

---

## Task 8: 添加键盘快捷键支持

**Files:**
- Modify: `ui/LoginPage.qml:567-576` (Component.onCompleted)

- [ ] **Step 1: 添加Escape键处理**

在Component.onCompleted部分（第567行之前）添加：

```qml
// ── 键盘快捷键 ──
Keys.onEscapePressed: {
    root.loginError = ""
}
```

- [ ] **Step 2: 确保Tab键默认行为正常**

Tab键在QML中默认支持焦点切换，无需额外代码

- [ ] **Step 3: 提交更改**

```bash
cd "E:\project\AttendanceServer"
git add ui/LoginPage.qml
git commit -m "feat(login): add keyboard shortcut support"
```

---

## Task 9: 测试和验证

**Files:**
- Test: `ui/LoginPage.qml`

- [ ] **Step 1: 构建项目**

```bash
cd "E:\project\AttendanceServer"
cmake --build out/build/debug
```

预期：构建成功，无编译错误

- [ ] **Step 2: 运行应用程序**

```bash
cd "E:\project\AttendanceServer"
./out/build/debug/appAttendanceAdmin.exe
```

预期：应用程序正常启动，显示登录界面

- [ ] **Step 3: 测试记住用户名功能**

1. 输入用户名（如"admin"）
2. 勾选"记住工号"
3. 输入密码
4. 点击登录（或跳过登录）
5. 关闭应用程序
6. 重新启动应用程序
7. 验证用户名自动填充

预期：用户名自动填充，复选框选中

- [ ] **Step 4: 测试取消记住用户名**

1. 取消勾选"记住工号"
2. 点击登录（或跳过登录）
3. 关闭应用程序
4. 重新启动应用程序
5. 验证用户名为空

预期：用户名为空，复选框未选中

- [ ] **Step 5: 测试密码可见性切换**

1. 输入密码
2. 点击眼睛图标
3. 验证密码显示为明文
4. 再次点击眼睛图标
5. 验证密码隐藏为圆点

预期：密码显示/隐藏切换正常

- [ ] **Step 6: 测试视觉设计**

1. 验证背景为浅色渐变
2. 验证卡片为白色背景
3. 验证输入框聚焦时边框变蓝
4. 验证按钮悬停效果
5. 验证错误提示样式

预期：视觉效果符合设计文档

- [ ] **Step 7: 提交最终更改**

```bash
cd "E:\project\AttendanceServer"
git add -A
git commit -m "feat(login): complete login page redesign"
```

---

## 验收标准

### 必须满足
1. [ ] 记住用户名功能正常工作
2. [ ] 密码可见性切换正常
3. [ ] 视觉设计符合简约商务风格
4. [ ] 所有测试用例通过

### 应该满足
1. [ ] 动画效果流畅
2. [ ] 错误提示清晰
3. [ ] 键盘快捷键可用

---

## 实现时间估算

- Task 1-2: 基础结构（30分钟）
- Task 3-4: UI组件（45分钟）
- Task 5-6: 视觉样式（45分钟）
- Task 7-8: 细节完善（30分钟）
- Task 9: 测试验证（30分钟）

**总计: 约3小时**
