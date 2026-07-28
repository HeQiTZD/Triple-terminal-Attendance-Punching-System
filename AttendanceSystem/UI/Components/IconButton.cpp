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
    case Minimize: setText(QStringLiteral("−")); break;
    case Maximize: setText(QStringLiteral("⬜")); break;
    case Close:    setText(QStringLiteral("✕")); break;
    }
    applyStyle();
}

void IconButton::reflectWindowState(bool isMaximized)
{
    if (m_role == Maximize) {
        setText(isMaximized ? QStringLiteral("⬝")
                            : QStringLiteral("⬜"));
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
