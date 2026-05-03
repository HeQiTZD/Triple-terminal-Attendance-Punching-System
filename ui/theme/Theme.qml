pragma Singleton

import QtQuick

QtObject {
    // ===== 色板（深色 IDE 风格） =====
    readonly property color bg:           "#1E1F22"   // 主背景
    readonly property color surface:      "#2B2D30"   // 卡片/面板
    readonly property color surfaceAlt:   "#26282B"   // 行交错背景
    readonly property color sideBar:      "#212328"   // 侧边栏背景
    readonly property color border:       "#3C3F41"   // 分隔线
    readonly property color borderStrong: "#4C5052"

    readonly property color text:         "#EDEDED"
    readonly property color textMuted:    "#9DA0A6"
    readonly property color textSubtle:   "#6F7176"

    readonly property color accent:       "#2D7FF9"
    readonly property color accentHover:  "#3D8BFB"
    readonly property color accentSubtle: "#1F4A8A"

    readonly property color success:      "#3FB950"
    readonly property color warning:      "#D29922"
    readonly property color danger:       "#F85149"
    readonly property color info:         "#58A6FF"

    readonly property color hover:        "#34373B"
    readonly property color selected:     "#2D4F78"
    readonly property color highlight:    "#FFD66B"

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

    // ===== 通用尺寸 =====
    readonly property int sideBarWidth: 220
    readonly property int statusBarHeight: 36
    readonly property int rowHeight: 30
    readonly property int controlHeight: 30

    // ===== 工具函数 =====
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
