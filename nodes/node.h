#ifndef NODE_H
#define NODE_H

#include <QGraphicsRectItem>

#include <QToolTip>
#include <QString>
#include <QImage>
#include <QList>

#include "tokens/token.h"

class Node : public QGraphicsRectItem
{

public:
    Node();
    Node(const QPoint& gridPosition);
    Node(const Node& other);
    ~Node();

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) override;

    void setActive (bool value);
    bool isActive();

    void setGridPosition (const QPoint& gridPosition);
    const QPoint& gridPosition() const;

    void setToken (Token* token);
    Token* token() const;

    friend QDataStream& operator<< (QDataStream&, const Node&);
    friend QDataStream& operator>> (QDataStream&, Node&);

private:
    void setupShape (const QPoint& gridPosition, const QSize& size);
    void drawNode   (QPainter *painter);
    void drawToken  (QPainter *painter);    
    void clearToken ();

    bool   m_isActive = false;
    QPoint m_position;
    Token* m_token = nullptr;

};

#endif // NODE_H
