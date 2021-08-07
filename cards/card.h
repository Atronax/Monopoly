#ifndef CARD_H
#define CARD_H

#include <QGraphicsRectItem>
#include "helper/description.h"

class Player;

// Cards can inflict positive effect to its user or negative effect to opposite player.
// Another data file would help here to organize holding and restoring the descriptions.

// XML file could be built somewhat like this:
// <category id = "positive">
//  <card index = "some_index">
//   <type></type> // as string, needs some conversion method to actual type (or reflection)
//   <name></name>
//   <description></description>
//   <imagePath></imagePath> // path to image file, that will be used as foreground
//  </card>
//
//  ...
// </category>
// Also data class to store the information of one card, list to store all of these descriptions,
// methods to parse XML into dom or sax and grab the relevant data to create description objects,
// which later can be used as parameters of factory methods to generate the actual cards, which
// is pretty nice for editors.

// So lets stop on bonuses for now. Since there are positive cards as well as negative, we will
// separate the XML file into two categories. Lets say, there are these positive bonuses:
// * + TREASURE          - immediately gives the player some random count of gold.
// * + OVERTIME          - doubles the returns from players' companies for one circle.
// * + MASTERCHEF        - allows to make additional movement with the chance of its doubling without need to turn.
// * + FAST_AND_FURIOUS  - moves the player half ring forwards (or backwards, no matter).
// for example, if map is 8x8, and player stands on (2,0), he will move to symmetric position (5,7) and vice versa.
// * + BIRTHDAY          - other players give 5000 to current.
// * + SCIENTIST         - plus 1 star to random company.
// * + TOGETHER            - moves all the players using the same count of steps.

// Cards with negative effect can be used to harm opposite players:
// * + THIEF         - steals some random count of gold from random player.
// * + DIVERSION     - forces opposite player to walk backwards.
// * + SABOTAGE      - chance to stop opposite players' companies income for one circle.
// * + RAID          - chance to steal one of the opposite players' companies.
// * + BRIBE         - chance to put opposite player to jail for some turns.
// * + SNEAK         - immediately moves player to opposite, if he is ahead.
// * + SPY           - chance to learn technologies of other player and add the count of stars of his best company to one of the own companies.

class Card : public QGraphicsRectItem
{
public:
    enum class CardType {TREASURE, OVERTIME, MASTERCHEF, FAST_AND_FURIOUS, BIRTHDAY, SCIENTIST, TOGETHER,
                         THIEF, DIVERSION, SABOTAGE, RAID, BRIBE, SNEAK, SPY,
                         DEFAULT};

    Card(const QString& name, const QString& description, const QString& imagePath);
    Card(Description* cd);
    ~Card();

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) override;

    const CardType& cardType() const;
    const QString& name() const;
    const QString& description() const;
    const QImage& imageFrontBG() const;
    const QImage& imageFrontFG() const;
    const QImage& imageBack() const;

    void setName(const QString& name);
    void setDescription(const QString& description);
    void setForeground(const QString& imagePath);
    void setFrontSide(bool toFrontSide);
    void setBackground();

    Player* owner () const;
    bool hasOwner () const;
    void setOwner (Player* player);

    void setCardType (const CardType& cardType);
    CardType stringToType(const QString& name);
    QString typeToString(const CardType& type);

    void setThumbnailRegion (const QRectF& thumbnailRegion);
    const QRectF& thumbnailRegion() const;

    bool isFrontSide();
    void turnAround();
    void use();

private:

    CardType m_cardType;

    bool m_frontSide;

    const int CARD_WIDTH = 60;
    const int CARD_HEIGHT = 80;

    Player* m_owner = nullptr;
    QRectF  m_thumbnailRegion;

    QString m_name;
    QString m_description;
    QImage  m_imageForeground;
    QImage  m_imageCoverFront;
    QImage  m_imageCoverBack;
};

#endif // CARD_H
