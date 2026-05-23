#ifndef DESIGNTOKENS_H
#define DESIGNTOKENS_H

#include <QColor>
#include <QString>
#include <QHash>

namespace DesignTokens {

// ==================== Primitive 色板 ====================
namespace Primitive {
    // Grays
    inline constexpr const char* gray0   = "#ffffff";
    inline constexpr const char* gray50  = "#f8fafc";
    inline constexpr const char* gray100 = "#f1f5f9";
    inline constexpr const char* gray200 = "#e2e8f0";
    inline constexpr const char* gray300 = "#cbd5e1";
    inline constexpr const char* gray400 = "#94a3b8";
    inline constexpr const char* gray500 = "#64748b";
    inline constexpr const char* gray600 = "#475569";
    inline constexpr const char* gray700 = "#334155";
    inline constexpr const char* gray800 = "#1e293b";
    inline constexpr const char* gray900 = "#0f172a";
    inline constexpr const char* gray950 = "#020617";

    // Blues
    inline constexpr const char* blue400 = "#60a5fa";
    inline constexpr const char* blue500 = "#3b82f6";
    inline constexpr const char* blue600 = "#2563eb";
    inline constexpr const char* blue700 = "#1d4ed8";
    inline constexpr const char* blue800 = "#1e40af";

    // Semantics
    inline constexpr const char* green600 = "#16a34a";
    inline constexpr const char* amber600 = "#d97706";
    inline constexpr const char* red600   = "#dc2626";
    inline constexpr const char* sky600   = "#0284c7";
}

// ==================== Semantic 令牌名常量 ====================
namespace Semantic {
    // Backgrounds
    inline constexpr const char* bgApp      = "bg.app";
    inline constexpr const char* bgSurface  = "bg.surface";
    inline constexpr const char* bgElevated = "bg.elevated";
    inline constexpr const char* bgOverlay  = "bg.overlay";

    // Text
    inline constexpr const char* textPrimary   = "text.primary";
    inline constexpr const char* textSecondary = "text.secondary";
    inline constexpr const char* textTertiary  = "text.tertiary";
    inline constexpr const char* textDisabled  = "text.disabled";

    // Brand
    inline constexpr const char* brandPrimary       = "brand.primary";
    inline constexpr const char* brandPrimaryHover  = "brand.primary.hover";
    inline constexpr const char* brandPrimaryActive = "brand.primary.active";

    // Semantic Colors
    inline constexpr const char* semSuccess = "semantic.success";
    inline constexpr const char* semWarning = "semantic.warning";
    inline constexpr const char* semDanger  = "semantic.danger";
    inline constexpr const char* semInfo    = "semantic.info";

    // Borders
    inline constexpr const char* borderDefault  = "border.default";
    inline constexpr const char* borderSubtle   = "border.subtle";
    inline constexpr const char* borderEmphasis = "border.emphasis";
}

// ==================== 字体族名常量 ====================
namespace FontFamily {
    inline constexpr const char* mono = "\"Consolas\", \"SF Mono\", monospace";
    inline constexpr const char* ui   = "\"Microsoft YaHei\", \"PingFang SC\", sans-serif";
    inline constexpr const char* en   = "\"Segoe UI\", \"Inter\", sans-serif";
}

// ==================== 字号 px 值 ====================
namespace FontSize {
    inline constexpr int xs   = 11;
    inline constexpr int sm   = 13;
    inline constexpr int base = 14;
    inline constexpr int lg   = 16;
    inline constexpr int xl   = 20;
    inline constexpr int xxl  = 24;
    inline constexpr int xxxl = 32;
}

// ==================== 字重 ====================
namespace FontWeight {
    inline constexpr int normal    = 400;
    inline constexpr int medium    = 500;
    inline constexpr int semibold  = 600;
    inline constexpr int bold      = 700;
}

// ==================== 间距 px 值 (index → px) ====================
namespace Spacing {
    inline constexpr int s1  = 4;
    inline constexpr int s2  = 8;
    inline constexpr int s3  = 12;
    inline constexpr int s4  = 16;
    inline constexpr int s5  = 20;
    inline constexpr int s6  = 24;
    inline constexpr int s8  = 32;

    // space(level) helper: 1→4, 2→8, 3→12, 4→16, 5→20, 6→24, 8→32
    inline constexpr int level(int n) {
        switch (n) {
            case 1: return s1;
            case 2: return s2;
            case 3: return s3;
            case 4: return s4;
            case 5: return s5;
            case 6: return s6;
            case 8: return s8;
            default: return s2;
        }
    }
}

// ==================== 圆角 px 值 ====================
namespace BorderRadius {
    inline constexpr int sm = 4;
    inline constexpr int md = 6;
    inline constexpr int lg = 8;
    inline constexpr int xl = 12;

    inline int fromName(const QString& name) {
        if (name == "sm") return sm;
        if (name == "md") return md;
        if (name == "lg") return lg;
        if (name == "xl") return xl;
        return md;
    }
}

} // namespace DesignTokens

#endif // DESIGNTOKENS_H
