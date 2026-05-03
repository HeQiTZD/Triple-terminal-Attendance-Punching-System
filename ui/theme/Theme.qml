pragma Singleton

import QtQuick

QtObject {
    // ===== 色板（浅色 · 不受系统暗黑影响，与 Fusion + ColorScheme.Light 配套） =====
    readonly property color bg:           "#F2F4F7"   // 主背景
    readonly property color surface:      "#FFFFFF"   // 卡片/面板
    readonly property color surfaceAlt:   "#EEF1F6"   // 行交错 / 条带
    readonly property color sideBar:      "#E8EBF1"   // 侧边栏
    readonly property color border:       "#D8DDE3"
    readonly property color borderStrong: "#BFC6D2"

    readonly property color text:         "#1F2937"
    readonly property color textMuted:    "#4B5563"
    readonly property color textSubtle:   "#6B7280"

    readonly property color accent:       "#1D6BFF"
    readonly property color accentHover:  "#3B82FF"
    readonly property color accentSubtle: "#DBEAFE"

    readonly property color success:      "#0D8A4C"
    readonly property color warning:      "#B45309"
    readonly property color danger:       "#C62828"
    readonly property color info:         "#1565C0"

    readonly property color hover:        "#DCE3EE"
    readonly property color selected:     "#C7DFFF"
    readonly property color highlight:    "#92400E"

    // ===== 间距 / 圆角 / 字号 =====
    readonly property int spacingXs: 4
    readonly property int spacingSm: 6
    readonly property int spacingMd: 10
    readonly property int spacingLg: 16
    readonly property int spacingXl: 24

    readonly property int radiusSm: 4
    readonly property int radiusMd: 6
    readonly property int radiusLg: 10

    readonly property int fontXs: 11
    readonly property int fontSm: 12
    readonly property int fontMd: 13
    readonly property int fontLg: 15
    readonly property int fontXl: 18
    readonly property int fontXxl: 22

    readonly property string fontFamily: "Microsoft YaHei UI"
    readonly property string fontMono:   "Consolas, 'Cascadia Mono', monospace"

    readonly property int sideBarWidth: 220
    readonly property int statusBarHeight: 36
    readonly property int rowHeight: 30
    readonly property int controlHeight: 30

    function statusColor(status) {
        const s = String(status || "").toLowerCase()
        if (s === "online" || s === "ok" || s === "running" || s === "success" || s === "pass" || s === "normal")
            return success
        if (s === "offline" || s === "stopped" || s === "fail" || s === "failed" || s === "error")
            return danger
        if (s === "maintenance" || s === "warn" || s === "warning" || s === "late" || s === "early")
            return warning
        if (s === "manual" || s === "info")
            return info
        return textMuted
    }
}
