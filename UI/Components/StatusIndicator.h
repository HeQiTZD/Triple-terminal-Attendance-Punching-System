#ifndef STATUSINDICATOR_H
#define STATUSINDICATOR_H

#include <QWidget>

class QLabel;

class StatusIndicator : public QWidget
{
    Q_OBJECT
public:
    enum State { Online, Offline, Warning };
    Q_ENUM(State)

    explicit StatusIndicator(QWidget* parent = nullptr);

    void setState(State s);
    State state() const;
    void setLabel(const QString& text);
    void setShowDot(bool show);

private:
    void applyStyle();
    State m_state = Offline;
    QLabel* m_dot = nullptr;
    QLabel* m_label = nullptr;
};

#endif // STATUSINDICATOR_H
