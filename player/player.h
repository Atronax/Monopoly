#ifndef PLAYER_H
#define PLAYER_H

#include <QGraphicsRectItem>

#include "hand.h"

class Player : public QGraphicsRectItem
{
public:
    enum class Shape {SQUARE, ROMB, CIRCLE};
    enum class Direction  {LEFT, UP, RIGHT, DOWN, LEFT_UP, LEFT_DOWN, RIGHT_UP, RIGHT_DOWN, NO_MOVE};

    explicit Player(const QPoint& gridPosition, const QString& name, const QColor& color, const QString& imagePath);
    ~Player();

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) override;   

    void setGridPosition (const QPoint& gridPosition);
    void setDirection (const Direction& direction);
    void setImage (const QString& imagePath);
    void setShape (const Shape& shape);
    void setColor (const QColor& color);
    void setSize  (const QSize& size);
    void setName  (const QString& name);

    const QPoint& gridPosition();
    const Direction& direction();
    const QColor& color();
    const QSize& size();
    const QString& name();

    void setBlocked(int turns);
    int  blocked() const;
    void decreaseBlock();

    Hand* hand();
    void  takeOwnershipToken(OwnershipToken* ot);
    void  takeCard(Card* card);

    void  circle();

private:    
    QPainterPath pathForCurrentShape();


    QPoint  m_gridPosition;
    Direction m_direction;
    Hand* m_hand = nullptr;

    // Basic things: position, shape and name.
    Shape   m_shape;
    QSize   m_size;
    QColor  m_color;
    QImage  m_image;
    QString m_name;

    // Various statistics variables.
    int m_rounds;
    int m_blocked;
};

#endif // PLAYER_H
