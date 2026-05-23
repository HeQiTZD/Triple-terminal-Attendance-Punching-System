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
