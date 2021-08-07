#include "player.h"

#include <QPainter>
#include <QFileInfo>
#include <QDebug>

Player::Player(const QPoint& gridPosition, const QString& name, const QColor& color, const QString &imagePath)
{
    setGridPosition(gridPosition);
    setSize(QSize(0,0));
    setName(name);
    setColor(color);
    setImage(imagePath);

    m_hand = new Hand(this);
    m_rounds = 0;
    m_blocked = 0;
}

Player::~Player()
{

}

QRectF Player::boundingRect() const
{
    return rect(); // QRect(m_gridPosition.x() * m_size.width(), m_gridPosition.y() * m_size.height(), m_size.width(), m_size.height());
}

void Player::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // 1. Prepare instuments for drawing
    QPen pen = QPen(QBrush(m_color.darker(40)), 3);
    QBrush brush = QBrush(m_color);
    painter->setRenderHint(QPainter::Antialiasing);

    // 2. Calculate path.
    QPainterPath path = pathForCurrentShape();

    // 3. Draw paths and fill them using selected pens and brushes.
    painter->setPen(pen);
    painter->drawPath(path);
    painter->fillPath(path, brush);
}

const QPoint &Player::gridPosition()
{
    return m_gridPosition;
}

const Player::Direction &Player::direction()
{
    return m_direction;
}

const QColor &Player::color()
{
    return m_color;
}

const QSize &Player::size()
{
    return m_size;
}

const QString &Player::name()
{
    return m_name;
}

void Player::setBlocked(int turns)
{
    m_blocked = turns;
}

int Player::blocked() const
{
    return m_blocked;
}

Hand *Player::hand()
{
    return m_hand;
}

void Player::takeOwnershipToken(OwnershipToken *ot)
{
    if (m_hand->gold() >= ot->buyingCost())
    {
        m_hand->pay(ot->buyingCost());
        m_hand->addToken(ot);

        ot->setOwner(this);
    }
    else
        qDebug() << name() << " hasn't enough money to make this deal.";
}

void Player::takeCard(Card *card)
{
    if (card)
    {
        m_hand->addCard(card);
        card->setOwner(this);

        qDebug() << "There are " << m_hand->m_cards->count() << " cards in hands of " << name();
    }
    else
        qDebug() << "Card is nullptr.";
}

void Player::circle()
{
    ++m_rounds;
}

QPainterPath Player::pathForCurrentShape()
{
    QPainterPath shape;

    switch (m_shape)
    {
    case Shape::SQUARE:
        shape.addRect(rect().adjusted(30,30,-30,-30));
        break;

    case Shape::CIRCLE:
        shape.addEllipse(rect().adjusted(30,30,-30,-30));
        break;

    case Shape::ROMB:
        int margin = 25;                                                                         // shift from the border

        shape.moveTo(rect().x() + rect().width() / 2.0f, rect().y() + margin);                   // center of upper border line
        shape.lineTo(rect().x() + rect().width() - margin, rect().y() + rect().height() / 2.0f); // center of right border line
        shape.lineTo(rect().x() + rect().width() / 2.0f, rect().y() + rect().height() - margin); // center of lower border line
        shape.lineTo(rect().x() + margin, rect().y() + rect().height() / 2.0f);                  // center of left  border line
        shape.lineTo(rect().x() + rect().width() / 2.0f, rect().y() + margin);                   // center of upper border line


        break;
    }

    if (shape.isEmpty())
        shape.addRect(rect()); // default case

    return shape;
}

void Player::setGridPosition(const QPoint &gridPosition)
{
    m_gridPosition = gridPosition;
}

void Player::setDirection(const Player::Direction &direction)
{
    m_direction = direction;
}

void Player::setImage(const QString &imagePath)
{
    if (QFileInfo::exists(imagePath))
    {
        m_image = QImage(imagePath).scaled(rect().width(), rect().height());
        update();
    }
}

void Player::setShape(const Player::Shape &shape)
{
    m_shape = shape;
    update();
}

void Player::setColor(const QColor &color)
{
    m_color = color;
    update();
}

void Player::setSize(const QSize &size)
{
    m_size = size;
}

void Player::setName(const QString &name)
{
    m_name = name;
}

void Player::decreaseBlock()
{
     --m_blocked;
}
