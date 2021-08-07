#include "node.h"

#include <QPainter>
#include <QDebug>

Node::Node()
    : QGraphicsRectItem()
{
    setGridPosition(QPoint(0,0));
}

Node::Node(const QPoint& gridPosition)
    : QGraphicsRectItem()
{
    setGridPosition(gridPosition);
}

Node::Node(const Node &other)
    : QGraphicsRectItem()
{
    setGridPosition(other.gridPosition());

}

Node::~Node()
{
    clearToken();
}

QRectF Node::boundingRect() const
{
    return rect();
}

void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    drawNode(painter);
    if (m_token)
        drawToken(painter);
}

void Node::setActive(bool value)
{
    m_isActive = value;
}

void Node::setGridPosition(const QPoint &p)
{
    m_position = p;
}

void Node::setToken(Token* token)
{    
    m_token = token;
}

bool Node::isActive()
{
    return m_isActive;
}

const QPoint &Node::gridPosition() const
{
    return m_position;
}

Token* Node::token() const
{
    return m_token;
}

void Node::drawNode(QPainter* painter)
{
    QRectF r = rect();
//    if (isActive())
//        r = r.adjusted(-10,-10,10,10);

    painter->setRenderHints(QPainter::Antialiasing);

    QPainterPath path;
    path.addRect(r);
    path.addRect(r.adjusted(7,7,-7,-7));

    painter->setPen(QPen(QBrush(Qt::white), 2.f));
    painter->drawPath(path);
    painter->fillPath(path, QBrush(Qt::black));
}

void Node::drawToken(QPainter* painter)
{
    // 1. Draw rectangle and fill it with a corresponding image.
    // qDebug() << "Node drawToken:: size of image is " << m_token->image().size();
    QRectF r = rect().adjusted(10, 10, -10, -10);

    painter->drawRect(r);
    painter->fillRect(r, Qt::lightGray);
    QImage img = m_token->image();
    if (!img.isNull())
        painter->drawImage(r, img);

    // 2. Draw token name.
    //    if (!m_token->name().isNull())
    //        painter->drawText(QRectF(r.x(), r.y(), r.width(), r.height() / 4.0f), m_token->name(), QTextOption(Qt::AlignTop | Qt::AlignCenter));

    // 3. Draw grid position string.
    painter->drawText(QRectF(r.x() + 17, r.y() + 3.3f * r.height() / 4.0f, r.width(), r.height() / 4.0f),
                      QString("(%1,%2)").arg(gridPosition().x()).arg(gridPosition().y()),
                      QTextOption(Qt::AlignTop | Qt::AlignLeft));
}

void Node::clearToken()
{
    if (m_token)
    {
        delete m_token;
        m_token = nullptr;
    }
}

QDataStream& operator<< (QDataStream &out, const Node &n)
{
    bool tokenAvailable = false;
    if (n.token() != nullptr)
        tokenAvailable = true;

    out << n.m_position << tokenAvailable;

    if (tokenAvailable)
        out << *n.m_token;

    return out;
}

QDataStream& operator>> (QDataStream &in, Node &n)
{
    QPoint position;
    bool   tokenAvailable;

    in >> position >> tokenAvailable;

    n.setGridPosition(position);

    if (tokenAvailable)
    {        
        Token token;
        in >> token;

        qDebug() << "token AVAILABLE: " << token.name() << ":" << token.description() << ":" << token.imagePath();

        n.clearToken();
        qDebug() << "token CLEARED";
        n.setToken(new Token (token.name(), token.description(), token.imagePath()));

        qDebug() << "token SET";
    }

    return in;
}
