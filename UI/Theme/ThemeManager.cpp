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
    int px = DesignTokens::FontSize::base;
    if (sizeKey == "xs")   px = DesignTokens::FontSize::xs;
    else if (sizeKey == "sm")   px = DesignTokens::FontSize::sm;
    else if (sizeKey == "base") px = DesignTokens::FontSize::base;
    else if (sizeKey == "lg")   px = DesignTokens::FontSize::lg;
    else if (sizeKey == "xl")   px = DesignTokens::FontSize::xl;
    else if (sizeKey == "2xl")  px = DesignTokens::FontSize::xxl;
    else if (sizeKey == "3xl")  px = DesignTokens::FontSize::xxxl;

    QFont f;
    f.setPixelSize(px);
    f.setWeight(static_cast<QFont::Weight>(weight));
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
