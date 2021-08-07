#ifndef MENU_H
#define MENU_H

#include <QGraphicsRectItem>
#include <QList>

#include "uielement.h"

class Menu : public QGraphicsRectItem
{
public:
    enum class State {BUTTON_HOVER, MENU_HOVER, DEFAULT};

    Menu();
    ~Menu();

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) override;

    void createButtons();
    void showButtons();
    void hideButtons();    

    bool isHidden () const;
    void setHidden(bool isHidden);

    UIElement* buttonAtPosition(const QPoint& pixelPosition);
    void setState(const State& state);

private:
    void setColor(const QColor& color);

    void defaultButtons();
    void deleteButtons();

    QColor m_color;
    State  m_state = State::DEFAULT;

    bool m_isHidden  = false;
    bool m_noButtons = true;
    bool m_buttonHovered = false;
    bool m_menuHovered   = false;

    QList<UIElement*> *m_buttons = nullptr;
};

#endif // MENU_H
