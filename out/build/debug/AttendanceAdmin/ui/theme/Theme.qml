pragma Singleton

import QtQuick

QtObject {
    // ===== 企业蓝色系配色 =====
    readonly property color primary:         "#1D6BFF"
    readonly property color primaryHover:   "#3B82FF"
    readonly property color primarySubtle:   "#DBEAFE"

    // ===== 背景色板 =====
    readonly property color bg:           "#F2F4F7"
    readonly property color surface:      "#FFFFFF"
    readonly property color surfaceAlt:   "#EEF1F6"
    readonly property color sideBar:      "#E8EBF1"
    readonly property color border:       "#D8DDE3"
    readonly property color borderStrong: "#BFC6D2"

    // ===== 文字色板 =====
    readonly property color text:         "#1F2937"
    readonly property color textMuted:    "#3D4551"
    readonly property color textSubtle:   "#6B7280"

    // ===== 主色 =====
    readonly property color accent:       "#1D6BFF"
    readonly property color accentHover:  "#3B82FF"
    readonly property color accentSubtle: "#DBEAFE"

    // ===== 状态色 =====
    readonly property color success:      "#0D8A4C"
    readonly property color warning:      "#9A3E04"
    readonly property color danger:       "#C62828"
    readonly property color info:         "#1565C0"

    // ===== 图表色 =====
    readonly property color pieNormal:    "#16a34a"
    readonly property color pieLate:      "#f97316"
    readonly property color pieEarly:     "#8b5cf6"
    readonly property color pieAbsent:    "#ef4444"

    // ===== 交互状态色 =====
    readonly property color hover:        "#CBD5E1"
    readonly property color selected:     "#C7DFFF"
    readonly property color highlight:    "#92400E"

    // ===== 阴影色 =====
    readonly property color shadowLight:  Qt.rgba(0, 0, 0, 0.06)
    readonly property color shadowMedium: Qt.rgba(0, 0, 0, 0.12)
    readonly property color shadowStrong: Qt.rgba(0, 0, 0, 0.20)

    // ===== 间距系统 =====
    readonly property int spacingXs: 4
    readonly property int spacingSm: 6
    readonly property int spacingMd: 10
    readonly property int spacingLg: 16
    readonly property int spacingXl: 24

    // ===== 圆角系统 =====
    readonly property int radiusSm: 4
    readonly property int radiusMd: 6
    readonly property int radiusLg: 10

    // ===== 字号系统 =====
    readonly property int fontXs: 11
    readonly property int fontSm: 12
    readonly property int fontMd: 13
    readonly property int fontLg: 15
    readonly property int fontXl: 18
    readonly property int fontXxl: 22

    // ===== 字体族 =====
    readonly property string fontFamily: "'Segoe UI', 'Microsoft YaHei UI', 'PingFang SC', sans-serif"
    readonly property string fontMono:   "Consolas, 'Cascadia Mono', monospace"

    // ===== 布局尺寸 =====
    readonly property int sideBarWidth: 220
    readonly property int statusBarHeight: 36
    readonly property int rowHeight: 30
    readonly property int controlHeight: 30

    // ===== 图表色数组 =====
    readonly property var chartColors: [
        "#1D6BFF",
        "#0D8A4C",
        "#f97316",
        "#8b5cf6",
        "#ef4444",
        "#06b6d4",
        "#84cc16",
        "#f59e0b"
    ]

    // ===== 状态颜色映射函数 =====
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

    // ===== 考勤饼图颜色映射 =====
    function attendancePieColor(status) {
        const s = String(status || "").toLowerCase()
        if (s === "normal" || s === "manual")
            return pieNormal
        if (s === "late")
            return pieLate
        if (s === "early")
            return pieEarly
        if (s === "absent")
            return pieAbsent
        return textMuted
    }

    // ===== 格式化考勤状态 =====
    function formatAttendanceStatus(code) {
        const s = String(code || "").toLowerCase()
        switch (s) {
        case "normal": return qsTr("正常")
        case "late": return qsTr("迟到")
        case "early": return qsTr("早退")
        case "absent": return qsTr("缺勤")
        case "manual": return qsTr("补签")
        default:
            if (!code)
                return "—"
            return String(code)
        }
    }

    // ===== 获取图表颜色 =====
    function chartColor(index) {
        return chartColors[index % chartColors.length]
    }

    // ===== 暗色模式色板（远期启用） =====
    readonly property color darkBg:           "#0F172A"
    readonly property color darkSurface:      "#1E293B"
    readonly property color darkSurfaceAlt:   "#273449"
    readonly property color darkSideBar:      "#162032"
    readonly property color darkBorder:       "#334155"
    readonly property color darkBorderStrong: "#475569"

    readonly property color darkText:         "#F1F5F9"
    readonly property color darkTextMuted:    "#94A3B8"
    readonly property color darkTextSubtle:   "#64748B"

    readonly property color darkHover:        "#334155"
    readonly property color darkSelected:     "#1E3A5F"

    readonly property color darkShadowLight:  Qt.rgba(0, 0, 0, 0.30)
    readonly property color darkShadowMedium: Qt.rgba(0, 0, 0, 0.45)
    readonly property color darkShadowStrong: Qt.rgba(0, 0, 0, 0.60)

    readonly property var darkChartColors: [
        "#3B82F6",
        "#22C55E",
        "#F97316",
        "#A78BFA",
        "#EF4444",
        "#06B6D4",
        "#84CC16",
        "#F59E0B"
    ]
}
