#ifndef UIELEMENTFACTORY_H
#define UIELEMENTFACTORY_H

#include "uielement.h"

class UIElementFactory
{
public:
    static UIElement* create (UIElement::Role role, const QString& text, const QSize& size, const QPoint& position, QGraphicsRectItem* parent = nullptr);

private:
};

#endif // UIELEMENTFACTORY_H
