#ifndef INFOCARD_H
#define INFOCARD_H

#include <QWidget>

class QVBoxLayout;

class InfoCard : public QWidget
{
    Q_OBJECT
public:
    explicit InfoCard(const QString& title = {},
                      QWidget* parent = nullptr);

    void setTitle(const QString& text);
    QVBoxLayout* contentLayout() const;

private:
    QWidget* m_titleLabel = nullptr;
    QWidget* m_contentArea = nullptr;
    QVBoxLayout* m_contentLayout = nullptr;
};

#endif // INFOCARD_H
