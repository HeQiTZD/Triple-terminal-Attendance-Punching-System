# UI 系统设计规范

## 元信息

- **日期**: 2026-05-23
- **项目**: AttendanceSystem 设备端人脸识别考勤系统
- **技术栈**: Qt 6.10.2 C++ Widgets, Fusion Style, QSS
- **范围**: 设计令牌体系 + 可复用组件库
- **风格**: 企业级专业风格（蓝灰配色体系）

---

## 1. 架构设计

### 1.1 三层结构

```
Application
├─ 组件层 (Component Layer)      StatusIndicator / ActionButton / InfoCard ...
│   封装样式 + 行为，对外暴露业务 API
├─ 令牌层 (Token Layer)           ThemeManager (单例)
│   Primitive → Semantic → Component 三级令牌
├─ QSS 补充层                    全局滚动条、Tooltip、伪状态微调
└─ Qt 原生控件                   回退层 (Fusion + 深色 Palette)
```

### 1.2 核心原则

- **ThemeManager** — 单例，启动时初始化，所有组件通过它获取令牌值
- **组件子类化** — 每个组件继承对应 Qt 控件，构造函数中自动从 ThemeManager 获取令牌应用样式
- **QSS 退居补充** — 只处理组件 API 不便覆盖的内容（`:hover`/`:pressed` 伪状态、全局滚动条、Tooltip）
- **Fusion 深色回退** — 保留现有 QPalette 和 QStyleFactory::create("Fusion")，确保未覆盖的原生控件不会出现浅色突兀

### 1.3 文件结构

```
UI/
├── Theme/
│   ├── ThemeManager.h           # 令牌管理器单例
│   ├── ThemeManager.cpp
│   ├── DesignTokens.h           # 令牌数据结构定义
│   └── ThemeManager.qss         # 补充全局 QSS
├── Components/
│   ├── ActionButton.h/cpp       # 操作按钮 (Primary/Secondary/Danger)
│   ├── IconButton.h/cpp         # 图标按钮 (窗口控制)
│   ├── StatusIndicator.h/cpp    # 状态指示灯
│   ├── InfoField.h/cpp          # 信息字段 (标签+值)
│   ├── InfoCard.h/cpp           # 信息卡片容器
│   ├── SectionPanel.h/cpp       # 分组面板 (替代 QGroupBox)
│   ├── SideNavButton.h/cpp      # 侧边导航按钮
│   ├── ToastNotification.h/cpp  # Toast 消息通知
│   └── FramelessWindow.h/cpp    # 无边框窗口基类
├── setwindow.h/cpp/ui           # 现有设置窗口 (Phase 5 改造)
└── facevideowidget.h/cpp        # 现有视频控件 (保持不变)
```

---

## 2. 设计令牌体系

### 2.1 色彩系统

#### Primitive 色板

| Token | 值 | 说明 |
|-------|-----|------|
| `gray-0` | `#ffffff` | white |
| `gray-50` | `#f8fafc` | - |
| `gray-100` | `#f1f5f9` | - |
| `gray-200` | `#e2e8f0` | - |
| `gray-300` | `#cbd5e1` | - |
| `gray-400` | `#94a3b8` | - |
| `gray-500` | `#64748b` | - |
| `gray-600` | `#475569` | - |
| `gray-700` | `#334155` | - |
| `gray-800` | `#1e293b` | - |
| `gray-900` | `#0f172a` | - |
| `gray-950` | `#020617` | - |
| `blue-400` | `#60a5fa` | - |
| `blue-500` | `#3b82f6` | - |
| `blue-600` | `#2563eb` | 品牌主色 |
| `blue-700` | `#1d4ed8` | - |
| `blue-800` | `#1e40af` | - |
| `green-600` | `#16a34a` | 成功/在线 |
| `amber-600` | `#d97706` | 警告 |
| `red-600` | `#dc2626` | 危险/错误 |
| `sky-600` | `#0284c7` | 信息 |

#### Semantic 令牌 (深色主题)

**背景层级**:
| Token | 映射 | 用途 |
|-------|------|------|
| `bg-app` | `gray-950` | 应用背景 |
| `bg-surface` | `gray-900` | 卡片/面板背景 |
| `bg-elevated` | `gray-800` | 悬浮层 (对话框/弹出) |
| `bg-overlay` | `gray-700` | 叠层 (mask) |

**文字层级**:
| Token | 映射 | 用途 |
|-------|------|------|
| `text-primary` | `gray-50` | 主要文字 |
| `text-secondary` | `gray-300` | 次要文字 |
| `text-tertiary` | `gray-400` | 辅助/占位文字 |
| `text-disabled` | `gray-500` | 禁用文字 |

**品牌**:
| Token | 映射 | 用途 |
|-------|------|------|
| `brand-primary` | `blue-600` | 主色 |
| `brand-primary-hover` | `blue-500` | hover |
| `brand-primary-active` | `blue-700` | active |

**语义色**:
| Token | 映射 |
|-------|------|
| `semantic-success` | `green-600` |
| `semantic-warning` | `amber-600` |
| `semantic-danger` | `red-600` |
| `semantic-info` | `sky-600` |

**边框**:
| Token | 映射 | 用途 |
|-------|------|------|
| `border-default` | `gray-700` | 默认边框 |
| `border-subtle` | `gray-800` | 微弱边框 |
| `border-emphasis` | `gray-500` | 强调边框 |

### 2.2 字体与排版

**字体栈**:
| Token | 值 | 用途 |
|-------|-----|------|
| `font-mono` | `"Consolas", "SF Mono", monospace` | 时间/数字/代码 |
| `font-ui` | `"Microsoft YaHei", "PingFang SC"` | 中文 UI |
| `font-en` | `"Segoe UI", "Inter"` | 英文/数字标题 |

**字号阶梯** (基于 4px 网格):
| Token | 大小 | 用途 |
|-------|------|------|
| `text-xs` | 11px | 辅助/角标 |
| `text-sm` | 13px | 次要正文/按钮 |
| `text-base` | 14px | 正文 |
| `text-lg` | 16px | 小标题 |
| `text-xl` | 20px | 信息值/大数字 |
| `text-2xl` | 24px | 页面标题 |
| `text-3xl` | 32px | 主标题 |

**字重**:
| Token | 值 |
|-------|-----|
| `font-normal` | 400 |
| `font-medium` | 500 |
| `font-semibold` | 600 |
| `font-bold` | 700 |

### 2.3 间距与圆角

**间距** (基于 4px):
| Token | 值 | 用途 |
|-------|-----|------|
| `space-1` | 4px | 紧凑间距 |
| `space-2` | 8px | 默认间距 |
| `space-3` | 12px | 段落间距 |
| `space-4` | 16px | 大间距 |
| `space-5` | 20px | 区块间距 |
| `space-6` | 24px | 页面间距 |
| `space-8` | 32px | 大区块间距 |

**圆角**:
| Token | 值 | 用途 |
|-------|-----|------|
| `radius-sm` | 4px | 紧凑元素 (标签/徽标) |
| `radius-md` | 6px | 默认 (按钮/输入框) |
| `radius-lg` | 8px | 卡片/面板 |
| `radius-xl` | 12px | 大型容器 |

**发光** (深色主题):
| Token | 值 |
|-------|-----|
| `glow-sm` | `0 0 4px rgba(37,99,235,0.15)` |
| `glow-md` | `0 0 8px rgba(37,99,235,0.20)` |
| `glow-lg` | `0 0 16px rgba(37,99,235,0.25)` |

### 2.4 ThemeManager API

```cpp
class ThemeManager : public QObject {
    Q_OBJECT
public:
    static ThemeManager* instance();
    void initialize();

    // 颜色
    QColor color(const QString& tokenPath) const;     // "bg.surface"
    QString colorHex(const QString& tokenPath) const;

    // 字体
    QFont font(const QString& size, int weight) const; // "text-lg", 600

    // 间距/圆角
    int space(int level) const;    // 4 → 16px
    int radius(const QString& size) const;  // "md" → 6px

    // 颜色操作
    QColor alpha(const QColor&, float a) const;
    QColor lighten(const QColor&, float amount) const;
    QColor darken(const QColor&, float amount) const;

    // 为组件生成 QSS
    QString componentStyle(const QString& key) const; // "actionbutton.primary"

signals:
    void themeChanged();
};
```

---

## 3. 组件设计

### 3.1 ActionButton — 操作按钮

替代裸 QPushButton，预置三种变体。

```cpp
class ActionButton : public QPushButton {
    Q_OBJECT
public:
    enum Variant { Primary, Secondary, Danger };

    explicit ActionButton(const QString& text,
                          Variant v = Primary,
                          QWidget* parent = nullptr);

    void setVariant(Variant v);
    void setBusy(bool busy);  // 加载中状态
};
```

**规格**:

| 属性 | Primary | Secondary | Danger |
|------|---------|-----------|--------|
| 背景 | `brand-primary` | `bg-elevated` | transparent |
| 文字色 | `white` | `text-primary` | `semantic-danger` |
| 边框 | none | `border-default` | `semantic-danger` |
| hover 背景 | `brand-primary-hover` | lighten(bg-elevated, 10%) | alpha(danger, 15%) |
| active 背景 | `brand-primary-active` | darken(bg-elevated, 10%) | alpha(danger, 25%) |
| 圆角 | `radius-md` (6px) | same | same |
| 最小尺寸 | 80 × 32 | 80 × 32 | 80 × 32 |
| 字重 | `font-semibold` | `font-semibold` | `font-semibold` |

### 3.2 IconButton — 图标按钮

窗口控制栏专用，固定 36×36px。

```cpp
class IconButton : public QPushButton {
    Q_OBJECT
public:
    enum Role { Minimize, Maximize, Close };

    explicit IconButton(Role role, QWidget* parent = nullptr);
    void reflectWindowState(bool isMaximized);
};
```

**规格**:
- 固定 36 × 36px，圆角 6px
- 默认背景透明，文字 `text-tertiary`
- hover: 背景 alpha(white, 8%)
- Close hover: 背景 `semantic-danger`，文字 white
- Maximize 自动根据窗口状态切换图标文字 (⬜/⬝)

### 3.3 StatusIndicator — 状态指示灯

```cpp
class StatusIndicator : public QWidget {
    Q_OBJECT
public:
    enum State { Online, Offline, Warning };

    void setState(State s);
    void setLabel(const QString& text);
    void setShowDot(bool show);
};
```

**规格**:
- 圆点 8 × 8px + 文字标签，水平排列 (间距 8px)
- 整体容器: 圆角 6px, padding 6px 14px, 字号 13px, 字重 600

| State | 圆点色 | 文字色 | 背景 | 边框 |
|-------|-------|-------|------|------|
| Online | `semantic-success` | `semantic-success` | alpha(success, 10%) | alpha(success, 30%) |
| Offline | `gray-500` | `text-tertiary` | `bg-surface` | `border-default` |
| Warning | `semantic-warning` | `semantic-warning` | alpha(warning, 10%) | alpha(warning, 30%) |

### 3.4 InfoField — 信息字段

用于 "员工号：00123" 这种标签-值对。

```cpp
class InfoField : public QWidget {
    Q_OBJECT
public:
    void setLabel(const QString& text);
    void setValue(const QString& text);
    void setValueColor(const QColor& c);
};
```

**规格**:
- 内部两个 QLabel 水平排列，间距 8px
- **标签**: `text-tertiary`, 16px, `font-bold`, 最小宽 68px
- **值**: `text-primary`, 20px, `font-bold`

### 3.5 ToastNotification — 消息通知

```cpp
class ToastNotification : public QWidget {
    Q_OBJECT
public:
    enum Level { Success, Error, Warning, Info };

    static void show(QWidget* parent, const QString& message,
                     Level level = Info, int durationMs = 3000);
};
```

**规格**:
- 固定于父容器顶部居中，宽度自适应内容 (最大 480px)
- 入场: 从上方滑入 (250ms ease-out)
- 离场: 淡出 (200ms)
- 左边框 4px 语义色条
- 背景 `bg-elevated`, 圆角 `radius-md`
- 自动 3 秒后消失

---

## 4. QSS 补充定位

### 4.1 职责范围

QSS 只处理以下内容，其余全部由组件 C++ 代码负责：

- 全局滚动条 (`QScrollBar:vertical/horizontal`)
- Tooltip 全局样式 (`QToolTip`)
- 焦点轮廓 (`:focus-visible`)
- 菜单全局样式 (`QMenu`, `QMenuBar`)
- 原生容器 (`QStackedWidget`, `QScrollArea`)

### 4.2 与旧 QSS 的关系

`mainwindow.qss` 中针对具体控件 ID 的样式（如 `#settingButton`、`#closeButton`、`#infoWidget` 等）全部迁移到对应组件的 C++ 代码中。剩余全局规则合并到 `ThemeManager.qss`。

---

## 5. 实施计划

### 5.1 实施阶段

| Phase | 内容 | 预计影响文件 |
|-------|------|-------------|
| **Phase 1** | 令牌基础设施: `ThemeManager` + `DesignTokens.h` + `ThemeManager.qss` | `main.cpp` 新增初始化调用 |
| **Phase 2** | 基础控件: `ActionButton` → `IconButton` → `StatusIndicator` → `InfoField` | 新增 `UI/Components/` 下 8 个文件 |
| **Phase 3** | 改造 MainWindow: 替换现有控件，清理旧 QSS | `mainwindow.cpp/h`, `mainwindow.ui`, `mainwindow.qss` |
| **Phase 4** | 容器组件 + Toast: `InfoCard` → `SectionPanel` → `ToastNotification` | 新增组件文件 |
| **Phase 5** | 改造 SetWindow: 替换设置页中的按钮/GroupBox 等 | `setwindow.cpp/h/ui` |

### 5.2 向后兼容

- 现有 QPalette 和 Fusion Style 设置保留，作为未覆盖原生控件的回退层
- CMakeLists.txt 在 Phase 1 中添加 `UI/Theme/` 和 `UI/Components/` 源文件
- 不改动 FaceVideoWidget、CameraCapture 等非 UI 模块

### 5.3 文件影响清单

| 文件 | Phase | 变更方式 |
|------|-------|---------|
| `main.cpp` | 1 | 新增 `ThemeManager::instance()->initialize()` |
| `UI/Theme/ThemeManager.h/cpp` | 1 | **新建** |
| `UI/Theme/DesignTokens.h` | 1 | **新建** |
| `UI/Theme/ThemeManager.qss` | 1 | **新建** |
| `UI/Components/ActionButton.h/cpp` | 2 | **新建** |
| `UI/Components/IconButton.h/cpp` | 2 | **新建** |
| `UI/Components/StatusIndicator.h/cpp` | 2 | **新建** |
| `UI/Components/InfoField.h/cpp` | 2 | **新建** |
| `CMakeLists.txt` | 2 | 添加新源文件 |
| `mainwindow.h/cpp` | 3 | 替换控件引用 |
| `mainwindow.ui` | 3 | 精简内联样式 |
| `resources/qss/mainwindow.qss` | 3 | 大幅精简 |
| `UI/Components/InfoCard.h/cpp` | 4 | **新建** |
| `UI/Components/SectionPanel.h/cpp` | 4 | **新建** |
| `UI/Components/ToastNotification.h/cpp` | 4 | **新建** |
| `setwindow.h/cpp/ui` | 5 | 控件替换 |
