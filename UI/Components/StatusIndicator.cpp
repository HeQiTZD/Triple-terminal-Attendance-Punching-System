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
