#ifndef VIEW_H
#define VIEW_H

#include <QGraphicsView>

class View : public QGraphicsView
{
    Q_OBJECT

public:
    View(QGraphicsScene* scene, QWidget* parent);

    void mouseMoveEvent(QMouseEvent *event) override;

signals:
    void mousePositionChanged(const QPoint& mousePosition);
};

#endif // VIEW_H
