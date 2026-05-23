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
