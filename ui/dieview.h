#ifndef DIEVIEW_H
#define DIEVIEW_H

#include <QGraphicsRectItem>

#include "die.h"

class DieView : public QGraphicsRectItem
{
public:
    DieView(Die* die);
    ~DieView();

private:
    void makeUI();

    Die* m_die;

    // setup and draw these in paint event
    // QImage  currentSideImage = m_die.current();
    UIElement* m_droppedText;
    UIElement* m_dropDieButton;
    UIElement* m_turnButton;

};

#endif // DIEVIEW_H
