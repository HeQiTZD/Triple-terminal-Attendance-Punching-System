#ifndef INFOFIELD_H
#define INFOFIELD_H

#include <QWidget>

class QLabel;

class InfoField : public QWidget
{
    Q_OBJECT
public:
    explicit InfoField(const QString& label = {},
                       QWidget* parent = nullptr);

    void setLabel(const QString& text);
    void setValue(const QString& text);
    void setValueColor(const QColor& c);
    QString value() const;

private:
    QLabel* m_label = nullptr;
    QLabel* m_value = nullptr;
};

#endif // INFOFIELD_H
