#include "uielementfactory.h"

#include <QPen>
#include <QDebug>

UIElement *UIElementFactory::create(UIElement::Role role, const QString &text, const QSize &size, const QPoint &position, QGraphicsRectItem* parent)
{
    UIElement* button = new UIElement (text);
    button->setRole(role);
    button->setPos(position);
    button->setSize(size);
    button->setBorderWidth(3);
    button->setRect(position.x(), position.y(), size.width(), size.height());
    button->setShape(UIElement::Shape::ROUNDED_RECTANGLE);
    button->setColor(UIElement::Element::BORDER, QColor("#fff"));
    button->setColor(UIElement::Element::FILL,   QColor("#111"));
    button->setColor(UIElement::Element::TEXT,   QColor("#fff"));
    button->setFont(QFont("Comic Sans", 10));
    button->setParentItem(parent);

    return button;
}
