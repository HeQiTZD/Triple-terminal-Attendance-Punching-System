#ifndef ICONBUTTON_H
#define ICONBUTTON_H

#include <QPushButton>

class IconButton : public QPushButton
{
    Q_OBJECT
public:
    enum Role { Minimize, Maximize, Close };
    Q_ENUM(Role)

    explicit IconButton(Role role, QWidget* parent = nullptr);
    void reflectWindowState(bool isMaximized);

private:
    void applyStyle();
    Role m_role;
};

#endif // ICONBUTTON_H
