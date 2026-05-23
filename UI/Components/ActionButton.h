#ifndef ACTIONBUTTON_H
#define ACTIONBUTTON_H

#include <QPushButton>

class ActionButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(Variant variant READ variant WRITE setVariant)

public:
    enum Variant { Primary, Secondary, Danger };
    Q_ENUM(Variant)

    explicit ActionButton(const QString& text = {},
                          Variant v = Primary,
                          QWidget* parent = nullptr);

    Variant variant() const;
    void setVariant(Variant v);
    void setBusy(bool busy);

private:
    void applyStyle();
    Variant m_variant = Primary;
    bool m_busy = false;
    QString m_originalText;
};

#endif // ACTIONBUTTON_H
