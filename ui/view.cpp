#include "view.h"

#include <QMouseEvent>
#include <QDebug>

View::View(QGraphicsScene* scene, QWidget* parent)
    : QGraphicsView (scene, parent)
{
    setMouseTracking(true);
}

void View::mouseMoveEvent(QMouseEvent *event)
{
    emit mousePositionChanged(event->pos() - pos());    
}
