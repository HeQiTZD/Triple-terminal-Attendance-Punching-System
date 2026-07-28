#ifndef TOASTNOTIFICATION_H
#define TOASTNOTIFICATION_H

#include <QWidget>

class QLabel;
class QPropertyAnimation;

class ToastNotification : public QWidget
{
    Q_OBJECT
public:
    enum Level { Success, Error, Warning, Info };

    /// 在 parent 顶部居中显示 Toast，自动消失
    static void show(QWidget* parent, const QString& message,
                     Level level = Info, int durationMs = 3000);

private:
    explicit ToastNotification(QWidget* parent, const QString& message,
                               Level level, int durationMs);
    void animateIn();
    void animateOut();

    QLabel* m_label = nullptr;
    Level m_level = Info;
    int m_durationMs = 3000;
};

#endif // TOASTNOTIFICATION_H
