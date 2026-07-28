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
