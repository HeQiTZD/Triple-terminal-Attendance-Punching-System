# 无边框窗口拉伸修复

日期: 2026-05-23

## 问题

C++ 侧 `FramelessWndProc` (WM_NCHITTEST) 已实现，但窗口边缘拖拽拉伸不生效。

## 根因

1. **`_applyWindowMode()` 中不必要的 flags 赋值**: `Main.qml` 的 `_applyWindowMode()` 在 `Component.onCompleted` 以及登录/登出时执行 `flags = Qt.Window | Qt.FramelessWindowHint`。虽然值和初始声明相同，但 Qt 6 中 `setFlags()` 可能触发 Windows 重建 HWND，使 `main.cpp` 中通过旧 HWND 安装的 `FramelessWndProc` 失效。

2. **`clampSize()` 仅监听 width 变化**: `onWidthChanged: clampSize()` 只在水平拖拽时维持宽高比，垂直拖拽会破坏比例约束。

## 修复

### Main.qml

- 从 `_applyWindowMode()` 中移除 `flags` 赋值（初始声明已设置，登录/登出时 flags 不变，无需重设）
- `clampSize()` 改为同时响应 `onWidthChanged` 和 `onHeightChanged`，双向维持固定宽高比

## 影响范围

- `ui/Main.qml`: `_applyWindowMode()` 移除 flags 赋值，`clampSize` 双向监听
