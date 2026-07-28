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
