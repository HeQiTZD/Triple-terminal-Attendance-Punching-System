# UI 系统 Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** 为 AttendanceSystem 构建设计令牌体系 + 可复用 Qt Widget 组件库，并改造 MainWindow/SetWindow 使用新组件。

**Architecture:** ThemeManager 单例管理三级令牌 (Primitive → Semantic → Component)，组件子类化 Qt 原生控件并在构造函数中从 ThemeManager 获取令牌应用样式。QSS 退居全局补充角色。保留 Fusion + 深色 Palette 作为原生控件回退层。

**Tech Stack:** Qt 6.10.2 C++ Widgets, Fusion Style, QSS, CMake, MinGW 64-bit

**Spec:** `docs/superpowers/specs/2026-05-23-ui-system-design.md`

---

### Task 1: DesignTokens.h — 原始色板与语义令牌定义

**Files:**
- Create: `UI/Theme/DesignTokens.h`

- [ ] **Step 1: 创建 DesignTokens.h**

```cpp
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
```

- [ ] **Step 2: 验证编译**

Run: `cmake --build build/Desktop_Qt_6_10_2_MinGW_64_bit-Debug --target AttendanceSystem`
Expected: 编译通过 (DesignTokens.h 是 header-only，不影响链接)

- [ ] **Step 3: Commit**

```bash
git add UI/Theme/DesignTokens.h
git commit -m "feat(ui): add DesignTokens.h with primitive colors, semantic tokens, and typography/spacing constants"
```

---

### Task 2: ThemeManager — 令牌管理器单例

**Files:**
- Create: `UI/Theme/ThemeManager.h`
- Create: `UI/Theme/ThemeManager.cpp`

- [ ] **Step 1: 创建 ThemeManager.h**

```cpp
#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QObject>
#include <QColor>
#include <QFont>
#include <QHash>
#include <QString>

class ThemeManager : public QObject
{
    Q_OBJECT

public:
    static ThemeManager* instance();

    /// 初始化令牌映射表，必须在 QApplication 创建后调用
    void initialize();

    /// 按路径获取颜色，如 "bg.surface", "brand.primary"
    QColor color(const QString& tokenPath) const;
    QString colorHex(const QString& tokenPath) const;

    /// 构建字体: font("text-lg", 600) → 16px semibold
    QFont font(const QString& sizeKey, int weight = 400) const;

    /// 间距: space(4) → 16px
    int space(int level) const;

    /// 圆角: radius("md") → 6px
    int radius(const QString& size) const;

    /// 工具方法
    static QColor alpha(const QColor& c, float a);
    static QColor lighten(const QColor& c, float amount);
    static QColor darken(const QColor& c, float amount);

    /// 加载补充 QSS
    void loadSupplementalQss();

signals:
    void themeChanged();

private:
    ThemeManager();
    QHash<QString, QString> m_colorMap;
    bool m_initialized = false;
};

#endif // THEMEMANAGER_H
```

- [ ] **Step 2: 创建 ThemeManager.cpp**

```cpp
#include "ThemeManager.h"
#include "DesignTokens.h"
#include <QApplication>
#include <QFile>

ThemeManager* ThemeManager::instance()
{
    static ThemeManager inst;
    return &inst;
}

ThemeManager::ThemeManager() {}

void ThemeManager::initialize()
{
    using namespace DesignTokens::Primitive;
    using namespace DesignTokens::Semantic;

    // -- 背景 --
    m_colorMap[bgApp]      = gray950;
    m_colorMap[bgSurface]  = gray900;
    m_colorMap[bgElevated] = gray800;
    m_colorMap[bgOverlay]  = gray700;

    // -- 文字 --
    m_colorMap[textPrimary]   = gray50;
    m_colorMap[textSecondary] = gray300;
    m_colorMap[textTertiary]  = gray400;
    m_colorMap[textDisabled]  = gray500;

    // -- 品牌 --
    m_colorMap[brandPrimary]       = blue600;
    m_colorMap[brandPrimaryHover]  = blue500;
    m_colorMap[brandPrimaryActive] = blue700;

    // -- 语义色 --
    m_colorMap[semSuccess] = green600;
    m_colorMap[semWarning] = amber600;
    m_colorMap[semDanger]  = red600;
    m_colorMap[semInfo]    = sky600;

    // -- 边框 --
    m_colorMap[borderDefault]  = gray700;
    m_colorMap[borderSubtle]   = gray800;
    m_colorMap[borderEmphasis] = gray500;

    m_initialized = true;
}

QColor ThemeManager::color(const QString& tokenPath) const
{
    return QColor(m_colorMap.value(tokenPath, "#ffffff"));
}

QString ThemeManager::colorHex(const QString& tokenPath) const
{
    return m_colorMap.value(tokenPath, "#ffffff");
}

QFont ThemeManager::font(const QString& sizeKey, int weight) const
{
    int px = DesignTokens::FontSize::base; // default 14px
    if (sizeKey == "xs")   px = DesignTokens::FontSize::xs;
    else if (sizeKey == "sm")   px = DesignTokens::FontSize::sm;
    else if (sizeKey == "base") px = DesignTokens::FontSize::base;
    else if (sizeKey == "lg")   px = DesignTokens::FontSize::lg;
    else if (sizeKey == "xl")   px = DesignTokens::FontSize::xl;
    else if (sizeKey == "2xl")  px = DesignTokens::FontSize::xxl;
    else if (sizeKey == "3xl")  px = DesignTokens::FontSize::xxxl;

    QFont f;
    f.setPixelSize(px);
    f.setWeight(weight);
    return f;
}

int ThemeManager::space(int level) const
{
    return DesignTokens::Spacing::level(level);
}

int ThemeManager::radius(const QString& size) const
{
    return DesignTokens::BorderRadius::fromName(size);
}

QColor ThemeManager::alpha(const QColor& c, float a)
{
    int r, g, b;
    c.getRgb(&r, &g, &b);
    return QColor(r, g, b, static_cast<int>(a * 255));
}

QColor ThemeManager::lighten(const QColor& c, float amount)
{
    return c.lighter(static_cast<int>(100 + amount * 100));
}

QColor ThemeManager::darken(const QColor& c, float amount)
{
    return c.darker(static_cast<int>(100 + amount * 100));
}

void ThemeManager::loadSupplementalQss()
{
    QFile f(":/qss/theme.qss");
    if (f.open(QFile::ReadOnly)) {
        qApp->setStyleSheet(qApp->styleSheet() + QString::fromLatin1(f.readAll()));
        f.close();
    }
}
```

- [ ] **Step 3: 验证编译**

Run: `cmake --build build/Desktop_Qt_6_10_2_MinGW_64_bit-Debug --target AttendanceSystem`
Expected: 编译通过

- [ ] **Step 4: Commit**

```bash
git add UI/Theme/ThemeManager.h UI/Theme/ThemeManager.cpp
git commit -m "feat(ui): add ThemeManager singleton with token resolution API"
```

---

### Task 3: ThemeManager.qss — 补充全局 QSS

**Files:**
- Create: `resources/qss/theme.qss`

- [ ] **Step 1: 创建 theme.qss**

```css
/* ============================================================
   ThemeManager 补充 QSS — 仅处理组件 API 不便覆盖的全局样式
   ============================================================ */

/* ---------- 全局滚动条 ---------- */
QScrollBar:vertical {
    background-color: #0f172a;
    width: 8px;
    border-radius: 4px;
    margin: 0px;
}
QScrollBar::handle:vertical {
    background-color: #334155;
    border-radius: 4px;
    min-height: 30px;
}
QScrollBar::handle:vertical:hover {
    background-color: #475569;
}
QScrollBar::add-line:vertical,
QScrollBar::sub-line:vertical {
    height: 0px;
}
QScrollBar::add-page:vertical,
QScrollBar::sub-page:vertical {
    background: none;
}

QScrollBar:horizontal {
    background-color: #0f172a;
    height: 8px;
    border-radius: 4px;
    margin: 0px;
}
QScrollBar::handle:horizontal {
    background-color: #334155;
    border-radius: 4px;
    min-width: 30px;
}
QScrollBar::handle:horizontal:hover {
    background-color: #475569;
}
QScrollBar::add-line:horizontal,
QScrollBar::sub-line:horizontal {
    width: 0px;
}
QScrollBar::add-page:horizontal,
QScrollBar::sub-page:horizontal {
    background: none;
}

/* ---------- Tooltip ---------- */
QToolTip {
    background-color: #1e293b;
    color: #f8fafc;
    border: 1px solid #334155;
    border-radius: 6px;
    padding: 6px 10px;
    font-size: 12px;
}

/* ---------- 焦点轮廓 ---------- */
QPushButton:focus-visible {
    outline: 2px solid #2563eb;
    outline-offset: 1px;
}

/* ---------- 菜单 ---------- */
QMenu {
    background-color: #0f172a;
    color: #f8fafc;
    border: 1px solid #334155;
    border-radius: 6px;
    padding: 4px 0;
}
QMenu::item {
    padding: 6px 24px;
}
QMenu::item:selected {
    background-color: #1e293b;
}
```

- [ ] **Step 2: 注册到 resources.qrc**

Read `resources/resources.qrc`, then add `<file alias="qss/theme.qss">qss/theme.qss</file>` inside `<qresource>`.

- [ ] **Step 3: 验证编译**

Run: `cmake --build build/Desktop_Qt_6_10_2_MinGW_64_bit-Debug --target AttendanceSystem`
Expected: 编译通过 (qrc 正确打包)

- [ ] **Step 4: Commit**

```bash
git add resources/qss/theme.qss resources/resources.qrc
git commit -m "feat(ui): add supplemental global QSS for scrollbars, tooltips, menus"
```

---

### Task 4: 集成 ThemeManager 到 main.cpp 和 CMakeLists.txt

**Files:**
- Modify: `main.cpp`
- Modify: `CMakeLists.txt`

- [ ] **Step 1: 修改 main.cpp**

在 `main.cpp` 中，Fusion palette 设置之后、QSS 加载之前，添加 ThemeManager 初始化：

```cpp
// 在 "a.setPalette(palette);" 之后、"QFile styleFile(...)" 之前 添加:

    // 初始化 UI 令牌系统
    ThemeManager::instance()->initialize();
    ThemeManager::instance()->loadSupplementalQss();
```

并在文件头部添加 include:
```cpp
#include "UI/Theme/ThemeManager.h"
```

- [ ] **Step 2: 修改 CMakeLists.txt**

在 `qt_add_executable(AttendanceSystem` 块中添加新文件:

```
        UI/Theme/ThemeManager.h UI/Theme/ThemeManager.cpp
```

- [ ] **Step 3: 验证编译**

Run: `cmake --build build/Desktop_Qt_6_10_2_MinGW_64_bit-Debug --target AttendanceSystem`
Expected: 编译通过

- [ ] **Step 4: Commit**

```bash
git add main.cpp CMakeLists.txt
git commit -m "feat(ui): integrate ThemeManager initialization into main.cpp and CMakeLists"
```

---

### Task 5: ActionButton — 操作按钮组件

**Files:**
- Create: `UI/Components/ActionButton.h`
- Create: `UI/Components/ActionButton.cpp`

- [ ] **Step 1: 创建 ActionButton.h**

```cpp
#ifndef ACTIONBUTTON_H
#define ACTIONBUTTON_H

#include <QPushButton>

class ActionButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(Variant variant READ variant WRITE setVariant)

public:
    enum Variant { Primary, Secondary, Danger };
    Q_ENUM(Variant)

    explicit ActionButton(const QString& text = {},
                          Variant v = Primary,
                          QWidget* parent = nullptr);

    Variant variant() const;
    void setVariant(Variant v);
    void setBusy(bool busy);

private:
    void applyStyle();
    Variant m_variant = Primary;
    bool m_busy = false;
    QString m_originalText;
};

#endif // ACTIONBUTTON_H
```

- [ ] **Step 2: 创建 ActionButton.cpp**

```cpp
#include "ActionButton.h"
#include "UI/Theme/ThemeManager.h"
#include "UI/Theme/DesignTokens.h"

ActionButton::ActionButton(const QString& text, Variant v, QWidget* parent)
    : QPushButton(text, parent)
    , m_variant(v)
    , m_originalText(text)
{
    setCursor(Qt::PointingHandCursor);
    setMinimumSize(80, 32);
    applyStyle();
}

ActionButton::Variant ActionButton::variant() const { return m_variant; }

void ActionButton::setVariant(Variant v)
{
    m_variant = v;
    applyStyle();
}

void ActionButton::setBusy(bool busy)
{
    m_busy = busy;
    setEnabled(!busy);
    if (busy) {
        if (m_originalText.isEmpty()) m_originalText = text();
        setText("...");
    } else {
        setText(m_originalText);
    }
}

void ActionButton::applyStyle()
{
    auto* tm = ThemeManager::instance();

    QColor bg, fg, border, hoverBg, activeBg;
    bool hasBorder = false;

    switch (m_variant) {
    case Primary:
        bg       = tm->color(DesignTokens::Semantic::brandPrimary);
        fg       = QColor("#ffffff");
        border   = Qt::transparent;
        hoverBg  = tm->color(DesignTokens::Semantic::brandPrimaryHover);
        activeBg = tm->color(DesignTokens::Semantic::brandPrimaryActive);
        break;
    case Secondary:
        bg       = tm->color(DesignTokens::Semantic::bgElevated);
        fg       = tm->color(DesignTokens::Semantic::textPrimary);
        border   = tm->color(DesignTokens::Semantic::borderDefault);
        hoverBg  = ThemeManager::lighten(bg, 0.10f);
        activeBg = ThemeManager::darken(bg, 0.10f);
        hasBorder = true;
        break;
    case Danger:
        bg       = Qt::transparent;
        fg       = tm->color(DesignTokens::Semantic::semDanger);
        border   = tm->color(DesignTokens::Semantic::semDanger);
        hoverBg  = ThemeManager::alpha(tm->color(DesignTokens::Semantic::semDanger), 0.15f);
        activeBg = ThemeManager::alpha(tm->color(DesignTokens::Semantic::semDanger), 0.25f);
        hasBorder = true;
        break;
    }

    int r = tm->radius("md");

    QString style = QString(
        "QPushButton {"
        "  background-color: %1;"
        "  color: %2;"
        "  border: %3;"
        "  border-radius: %4px;"
        "  padding: 6px 16px;"
        "  font-size: 13px;"
        "  font-weight: 600;"
        "}"
        "QPushButton:hover { background-color: %5; }"
        "QPushButton:pressed { background-color: %6; }"
        "QPushButton:disabled { opacity: 0.5; }"
    ).arg(bg.name())
     .arg(fg.name())
     .arg(hasBorder ? QString("1px solid %1").arg(border.name()) : QString("none"))
     .arg(r)
     .arg(hoverBg.name())
     .arg(activeBg.name());

    setStyleSheet(style);
}
```

- [ ] **Step 3: 更新 CMakeLists.txt**

在 `qt_add_executable` 块中添加:
```
        UI/Components/ActionButton.h UI/Components/ActionButton.cpp
```

- [ ] **Step 4: 验证编译**

Run: `cmake --build build/Desktop_Qt_6_10_2_MinGW_64_bit-Debug --target AttendanceSystem`
Expected: 编译通过

- [ ] **Step 5: Commit**

```bash
git add UI/Components/ActionButton.h UI/Components/ActionButton.cpp CMakeLists.txt
git commit -m "feat(ui): add ActionButton component with Primary/Secondary/Danger variants"
```

---

### Task 6: IconButton — 图标按钮组件

**Files:**
- Create: `UI/Components/IconButton.h`
- Create: `UI/Components/IconButton.cpp`

- [ ] **Step 1: 创建 IconButton.h**

```cpp
#ifndef ICONBUTTON_H
#define ICONBUTTON_H

#include <QPushButton>

class IconButton : public QPushButton
{
    Q_OBJECT
public:
    enum Role { Minimize, Maximize, Close };
    Q_ENUM(Role)

    explicit IconButton(Role role, QWidget* parent = nullptr);
    void reflectWindowState(bool isMaximized);

private:
    void applyStyle();
    Role m_role;
};

#endif // ICONBUTTON_H
```

- [ ] **Step 2: 创建 IconButton.cpp**

```cpp
#include "IconButton.h"
#include "UI/Theme/ThemeManager.h"
#include "UI/Theme/DesignTokens.h"

IconButton::IconButton(Role role, QWidget* parent)
    : QPushButton(parent)
    , m_role(role)
{
    setFixedSize(36, 36);
    setCursor(Qt::PointingHandCursor);

    switch (role) {
    case Minimize: setText(QStringLiteral("−")); break;   // −
    case Maximize: setText(QStringLiteral("⬜")); break;   // ⬜
    case Close:    setText(QStringLiteral("✕")); break;   // ✕
    }
    applyStyle();
}

void IconButton::reflectWindowState(bool isMaximized)
{
    if (m_role == Maximize) {
        setText(isMaximized ? QStringLiteral("⬝")   // ⬝
                            : QStringLiteral("⬜")); // ⬜
    }
}

void IconButton::applyStyle()
{
    auto* tm = ThemeManager::instance();
    int r = tm->radius("md");
    QString fg    = tm->colorHex(DesignTokens::Semantic::textTertiary);
    QString hoverBg;
    QString hoverFg = "#e6edf3";

    if (m_role == Close) {
        hoverBg = tm->colorHex(DesignTokens::Semantic::semDanger);
        hoverFg = "#ffffff";
    } else {
        hoverBg = "rgba(255,255,255,0.08)";
    }

    setStyleSheet(QString(
        "QPushButton {"
        "  background-color: transparent;"
        "  color: %1;"
        "  font-size: 16px;"
        "  font-weight: 700;"
        "  border: none;"
        "  border-radius: %2px;"
        "}"
        "QPushButton:hover { background-color: %3; color: %4; }"
    ).arg(fg).arg(r).arg(hoverBg).arg(hoverFg));
}
```

- [ ] **Step 3: 更新 CMakeLists.txt**

在 `qt_add_executable` 块中添加:
```
        UI/Components/IconButton.h UI/Components/IconButton.cpp
```

- [ ] **Step 4: 验证编译**

Run: `cmake --build build/Desktop_Qt_6_10_2_MinGW_64_bit-Debug --target AttendanceSystem`
Expected: 编译通过

- [ ] **Step 5: Commit**

```bash
git add UI/Components/IconButton.h UI/Components/IconButton.cpp CMakeLists.txt
git commit -m "feat(ui): add IconButton component for window control bar"
```

---

### Task 7: StatusIndicator — 状态指示灯组件

**Files:**
- Create: `UI/Components/StatusIndicator.h`
- Create: `UI/Components/StatusIndicator.cpp`

- [ ] **Step 1: 创建 StatusIndicator.h**

```cpp
#ifndef STATUSINDICATOR_H
#define STATUSINDICATOR_H

#include <QWidget>

class QLabel;

class StatusIndicator : public QWidget
{
    Q_OBJECT
public:
    enum State { Online, Offline, Warning };
    Q_ENUM(State)

    explicit StatusIndicator(QWidget* parent = nullptr);

    void setState(State s);
    State state() const;
    void setLabel(const QString& text);
    void setShowDot(bool show);

private:
    void applyStyle();
    State m_state = Offline;
    QLabel* m_dot = nullptr;
    QLabel* m_label = nullptr;
};

#endif // STATUSINDICATOR_H
```

- [ ] **Step 2: 创建 StatusIndicator.cpp**

```cpp
#include "StatusIndicator.h"
#include "UI/Theme/ThemeManager.h"
#include "UI/Theme/DesignTokens.h"
#include <QLabel>
#include <QHBoxLayout>

StatusIndicator::StatusIndicator(QWidget* parent)
    : QWidget(parent)
{
    auto* lay = new QHBoxLayout(this);
    lay->setContentsMargins(6, 6, 14, 6);
    lay->setSpacing(8);

    m_dot = new QLabel(this);
    m_dot->setFixedSize(8, 8);
    lay->addWidget(m_dot);

    m_label = new QLabel(this);
    m_label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    lay->addWidget(m_label);

    applyStyle();
}

void StatusIndicator::setState(State s)
{
    m_state = s;
    applyStyle();
}

StatusIndicator::State StatusIndicator::state() const { return m_state; }

void StatusIndicator::setLabel(const QString& text) { m_label->setText(text); }

void StatusIndicator::setShowDot(bool show) { m_dot->setVisible(show); }

void StatusIndicator::applyStyle()
{
    auto* tm = ThemeManager::instance();
    int r = tm->radius("md");
    QString dotColor, textColor, bg, border;

    switch (m_state) {
    case Online:
        dotColor  = tm->colorHex(DesignTokens::Semantic::semSuccess);
        textColor = dotColor;
        bg        = QString("rgba(22,163,74,0.10)");
        border    = QString("rgba(22,163,74,0.30)");
        break;
    case Offline:
        dotColor  = DesignTokens::Primitive::gray500;
        textColor = tm->colorHex(DesignTokens::Semantic::textTertiary);
        bg        = tm->colorHex(DesignTokens::Semantic::bgSurface);
        border    = tm->colorHex(DesignTokens::Semantic::borderDefault);
        break;
    case Warning:
        dotColor  = tm->colorHex(DesignTokens::Semantic::semWarning);
        textColor = dotColor;
        bg        = QString("rgba(217,119,6,0.10)");
        border    = QString("rgba(217,119,6,0.30)");
        break;
    }

    m_dot->setStyleSheet(QString(
        "background-color: %1; border-radius: 4px;"
    ).arg(dotColor));

    m_label->setStyleSheet(QString(
        "color: %1; font-size: 13px; font-weight: 600; background: transparent;"
    ).arg(textColor));

    setStyleSheet(QString(
        "StatusIndicator { background-color: %1; border: 1px solid %2; border-radius: %3px; }"
    ).arg(bg).arg(border).arg(r));
}
```

- [ ] **Step 3: 更新 CMakeLists.txt**

在 `qt_add_executable` 块中添加:
```
        UI/Components/StatusIndicator.h UI/Components/StatusIndicator.cpp
```

- [ ] **Step 4: 验证编译**

Run: `cmake --build build/Desktop_Qt_6_10_2_MinGW_64_bit-Debug --target AttendanceSystem`
Expected: 编译通过

- [ ] **Step 5: Commit**

```bash
git add UI/Components/StatusIndicator.h UI/Components/StatusIndicator.cpp CMakeLists.txt
git commit -m "feat(ui): add StatusIndicator component with Online/Offline/Warning states"
```

---

### Task 8: InfoField — 信息字段组件

**Files:**
- Create: `UI/Components/InfoField.h`
- Create: `UI/Components/InfoField.cpp`

- [ ] **Step 1: 创建 InfoField.h**

```cpp
#ifndef INFOFIELD_H
#define INFOFIELD_H

#include <QWidget>

class QLabel;

class InfoField : public QWidget
{
    Q_OBJECT
public:
    explicit InfoField(const QString& label = {},
                       QWidget* parent = nullptr);

    void setLabel(const QString& text);
    void setValue(const QString& text);
    void setValueColor(const QColor& c);
    QString value() const;

private:
    QLabel* m_label = nullptr;
    QLabel* m_value = nullptr;
};

#endif // INFOFIELD_H
```

- [ ] **Step 2: 创建 InfoField.cpp**

```cpp
#include "InfoField.h"
#include "UI/Theme/ThemeManager.h"
#include "UI/Theme/DesignTokens.h"
#include <QLabel>
#include <QHBoxLayout>

InfoField::InfoField(const QString& label, QWidget* parent)
    : QWidget(parent)
{
    auto* tm = ThemeManager::instance();

    auto* lay = new QHBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(tm->space(2)); // 8px

    m_label = new QLabel(label, this);
    m_label->setMinimumWidth(68);
    m_label->setFont(tm->font("lg", DesignTokens::FontWeight::bold));
    m_label->setStyleSheet(QString(
        "color: %1; background: transparent;"
    ).arg(tm->colorHex(DesignTokens::Semantic::textTertiary)));

    lay->addWidget(m_label);

    m_value = new QLabel(this);
    m_value->setFont(tm->font("xl", DesignTokens::FontWeight::bold));
    m_value->setStyleSheet(QString(
        "color: %1; background: transparent;"
    ).arg(tm->colorHex(DesignTokens::Semantic::textPrimary)));

    lay->addWidget(m_value);
}

void InfoField::setLabel(const QString& text) { m_label->setText(text); }
void InfoField::setValue(const QString& text) { m_value->setText(text); }
QString InfoField::value() const { return m_value->text(); }

void InfoField::setValueColor(const QColor& c)
{
    m_value->setStyleSheet(QString(
        "color: %1; background: transparent;"
    ).arg(c.name()));
}
```

- [ ] **Step 3: 更新 CMakeLists.txt**

在 `qt_add_executable` 块中添加:
```
        UI/Components/InfoField.h UI/Components/InfoField.cpp
```

- [ ] **Step 4: 验证编译**

Run: `cmake --build build/Desktop_Qt_6_10_2_MinGW_64_bit-Debug --target AttendanceSystem`
Expected: 编译通过

- [ ] **Step 5: Commit**

```bash
git add UI/Components/InfoField.h UI/Components/InfoField.cpp CMakeLists.txt
git commit -m "feat(ui): add InfoField component for label-value display pairs"
```

---

### Task 9: 改造 MainWindow — 控件替换

**Files:**
- Modify: `mainwindow.h`
- Modify: `mainwindow.cpp`
- Modify: `mainwindow.ui`

- [ ] **Step 1: 修改 mainwindow.h — 更新成员类型**

将:
```cpp
QWidget* m_VideoWidget = nullptr;
```
保留不变。

添加新 include 和成员变量 (在文件头部):
```cpp
#include "UI/Components/ActionButton.h"
#include "UI/Components/IconButton.h"
#include "UI/Components/StatusIndicator.h"
#include "UI/Components/InfoField.h"
```

**删除**以下槽函数声明 (不再需要 — 组件自管理样式):
```cpp
// 删除这两行:
// void onNetworkStateChanged(bool isOnline);
```

替换为:
```cpp
    // 更新网络状态显示 (通过 StatusIndicator 组件)
    void onNetworkStateChanged(bool isOnline);
```

声明保留但实现改为使用组件。同时在 private 区域添加组件成员:

```cpp
    // UI 组件
    StatusIndicator* m_statusIndicator = nullptr;
    InfoField* m_fieldEmployeeId = nullptr;
    InfoField* m_fieldName = nullptr;
    InfoField* m_fieldStatus = nullptr;
    InfoField* m_fieldCheckTime = nullptr;
```

同时删除 `initNetWorkStatus` 声明中的旧逻辑引用 (声明保留，实现将修改)。

- [ ] **Step 2: 修改 mainwindow.cpp — 头文件 include**

确保新组件头文件已 include (通过 mainwindow.h 间接引入即可)。

- [ ] **Step 3: 修改 mainwindow.cpp — 替换网络状态指示器**

改造 `initNetWorkStatus()`:

```cpp
void MainWindow::initNetWorkStatus()
{
    // 找到 topWidget 中的旧 networkStatusLabel，隐藏它
    ui->networkStatusLabel->hide();

    // 创建新的 StatusIndicator 组件
    m_statusIndicator = new StatusIndicator(ui->topWidget);
    m_statusIndicator->setLabel(QStringLiteral("离线"));
    m_statusIndicator->setState(StatusIndicator::Offline);

    // 插入到 topWidget 布局中 (在原 networkStatusLabel 的位置)
    QHBoxLayout* topLayout = qobject_cast<QHBoxLayout*>(ui->topWidget->layout());
    if (topLayout) {
        // 找到 networkStatusLabel 在布局中的索引
        int idx = topLayout->indexOf(ui->networkStatusLabel);
        if (idx >= 0) {
            topLayout->insertWidget(idx, m_statusIndicator);
        }
    }

    // 连接信号
    connect(networkClient, &Networkclient::networkStateChanged,
            this, &MainWindow::onNetworkStateChanged, Qt::QueuedConnection);
    connect(networkClient, &Networkclient::connected, this, [=](){
        qDebug() << "网络已连接";
    }, Qt::QueuedConnection);
    connect(networkClient, &Networkclient::disconnected, this, [=](){
        qDebug() << "网络已断开";
    }, Qt::QueuedConnection);

    onNetworkStateChanged(false);
}
```

- [ ] **Step 4: 修改 mainwindow.cpp — 替换 onNetworkStateChanged**

```cpp
void MainWindow::onNetworkStateChanged(bool isOnline)
{
    if (!m_statusIndicator) return;
    if (isOnline) {
        m_statusIndicator->setState(StatusIndicator::Online);
        m_statusIndicator->setLabel(QStringLiteral("在线"));
    } else {
        m_statusIndicator->setState(StatusIndicator::Offline);
        m_statusIndicator->setLabel(QStringLiteral("离线"));
    }
}
```

- [ ] **Step 5: 修改 mainwindow.cpp — 替换窗口控制按钮**

在 `MainWindow::MainWindow()` 构造函数中，setupUi 之后添加:

```cpp
    // 隐藏旧的窗口控制按钮，用 IconButton 替换
    ui->minimizeButton->hide();
    ui->maximizeButton->hide();
    ui->closeButton->hide();

    // 创建新的 IconButton
    auto* btnMin = new IconButton(IconButton::Minimize, ui->topWidget);
    auto* btnMax = new IconButton(IconButton::Maximize, ui->topWidget);
    auto* btnClose = new IconButton(IconButton::Close, ui->topWidget);

    // 插入到 topWidget 布局末尾
    QHBoxLayout* topLayout = qobject_cast<QHBoxLayout*>(ui->topWidget->layout());
    if (topLayout) {
        topLayout->addWidget(btnMin);
        topLayout->addWidget(btnMax);
        topLayout->addWidget(btnClose);
    }

    connect(btnMin, &QPushButton::clicked, this, &MainWindow::onMinimizeButtonClicked);
    connect(btnMax, &QPushButton::clicked, this, &MainWindow::onMaximizeButtonClicked);
    connect(btnClose, &QPushButton::clicked, this, &MainWindow::onCloseButtonClicked);
```

- [ ] **Step 6: 修改 mainwindow.cpp — 替换 onMaximizeButtonClicked 使用 reflectWindowState**

保留现有逻辑，在 `showNormal()` 和 `showMaximized()` 之后添加对 btnMax 的 `reflectWindowState()` 调用。但由于 btnMax 是局部变量，需要改为成员变量。

在 mainwindow.h 中添加成员:
```cpp
    IconButton* m_btnMaximize = nullptr;
```

修改构造函数中的 IconButton 创建代码:
```cpp
    m_btnMaximize = new IconButton(IconButton::Maximize, ui->topWidget);
```

修改 `onMaximizeButtonClicked()`:
```cpp
void MainWindow::onMaximizeButtonClicked()
{
    if (isMaximized()) {
        showNormal();
    } else {
        showMaximized();
    }
    if (m_btnMaximize) {
        m_btnMaximize->reflectWindowState(isMaximized());
    }
}
```

- [ ] **Step 7: 修改 mainwindow.cpp — 替换设置按钮**

构造函数中:
```cpp
    ui->settingButton->hide();

    auto* btnSettings = new ActionButton(QStringLiteral("⚙  设置"), ActionButton::Secondary, ui->topWidget);
    // ⚙ 设置

    if (topLayout) {
        // 找到 settingButton 的索引，替换它
        int idx = topLayout->indexOf(ui->settingButton);
        if (idx >= 0) {
            topLayout->insertWidget(idx, btnSettings);
        }
    }

    connect(btnSettings, &QPushButton::clicked, this, &MainWindow::onSetPushButten);
```

- [ ] **Step 8: 修改 mainwindow.cpp — 替换 InfoWidget 中的信息字段**

在 `InfoWidget()` 方法中 (或创建新方法 `initInfoFields()`):

```cpp
void MainWindow::initInfoFields()
{
    // 隐藏旧的 label
    ui->employeeIdLabel->hide();
    ui->employeeIdEdit->hide();
    ui->nameLabel->hide();
    ui->nameEdit->hide();
    ui->statusLabel->hide();
    ui->statusEdit->hide();
    ui->checkTimeLabel->hide();
    ui->checkTimeEdit->hide();

    QHBoxLayout* infoLayout = qobject_cast<QHBoxLayout*>(ui->infoWidget->layout());
    if (!infoLayout) return;

    m_fieldEmployeeId = new InfoField(QStringLiteral("员工号："), ui->infoWidget);
    m_fieldName       = new InfoField(QStringLiteral("姓名："),   ui->infoWidget);
    m_fieldStatus     = new InfoField(QStringLiteral("打卡状态："), ui->infoWidget);
    m_fieldCheckTime  = new InfoField(QStringLiteral("打卡时间："), ui->infoWidget);

    infoLayout->addWidget(m_fieldEmployeeId);
    infoLayout->addWidget(m_fieldName);
    infoLayout->addWidget(m_fieldStatus);
    infoLayout->addWidget(m_fieldCheckTime);
}
```

然后在构造函数中 `InfoWidget()` 调用之前调用 `initInfoFields()`。

- [ ] **Step 9: 修改 mainwindow.cpp — 更新 onRecognitionSuccess**

```cpp
void MainWindow::onRecognitionSuccess(const QString &employeeId,
                                      const QString &name,
                                      const QString &status,
                                      const QString &checkTime,
                                      const QImage &faceImage)
{
    if (m_fieldEmployeeId) m_fieldEmployeeId->setValue(employeeId);
    if (m_fieldName)       m_fieldName->setValue(name);
    if (m_fieldStatus)     m_fieldStatus->setValue(status);
    if (m_fieldCheckTime)  m_fieldCheckTime->setValue(checkTime);
}
```

- [ ] **Step 10: 验证编译**

Run: `cmake --build build/Desktop_Qt_6_10_2_MinGW_64_bit-Debug --target AttendanceSystem`
Expected: 编译通过

- [ ] **Step 11: Commit**

```bash
git add mainwindow.h mainwindow.cpp mainwindow.ui
git commit -m "refactor(ui): replace MainWindow raw widgets with ActionButton, IconButton, StatusIndicator, InfoField components"
```

---

### Task 10: 精简 mainwindow.qss

**Files:**
- Modify: `resources/qss/mainwindow.qss`

- [ ] **Step 1: 删除已迁移到组件的样式规则**

删除以下选择器块 (它们现在由组件 C++ 代码管理):
- `QPushButton#settingButton` 及所有伪状态
- `QPushButton#minimizeButton, QPushButton#maximizeButton` 及伪状态
- `QPushButton#closeButton` 及伪状态
- `QLabel#networkStatusLabel`
- `QWidget#infoWidget QLabel#employeeIdLabel` 及同组
- `QWidget#infoWidget QLabel#employeeIdEdit` 及同组

保留的内容:
- `QMainWindow` 背景
- `QWidget#centralwidget` 背景
- `QWidget#topWidget` 顶部工具栏
- `QLabel#timeLabel` 时间标签
- 通用 `QLabel` 颜色
- `QLabel#cameraLabel`
- `QLineEdit` 系列
- `QWidget#cameraWidget`
- `QLabel#cameraDisplay`
- 通用 `QPushButton` (作为回退)
- 滚动条系列

- [ ] **Step 2: 验证编译并运行**

Run: `cmake --build build/Desktop_Qt_6_10_2_MinGW_64_bit-Debug --target AttendanceSystem`
Expected: 编译通过

- [ ] **Step 3: Commit**

```bash
git add resources/qss/mainwindow.qss
git commit -m "refactor(ui): remove widget-specific QSS rules migrated to components"
```

---

### Task 11: InfoCard — 信息卡片容器

**Files:**
- Create: `UI/Components/InfoCard.h`
- Create: `UI/Components/InfoCard.cpp`

- [ ] **Step 1: 创建 InfoCard.h**

```cpp
#ifndef INFOCARD_H
#define INFOCARD_H

#include <QWidget>

class QVBoxLayout;

class InfoCard : public QWidget
{
    Q_OBJECT
public:
    explicit InfoCard(const QString& title = {},
                      QWidget* parent = nullptr);

    /// 设置卡片标题
    void setTitle(const QString& text);

    /// 返回内容区域布局，调用方向其中添加子控件
    QVBoxLayout* contentLayout() const;

private:
    QWidget* m_titleLabel = nullptr;
    QWidget* m_contentArea = nullptr;
    QVBoxLayout* m_contentLayout = nullptr;
};

#endif // INFOCARD_H
```

- [ ] **Step 2: 创建 InfoCard.cpp**

```cpp
#include "InfoCard.h"
#include "UI/Theme/ThemeManager.h"
#include "UI/Theme/DesignTokens.h"
#include <QLabel>
#include <QVBoxLayout>

InfoCard::InfoCard(const QString& title, QWidget* parent)
    : QWidget(parent)
{
    auto* tm = ThemeManager::instance();
    int r = tm->radius("lg");

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 标题栏
    m_titleLabel = new QLabel(title, this);
    m_titleLabel->setFont(tm->font("sm", DesignTokens::FontWeight::semibold));
    m_titleLabel->setStyleSheet(QString(
        "color: %1; background: transparent; padding: 4px 0px; letter-spacing: 1px;"
    ).arg("#adb5bd"));
    mainLayout->addWidget(m_titleLabel);

    // 内容区域
    m_contentArea = new QWidget(this);
    m_contentLayout = new QVBoxLayout(m_contentArea);
    m_contentLayout->setContentsMargins(
        tm->space(4), tm->space(3), tm->space(4), tm->space(3)); // 16,12,16,12
    m_contentLayout->setSpacing(tm->space(2));
    mainLayout->addWidget(m_contentArea, 1);

    // 卡片整体样式
    setStyleSheet(QString(
        "InfoCard {"
        "  background-color: %1;"
        "  border: 1px solid %2;"
        "  border-radius: %3px;"
        "}"
    ).arg(tm->colorHex(DesignTokens::Semantic::bgSurface))
     .arg(tm->colorHex(DesignTokens::Semantic::borderSubtle))
     .arg(r));
}

void InfoCard::setTitle(const QString& text)
{
    qobject_cast<QLabel*>(m_titleLabel)->setText(text);
}

QVBoxLayout* InfoCard::contentLayout() const
{
    return m_contentLayout;
}
```

- [ ] **Step 3: 更新 CMakeLists.txt**

添加:
```
        UI/Components/InfoCard.h UI/Components/InfoCard.cpp
```

- [ ] **Step 4: 验证编译**

Run: `cmake --build build/Desktop_Qt_6_10_2_MinGW_64_bit-Debug --target AttendanceSystem`
Expected: 编译通过

- [ ] **Step 5: Commit**

```bash
git add UI/Components/InfoCard.h UI/Components/InfoCard.cpp CMakeLists.txt
git commit -m "feat(ui): add InfoCard container component with title and content area"
```

---

### Task 12: SectionPanel — 分组面板 (替代 QGroupBox)

**Files:**
- Create: `UI/Components/SectionPanel.h`
- Create: `UI/Components/SectionPanel.cpp`

- [ ] **Step 1: 创建 SectionPanel.h**

```cpp
#ifndef SECTIONPANEL_H
#define SECTIONPANEL_H

#include <QWidget>

class QVBoxLayout;

class SectionPanel : public QWidget
{
    Q_OBJECT
public:
    explicit SectionPanel(const QString& title = {},
                          QWidget* parent = nullptr);

    void setTitle(const QString& text);
    QVBoxLayout* contentLayout() const;

private:
    QWidget* m_header = nullptr;
    QLabel* m_titleLabel = nullptr;
    QWidget* m_contentArea = nullptr;
    QVBoxLayout* m_contentLayout = nullptr;
};

#endif // SECTIONPANEL_H
```

- [ ] **Step 2: 创建 SectionPanel.cpp**

```cpp
#include "SectionPanel.h"
#include "UI/Theme/ThemeManager.h"
#include "UI/Theme/DesignTokens.h"
#include <QLabel>
#include <QVBoxLayout>

SectionPanel::SectionPanel(const QString& title, QWidget* parent)
    : QWidget(parent)
{
    auto* tm = ThemeManager::instance();

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(tm->space(2));

    // 标题
    m_titleLabel = new QLabel(title, this);
    m_titleLabel->setFont(tm->font("lg", DesignTokens::FontWeight::bold));
    m_titleLabel->setStyleSheet(QString(
        "color: %1; background: transparent; padding-bottom: 4px;"
    ).arg(tm->colorHex(DesignTokens::Semantic::textPrimary)));
    mainLayout->addWidget(m_titleLabel);

    // 分隔线
    auto* divider = new QWidget(this);
    divider->setFixedHeight(1);
    divider->setStyleSheet(QString(
        "background-color: %1;"
    ).arg(tm->colorHex(DesignTokens::Semantic::borderSubtle)));
    mainLayout->addWidget(divider);

    // 内容区
    m_contentArea = new QWidget(this);
    m_contentLayout = new QVBoxLayout(m_contentArea);
    m_contentLayout->setContentsMargins(0, tm->space(2), 0, 0);
    m_contentLayout->setSpacing(tm->space(3));
    mainLayout->addWidget(m_contentArea, 1);
}

void SectionPanel::setTitle(const QString& text) { m_titleLabel->setText(text); }
QVBoxLayout* SectionPanel::contentLayout() const { return m_contentLayout; }
```

- [ ] **Step 3: 更新 CMakeLists.txt**

添加:
```
        UI/Components/SectionPanel.h UI/Components/SectionPanel.cpp
```

- [ ] **Step 4: 验证编译**

Run: `cmake --build build/Desktop_Qt_6_10_2_MinGW_64_bit-Debug --target AttendanceSystem`
Expected: 编译通过

- [ ] **Step 5: Commit**

```bash
git add UI/Components/SectionPanel.h UI/Components/SectionPanel.cpp CMakeLists.txt
git commit -m "feat(ui): add SectionPanel component as QGroupBox replacement"
```

---

### Task 13: ToastNotification — 消息通知

**Files:**
- Create: `UI/Components/ToastNotification.h`
- Create: `UI/Components/ToastNotification.cpp`

- [ ] **Step 1: 创建 ToastNotification.h**

```cpp
#ifndef TOASTNOTIFICATION_H
#define TOASTNOTIFICATION_H

#include <QWidget>

class QLabel;
class QPropertyAnimation;

class ToastNotification : public QWidget
{
    Q_OBJECT
public:
    enum Level { Success, Error, Warning, Info };

    /// 在 parent 顶部居中显示 Toast，自动消失
    static void show(QWidget* parent, const QString& message,
                     Level level = Info, int durationMs = 3000);

private:
    explicit ToastNotification(QWidget* parent, const QString& message,
                               Level level, int durationMs);
    void animateIn();
    void animateOut();

    QLabel* m_label = nullptr;
    Level m_level = Info;
    int m_durationMs = 3000;
};

#endif // TOASTNOTIFICATION_H
```

- [ ] **Step 2: 创建 ToastNotification.cpp**

```cpp
#include "ToastNotification.h"
#include "UI/Theme/ThemeManager.h"
#include "UI/Theme/DesignTokens.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QTimer>

ToastNotification::ToastNotification(QWidget* parent, const QString& message,
                                     Level level, int durationMs)
    : QWidget(parent)
    , m_level(level)
    , m_durationMs(durationMs)
{
    auto* tm = ThemeManager::instance();

    setMaximumWidth(480);
    setMinimumHeight(40);

    auto* lay = new QHBoxLayout(this);
    lay->setContentsMargins(12, 10, 16, 10);
    lay->setSpacing(8);

    // 左侧色条
    auto* bar = new QWidget(this);
    bar->setFixedWidth(4);
    bar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    QString barColor;
    QString iconText;
    switch (level) {
    case Success: barColor = tm->colorHex(DesignTokens::Semantic::semSuccess); iconText = QStringLiteral("✓ "); break;
    case Error:   barColor = tm->colorHex(DesignTokens::Semantic::semDanger);  iconText = QStringLiteral("✗ "); break;
    case Warning: barColor = tm->colorHex(DesignTokens::Semantic::semWarning); iconText = QStringLiteral("⚠ "); break;
    case Info:    barColor = tm->colorHex(DesignTokens::Semantic::semInfo);    iconText = QStringLiteral("ℹ "); break;
    }
    bar->setStyleSheet(QString("background-color: %1; border-radius: 2px;").arg(barColor));
    lay->addWidget(bar);

    m_label = new QLabel(iconText + message, this);
    m_label->setWordWrap(true);
    m_label->setFont(tm->font("sm", DesignTokens::FontWeight::medium));
    m_label->setStyleSheet(QString(
        "color: %1; background: transparent;"
    ).arg(tm->colorHex(DesignTokens::Semantic::textPrimary)));
    lay->addWidget(m_label);

    // 容器样式
    int r = tm->radius("md");
    setStyleSheet(QString(
        "ToastNotification {"
        "  background-color: %1;"
        "  border-radius: %2px;"
        "}"
    ).arg(tm->colorHex(DesignTokens::Semantic::bgElevated)).arg(r));

    // 透明度效果
    auto* effect = new QGraphicsOpacityEffect(this);
    effect->setOpacity(0.0);
    setGraphicsEffect(effect);

    // 定位在父容器顶部居中
    setGeometry(
        (parent->width() - 480) / 2, 8,
        480, sizeHint().height()
    );

    show();
    raise();
    animateIn();

    // 自动消失
    QTimer::singleShot(durationMs, this, &ToastNotification::animateOut);
}

void ToastNotification::show(QWidget* parent, const QString& message,
                              Level level, int durationMs)
{
    // 自动删除旧实例
    new ToastNotification(parent, message, level, durationMs);
}

void ToastNotification::animateIn()
{
    auto* effect = qobject_cast<QGraphicsOpacityEffect*>(graphicsEffect());
    if (!effect) return;

    auto* anim = new QPropertyAnimation(effect, "opacity", this);
    anim->setDuration(250);
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    anim->start(QAbstractAnimation::DeleteWhenStopped);

    // 同时做位移动画 (从上方滑入)
    auto* posAnim = new QPropertyAnimation(this, "pos", this);
    QPoint startPos(x(), y() - 20);
    QPoint endPos(x(), y());
    posAnim->setDuration(250);
    posAnim->setStartValue(startPos);
    posAnim->setEndValue(endPos);
    posAnim->setEasingCurve(QEasingCurve::OutCubic);
    posAnim->start(QAbstractAnimation::DeleteWhenStopped);
}

void ToastNotification::animateOut()
{
    auto* effect = qobject_cast<QGraphicsOpacityEffect*>(graphicsEffect());
    if (!effect) { deleteLater(); return; }

    auto* anim = new QPropertyAnimation(effect, "opacity", this);
    anim->setDuration(200);
    anim->setStartValue(1.0);
    anim->setEndValue(0.0);
    anim->setEasingCurve(QEasingCurve::InCubic);
    connect(anim, &QPropertyAnimation::finished, this, &QObject::deleteLater);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}
```

- [ ] **Step 3: 更新 CMakeLists.txt**

添加:
```
        UI/Components/ToastNotification.h UI/Components/ToastNotification.cpp
```

- [ ] **Step 4: 验证编译**

Run: `cmake --build build/Desktop_Qt_6_10_2_MinGW_64_bit-Debug --target AttendanceSystem`
Expected: 编译通过

- [ ] **Step 5: Commit**

```bash
git add UI/Components/ToastNotification.h UI/Components/ToastNotification.cpp CMakeLists.txt
git commit -m "feat(ui): add ToastNotification component with slide-in/fade-out animation"
```

---

### Task 14: 改造 SetWindow — 设置页控件替换

**Files:**
- Modify: `UI/setwindow.h`
- Modify: `UI/setwindow.cpp`
- Modify: `UI/setwindow.ui`

- [ ] **Step 1: 修改 setwindow.ui — 删除内联样式**

删除 `btnTestConnection` 的 `styleSheet` 属性:
```xml
<!-- 删除:
<property name="styleSheet">
 <string notr="true">background-color: #4CAF50; color: white;</string>
</property>
-->
```

同样删除 `btnDisconnect` 的 `styleSheet` 属性:
```xml
<!-- 删除:
<property name="styleSheet">
 <string notr="true">background-color: #f44336; color: white;</string>
</property>
-->
```

- [ ] **Step 2: 修改 setwindow.cpp — 在构造函数末尾用 ActionButton 包装关键按钮**

在 `SetWindow::SetWindow()` 构造函数末尾 (`setupConnections()` 之后) 添加:

```cpp
    // 将按钮替换为 ActionButton 样式 (不改变 objectName, 只覆盖样式)
    auto applyActionStyle = [](QPushButton* btn, ActionButton::Variant v) {
        // 使用 ActionButton 样式但不替换 widget
        auto* tm = ThemeManager::instance();
        QColor bg, fg, border, hoverBg, activeBg;
        bool hasBorder = false;

        switch (v) {
        case ActionButton::Primary:
            bg = tm->color(DesignTokens::Semantic::brandPrimary);
            fg = QColor("#ffffff");
            border = Qt::transparent;
            hoverBg = tm->color(DesignTokens::Semantic::brandPrimaryHover);
            activeBg = tm->color(DesignTokens::Semantic::brandPrimaryActive);
            break;
        case ActionButton::Secondary:
            bg = tm->color(DesignTokens::Semantic::bgElevated);
            fg = tm->color(DesignTokens::Semantic::textPrimary);
            border = tm->color(DesignTokens::Semantic::borderDefault);
            hoverBg = ThemeManager::lighten(bg, 0.10f);
            activeBg = ThemeManager::darken(bg, 0.10f);
            hasBorder = true;
            break;
        case ActionButton::Danger:
            bg = Qt::transparent;
            fg = tm->color(DesignTokens::Semantic::semDanger);
            border = tm->color(DesignTokens::Semantic::semDanger);
            hoverBg = ThemeManager::alpha(tm->color(DesignTokens::Semantic::semDanger), 0.15f);
            activeBg = ThemeManager::alpha(tm->color(DesignTokens::Semantic::semDanger), 0.25f);
            hasBorder = true;
            break;
        }

        int r = tm->radius("md");
        btn->setStyleSheet(QString(
            "QPushButton {"
            "  background-color: %1; color: %2;"
            "  border: %3; border-radius: %4px;"
            "  padding: 6px 16px; font-size: 13px; font-weight: 600;"
            "}"
            "QPushButton:hover { background-color: %5; }"
            "QPushButton:pressed { background-color: %6; }"
        ).arg(bg.name()).arg(fg.name())
         .arg(hasBorder ? QString("1px solid %1").arg(border.name()) : "none")
         .arg(r).arg(hoverBg.name()).arg(activeBg.name()));
    };

    applyActionStyle(ui->btnTestConnection, ActionButton::Primary);
    applyActionStyle(ui->btnDisconnect, ActionButton::Danger);
    applyActionStyle(ui->btnSave, ActionButton::Primary);
    applyActionStyle(ui->btnCancel, ActionButton::Secondary);
    applyActionStyle(ui->btnRestore, ActionButton::Secondary);
```

并添加 include:
```cpp
#include "UI/Components/ActionButton.h"
#include "UI/Theme/ThemeManager.h"
#include "UI/Theme/DesignTokens.h"
```

- [ ] **Step 3: 验证编译**

Run: `cmake --build build/Desktop_Qt_6_10_2_MinGW_64_bit-Debug --target AttendanceSystem`
Expected: 编译通过

- [ ] **Step 4: Commit**

```bash
git add UI/setwindow.h UI/setwindow.cpp UI/setwindow.ui
git commit -m "refactor(ui): apply ActionButton styles to SetWindow buttons, remove inline QSS"
```

---

## 验证清单

全部任务完成后，执行以下验证:

- [ ] 编译: `cmake --build build/Desktop_Qt_6_10_2_MinGW_64_bit-Debug --target AttendanceSystem` — PASS
- [ ] 运行: 启动应用，确认主界面渲染正常
- [ ] 按钮交互: Primary/Secondary/Danger 按钮 hover/pressed 状态正确
- [ ] 窗口控制: 最小化/最大化/关闭按钮正常，最大化图标切换正确
- [ ] 网络状态: 在线/离线指示灯切换正常
- [ ] 信息字段: 识别成功后员工号/姓名/状态/时间显示正确
- [ ] 设置窗口: 打开设置，按钮样式统一，导航切换正常
- [ ] Toast: 调用 `ToastNotification::show()` 验证弹出和消失动画
