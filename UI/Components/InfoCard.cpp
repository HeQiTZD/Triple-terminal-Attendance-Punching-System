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
        tm->space(4), tm->space(3), tm->space(4), tm->space(3));
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
