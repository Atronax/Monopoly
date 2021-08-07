#include "card.h"

#include <QFileInfo>
#include <QPainter>
#include <QDebug>

#include "player/player.h"

Card::Card(const QString& name, const QString& description, const QString& imagePath)
{
    setName(name);
    setDescription(description);
    setBackground();
    setForeground("D:/monopoly/cards/" + imagePath.trimmed());
    setFrontSide(false);
}

Card::Card(Description *cd)
{
    if (cd->objectType() != Description::ObjectType::CARD)
    {
        qDebug() << "The description hasn't any connections with cards!";
        return;
    }

    setCardType(stringToType(cd->type().trimmed()));
    setName(cd->name().trimmed());
    setDescription(cd->description());
    setBackground();
    setForeground("d:/monopoly/cards/" + cd->imagePath().trimmed());
    setFrontSide(false);
}

Card::~Card()
{

}

QRectF Card::boundingRect() const
{
    return rect();
}

void Card::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // Deck class is responsible for drawing cards.

    // Draw image. Show name and description as a tooltip.
    // QImage image = (frontSide == true) ? m_imageFront : m_imageBack;
    // painter->drawImage(rect(), image);
}

const Card::CardType &Card::cardType() const
{
    return m_cardType;
}

Player* Card::owner() const
{
    return m_owner;
}

const QString &Card::name() const
{
    return m_name;
}

const QString &Card::description() const
{
    return m_description;
}

void Card::setName(const QString &name)
{
    m_name = name;
}

void Card::setDescription(const QString &description)
{
    m_description = description;
}

void Card::setForeground(const QString &imagePath)
{
    m_imageForeground = QImage(imagePath);
}

void Card::setFrontSide(bool toFrontSide)
{
    m_frontSide = toFrontSide;
}

void Card::setBackground()
{
    // default values for frontside and backside of the card
    m_imageCoverFront = QImage("d:/monopoly/cards/card_front.png");
    m_imageCoverBack  = QImage("d:/monopoly/cards/card_back.png");
}

bool Card::hasOwner() const
{
    return (m_owner != nullptr);
}

bool Card::isFrontSide()
{
    return m_frontSide;
}

void Card::turnAround()
{
    m_frontSide = !m_frontSide;
}

void Card::use()
{
    qDebug() << "Card " << name() << " have been used.";
}

Card::CardType Card::stringToType(const QString &typeString)
{
    qDebug() << "converting string " << typeString << " to actual type";

    QStringList SL = {"treasure", "overtime", "masterchef", "fast_and_furious", "birthday", "scientist", "together",
                      "thief", "diversion", "sabotage", "raid", "bribe", "sneak", "spy"};

    QList<CardType> TL = {CardType::TREASURE, CardType::OVERTIME, CardType::MASTERCHEF, CardType::FAST_AND_FURIOUS, CardType::BIRTHDAY, CardType::SCIENTIST, CardType::TOGETHER,
                          CardType::THIEF, CardType::DIVERSION, CardType::SABOTAGE, CardType::RAID, CardType::BRIBE, CardType::SNEAK, CardType::SPY};

    int index = -1;
    for (int i = 0; i < SL.count(); ++i)
    {
        if (SL.at(i) == typeString)
        {
            index = i;
            break;
        }
    }

    if (index >= 0)
        return TL.at(index);
    else
        return Card::CardType::DEFAULT;
}

QString Card::typeToString(const Card::CardType &type)
{
    QList<CardType> TL = {CardType::TREASURE, CardType::OVERTIME, CardType::MASTERCHEF, CardType::FAST_AND_FURIOUS, CardType::BIRTHDAY, CardType::SCIENTIST, CardType::TOGETHER,
                          CardType::THIEF, CardType::DIVERSION, CardType::SABOTAGE, CardType::RAID, CardType::BRIBE, CardType::SNEAK, CardType::SPY};

    QStringList SL = {"treasure", "overtime", "masterchef", "fast_and_furious", "birthday", "scientist", "together",
                      "thief", "diversion", "sabotage", "raid", "bribe", "sneak", "spy"};

    int index = -1;
    for (int i = 0; i < TL.count(); ++i)
    {
        if (TL.at(i) == type)
        {
            index = i;
            break;
        }
    }

    if (index >= 0)
        return SL.at(index);
    else
        return "";
}

const QImage &Card::imageFrontBG() const
{
    return m_imageCoverFront;
}

const QImage &Card::imageFrontFG() const
{
    return m_imageForeground;
}

const QImage &Card::imageBack() const
{
    return m_imageCoverBack;
}

void Card::setOwner(Player *player)
{
    m_owner = player;
}

void Card::setThumbnailRegion(const QRectF &thumbnailRegion)
{
    m_thumbnailRegion = thumbnailRegion;
}

void Card::setCardType(const Card::CardType &cardType)
{
    m_cardType = cardType;
}

const QRectF &Card::thumbnailRegion() const
{
    return m_thumbnailRegion;
}
