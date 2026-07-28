# 登录页面重新设计 - 详细设计文档

**文档版本**: v1.0
**创建日期**: 2026-06-18
**状态**: 已批准
**设计者**: Claude

---

## 1. 概述

### 1.1 设计目标

基于用户需求，重新设计登录页面，实现：
- **记住用户名功能**：只保存用户名，下次自动填充
- **密码可见性切换**：显示/隐藏密码按钮（已实现）
- **视觉体验优化**：简约专业风格，全屏背景布局

### 1.2 设计原则

- **简约专业**：符合企业级应用的专业形象
- **用户友好**：降低学习成本，提升操作效率
- **安全可靠**：只保存用户名，不保存密码
- **可维护性**：代码结构清晰，便于后续维护

---

## 2. 视觉设计

### 2.1 颜色方案

```qml
// 主色调
readonly property color primary: "#2563EB"        // 蓝色主色
readonly property color primaryHover: "#3B82F6"   // 蓝色悬停
readonly property color primaryLight: "#DBEAFE"   // 蓝色浅色

// 背景色
readonly property color bgGradientStart: "#F8FAFC"  // 渐变开始
readonly property color bgGradientEnd: "#F1F5F9"    // 渐变结束

// 卡片色
readonly property color cardBg: "#FFFFFF"           // 卡片背景
readonly property color cardBorder: "#E2E8F0"      // 卡片边框

// 文字色
readonly property color textPrimary: "#1E293B"     // 主要文字
readonly property color textSecondary: "#64748B"   // 次要文字
readonly property color textMuted: "#94A3B8"       // 淡化文字
```

### 2.2 布局结构

```
┌─────────────────────────────────────┐
│ 顶部栏：设置 | 最小化 | 关闭        │
├─────────────────────────────────────┤
│                                     │
│         ┌─────────────────┐         │
│         │    Logo图标     │         │
│         │  考勤管理系统    │         │
│         │  管理端登录      │         │
│         ├─────────────────┤         │
│         │ 工号/用户名      │         │
│         │ [输入框]        │         │
│         │                 │         │
│         │ 密码            │         │
│         │ [输入框] [眼睛] │         │
│         │                 │         │
│         │ ☑ 记住工号      │         │
│         │                 │         │
│         │   [登录按钮]    │         │
│         │                 │         │
│         │ 跳过登录        │         │
│         └─────────────────┘         │
│                                     │
│         © 2026 AttendanceAdmin      │
└─────────────────────────────────────┘
```

### 2.3 动画效果

- **页面加载**：卡片从下方滑入（500ms）
- **输入框聚焦**：边框颜色渐变（200ms）
- **按钮悬停**：颜色加深（150ms）
- **错误提示**：淡入显示（300ms）

---

## 3. 功能设计

### 3.1 记住用户名功能

#### 数据存储
- 使用 Qt 的 QSettings 存储用户名
- 存储路径：`HKEY_CURRENT_USER\Software\AttendanceAdmin`
- 键名：`rememberedUsername`

#### 功能流程
1. 用户登录时，如果勾选"记住工号"，保存用户名到注册表
2. 下次启动时，自动读取并填充用户名
3. 用户可随时取消勾选，清除保存的用户名

#### UI元素
- 复选框：`CheckBox { text: "记住工号" }`
- 位置：密码输入框下方，登录按钮上方

### 3.2 密码可见性切换（已实现）

#### 功能
- 点击眼睛图标切换密码显示/隐藏
- 使用 `visibility.svg` 和 `visibility_off.svg`

#### 交互
- 悬停时图标高亮
- 点击时切换状态
- 密码字段同步切换 `echoMode`

### 3.3 表单验证

#### 实时验证
- 用户名：非空验证
- 密码：非空验证
- 验证失败：输入框边框变红

#### 提交前验证
- 点击登录按钮时检查所有字段
- 验证失败：显示错误提示

### 3.4 错误处理

#### 错误类型
- 网络错误：显示"未连接服务器"
- 认证失败：显示"用户名或密码错误"
- 服务器错误：显示具体错误信息

#### 错误提示
- 位置：表单上方
- 样式：红色背景，白色文字
- 自动消失：5秒后

---

## 4. 交互设计

### 4.1 用户流程

```
启动应用
    ↓
读取保存的用户名（如果有）
    ↓
显示登录界面
    ↓
用户输入/确认用户名
    ↓
用户输入密码
    ↓
可选：勾选"记住工号"
    ↓
点击"登录"按钮
    ↓
验证表单
    ↓
├─ 验证失败 → 显示错误提示
└─ 验证成功 → 调用登录接口
                ↓
        ├─ 登录失败 → 显示错误提示
        └─ 登录成功 → 跳转主页
```

### 4.2 状态管理

#### 登录状态
- `loggingIn: false` - 空闲状态
- `loggingIn: true` - 登录中（禁用输入，显示加载动画）

#### 表单状态
- `userField.text` - 用户名
- `passField.text` - 密码
- `rememberUsername: false` - 记住用户名开关

#### 错误状态
- `loginError: ""` - 无错误
- `loginError: "错误信息"` - 显示错误

### 4.3 键盘快捷键

- `Enter`：在密码输入框按Enter，触发登录
- `Tab`：在用户名和密码输入框之间切换
- `Escape`：清除错误提示

### 4.4 加载状态

#### 登录按钮
- 空闲：显示"登录"
- 登录中：显示"登录中..." + 旋转动画
- 禁用：按钮变灰，不可点击

#### 输入框
- 登录中：禁用输入
- 空闲：可输入

---

## 5. 技术实现

### 5.1 文件结构

```
ui/
├── LoginPage.qml          # 登录页面主文件
├── theme/
│   └── Theme.qml          # 主题配置（需更新颜色）
└── components/
    └── (现有组件)
```

### 5.2 代码结构

**LoginPage.qml 主要部分**：

```qml
Item {
    id: root
    
    // 属性
    required property var sessionManager
    property bool loggingIn: false
    property string loginError: ""
    property bool passwordVisible: false
    property bool rememberUsername: false
    
    // 背景
    Rectangle { ... }
    
    // 顶部按钮
    Row { ... }
    
    // 登录卡片
    Item {
        id: loginBlock
        Rectangle {
            id: cardBg
            ColumnLayout {
                // 标题区
                // 表单区
                // 记住用户名复选框
                // 错误提示
                // 按钮区
            }
        }
    }
    
    // 逻辑函数
    function _resetForm() { ... }
    function _submitLogin() { ... }
    function _loadSavedUsername() { ... }
    function _saveUsername() { ... }
    function _clearSavedUsername() { ... }
}
```

### 5.3 关键代码片段

#### 记住用户名功能

```qml
// 加载保存的用户名
function _loadSavedUsername() {
    var settings = Qt.createQmlObject('import QtCore; QtCore.Settings {}', root)
    var saved = settings.value("rememberedUsername", "")
    if (saved) {
        userField.text = saved
        rememberCheckbox.checked = true
    }
}

// 保存用户名
function _saveUsername() {
    if (rememberCheckbox.checked) {
        var settings = Qt.createQmlObject('import QtCore; QtCore.Settings {}', root)
        settings.setValue("rememberedUsername", userField.text.trim())
    } else {
        _clearSavedUsername()
    }
}

// 清除保存的用户名
function _clearSavedUsername() {
    var settings = Qt.createQmlObject('import QtCore; QtCore.Settings {}', root)
    settings.remove("rememberedUsername")
}
```

#### 登录按钮状态

```qml
Rectangle {
    id: loginBtn
    enabled: !root.loggingIn
           && userField.text.trim().length > 0
           && passField.text.length > 0
    opacity: enabled ? 1.0 : 0.5
    
    gradient: Gradient {
        GradientStop {
            position: 0.0;
            color: loginBtnMouse.containsMouse ? "#3B82F6" : "#2563EB"
        }
        GradientStop {
            position: 1.0;
            color: loginBtnMouse.containsMouse ? "#2563EB" : "#1D4ED8"
        }
    }
}
```

### 5.4 依赖项

- Qt Quick.Controls
- Qt Quick.Layouts
- QtCore (用于 QSettings)

---

## 6. 测试和验收

### 6.1 功能测试

#### 记住用户名功能
- [ ] 勾选"记住工号"后登录，重启应用用户名自动填充
- [ ] 取消勾选"记住工号"后登录，重启应用用户名为空
- [ ] 清空用户名后登录，重启应用用户名为空

#### 密码可见性切换
- [ ] 点击眼睛图标，密码显示为明文
- [ ] 再次点击，密码隐藏为圆点
- [ ] 登录过程中切换状态不生效

#### 表单验证
- [ ] 用户名为空时，登录按钮禁用
- [ ] 密码为空时，登录按钮禁用
- [ ] 两者都为空时，登录按钮禁用

#### 登录流程
- [ ] 输入正确用户名密码，登录成功
- [ ] 输入错误密码，显示错误提示
- [ ] 网络断开时，显示"未连接服务器"
- [ ] 登录中显示加载动画，禁用输入

### 6.2 视觉测试

#### 布局
- [ ] 窗口最小尺寸（1024×700）下布局正常
- [ ] 窗口最大化时布局居中
- [ ] 不同DPI下文字清晰

#### 颜色
- [ ] 背景渐变正常显示
- [ ] 卡片阴影效果正常
- [ ] 按钮悬停颜色变化

#### 动画
- [ ] 页面加载卡片滑入动画
- [ ] 输入框聚焦边框颜色变化
- [ ] 错误提示淡入动画

### 6.3 兼容性测试

- [ ] Windows 10 正常运行
- [ ] Windows 11 正常运行
- [ ] 高DPI显示器下正常显示

### 6.4 性能测试

- [ ] 页面加载时间 < 500ms
- [ ] 登录响应时间 < 2s
- [ ] 动画流畅无卡顿

### 6.5 验收标准

#### 必须满足
1. 记住用户名功能正常工作
2. 密码可见性切换正常
3. 表单验证完整
4. 视觉设计符合简约商务风格
5. 所有测试用例通过

#### 应该满足
1. 动画效果流畅
2. 错误提示清晰
3. 键盘快捷键可用

---

## 7. 实现计划

### 7.1 任务分解

1. **更新Theme.qml颜色方案**
   - 添加新的颜色定义
   - 更新现有颜色值

2. **实现记住用户名功能**
   - 添加QSettings读写逻辑
   - 添加复选框UI
   - 在登录流程中集成保存/加载逻辑

3. **优化视觉设计**
   - 更新背景渐变
   - 调整卡片样式
   - 优化按钮和输入框样式

4. **完善交互细节**
   - 添加键盘快捷键
   - 优化动画效果
   - 完善错误提示

5. **测试和调试**
   - 功能测试
   - 视觉测试
   - 兼容性测试

### 7.2 预计时间

- 设计确认：已完成
- 代码实现：2-3小时
- 测试调试：1-2小时
- 总计：3-5小时

---

## 8. 风险和注意事项

### 8.1 技术风险

- **QSettings兼容性**：确保在不同Windows版本上正常工作
- **动画性能**：避免复杂动画影响性能
- **DPI适配**：确保高DPI显示器下正常显示

### 8.2 用户体验风险

- **学习成本**：新界面需要用户适应
- **功能变化**：记住用户名功能变化可能影响用户习惯

### 8.3 缓解措施

- 提供清晰的视觉引导
- 保持核心交互逻辑不变
- 充分测试各种场景

---

## 9. 附录

### 9.1 参考文档

- AttendanceAdmin UI 重新设计需求文档
- Theme.qml 设计系统
- Qt 6 QML 文档

### 9.2 变更记录

| 日期 | 版本 | 变更内容 |
|------|------|----------|
| 2026-06-18 | v1.0 | 初始版本 |

---

**文档状态**：已批准
**下一步**：开始实现
