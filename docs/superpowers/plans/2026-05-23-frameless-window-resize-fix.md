# 无边框窗口拉伸修复 实现计划

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** 修复无边框窗口边缘拖拽拉伸不生效的 bug

**Architecture:** 仅修改 `ui/Main.qml` 一处文件：(1) 从 `_applyWindowMode()` 中移除 `flags` 赋值，防止运行时重建 HWND 导致 C++ 侧 WndProc 失效；(2) 改进 `clampSize()` 使其同时响应宽/高变化，拖拽任意边缘都能维持固定宽高比

**Tech Stack:** Qt 6.8 QML, C++ (Win32 API)

---

### Task 1: 修复 Main.qml

**Files:**
- Modify: `ui/Main.qml:57-69` (clampSize + onWidthChanged)
- Modify: `ui/Main.qml:137-155` (_applyWindowMode)

- [ ] **Step 1: 替换 clampSize() 及相关属性**

删除旧的 `clampSize()`、`onWidthChanged` 和 `_adjustingSize`，替换为追踪前值的双向版本。

旧代码 (`ui/Main.qml` 约第 59-69 行):
```qml
    readonly property real aspectRatio: 1480.0 / 920.0
    property bool _adjustingSize: false

    function clampSize() {
        if (_adjustingSize || !sessionManager.isLoggedIn) return
        _adjustingSize = true
        height = Math.round(width / aspectRatio)
        _adjustingSize = false
    }

    onWidthChanged: clampSize()
```

新代码:
```qml
    readonly property real aspectRatio: 1480.0 / 920.0
    property real _prevW: 0
    property real _prevH: 0

    function clampSize() {
        if (!sessionManager.isLoggedIn) return
        if (width !== _prevW) {
            height = Math.round(width / aspectRatio)
        } else if (height !== _prevH) {
            width = Math.round(height * aspectRatio)
        }
        _prevW = width
        _prevH = height
    }

    onWidthChanged: clampSize()
    onHeightChanged: clampSize()
```

- [ ] **Step 2: 从 _applyWindowMode() 移除 flags 赋值**

旧代码 (`ui/Main.qml` 第 137-155 行):
```qml
    function _applyWindowMode() {
        if (sessionManager.isLoggedIn) {
            minimumWidth = 1100
            minimumHeight = 700
            maximumWidth = 16777215
            maximumHeight = 16777215
            width = mainWindowWidth
            height = mainWindowHeight
            flags = Qt.Window | Qt.FramelessWindowHint
        } else {
            minimumWidth = loginWindowWidth
            minimumHeight = loginWindowHeight
            maximumWidth = loginWindowWidth
            maximumHeight = loginWindowHeight
            width = loginWindowWidth
            height = loginWindowHeight
            flags = Qt.Window | Qt.FramelessWindowHint
        }
    }
```

新代码:
```qml
    function _applyWindowMode() {
        if (sessionManager.isLoggedIn) {
            minimumWidth = 1100
            minimumHeight = 700
            maximumWidth = 16777215
            maximumHeight = 16777215
            width = mainWindowWidth
            height = mainWindowHeight
        } else {
            minimumWidth = loginWindowWidth
            minimumHeight = loginWindowHeight
            maximumWidth = loginWindowWidth
            maximumHeight = loginWindowHeight
            width = loginWindowWidth
            height = loginWindowHeight
        }
    }
```

- [ ] **Step 3: 构建验证**

```bash
cmake --build out/build/debug --target appAttendanceAdmin
```
预期: 编译成功，无错误。

- [ ] **Step 4: 提交**

```bash
git add ui/Main.qml
git commit -m "fix: prevent HWND invalidation and add bidirectional aspect ratio clamping

Remove redundant flags assignment in _applyWindowMode() that could
trigger HWND recreation and invalidate the WndProc installed by
installFramelessResize. Replace _adjustingSize guard with previous-value
tracking so clampSize() correctly maintains aspect ratio when either
width or height changes.

Co-Authored-By: Claude Opus 4.7 <noreply@anthropic.com>"
```
