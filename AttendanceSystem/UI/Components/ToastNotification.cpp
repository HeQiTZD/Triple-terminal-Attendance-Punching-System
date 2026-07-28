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

    QWidget::show();
    raise();
    animateIn();

    // 自动消失
    QTimer::singleShot(durationMs, this, &ToastNotification::animateOut);
}

void ToastNotification::show(QWidget* parent, const QString& message,
                              Level level, int durationMs)
{
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
