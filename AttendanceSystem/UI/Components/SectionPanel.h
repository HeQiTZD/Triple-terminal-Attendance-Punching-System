#ifndef SECTIONPANEL_H
#define SECTIONPANEL_H

#include <QWidget>

class QVBoxLayout;
class QLabel;

class SectionPanel : public QWidget
{
    Q_OBJECT
public:
    explicit SectionPanel(const QString& title = {},
                          QWidget* parent = nullptr);

    void setTitle(const QString& text);
    QVBoxLayout* contentLayout() const;

private:
    QLabel* m_titleLabel = nullptr;
    QWidget* m_contentArea = nullptr;
    QVBoxLayout* m_contentLayout = nullptr;
};

#endif // SECTIONPANEL_H
