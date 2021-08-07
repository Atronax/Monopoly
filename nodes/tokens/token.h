#ifndef TOKEN_H
#define TOKEN_H

#include <QGraphicsRectItem>

// Token are objects, that may be placed on any node of the scene.
// This is base class for all types of tokens. They are:

// - ownership token     *** (define ownership of some companies)
// This is subclass of token class, that has:
// * name of the company
// * description of the company
// * image of the company
// * cost to buy (lv0) : if player hits the node and decides to buy the ownership token,
// * cost to upgrade (to lv1, lv2, lv3) : player gets this ability each time he hits node repeatedly
// * profit per circle (lv0, lv1, lv2, lv3): player get the bonus each time he hits start node)
// * current upgrade level

// - exchange token       *** (allows to buy\sell ownership tokens or bonus cards)
// This is subclass of token class, that:
// * opens modal exchange window
// * player can place one ownership token for sell and put the price in range of {110%*base_price, 130%*base_price};
// * other players may buy the lot
// * if players won't buy the lot, ownership token is sold for 110% of the base_price and moves to its place on table.

// - double move token    *** (allows to make additional movement on a single turn)
// - move backwards token *** (forces to make move backwards)
// - prison token         *** (forces to stay there for additional turn)
// - go to start token    *** (allows pass all nodes and move to start node immediately)
// - start token          *** (represents beginning node of every circle)

// [st][ot][ot][ot][dm][ot][ot][ot][ts]
// [ot]****************************[ot]
// [ot]****************************[ot]
// [ot]****************************[ot]
// [sb]****************************[c-]
// [ot]****************************[ot]
// [ot]****************************[ot]
// [ot]****************************[ot]
// [et][ot][ot][ot][c+][ot][ot][ot][pr]

class Token : public QGraphicsRectItem
{
public:
    Token();
    Token(const QString& name, const QString& description, const QString& imagePath);
    Token(const Token& other);
    virtual ~Token();

    QRectF boundingRect() const override;

    void setName         (const QString& name);
    void setImage        (const QString& imagePath);
    void setDescription  (const QString& description);

    const QString& name() const;
    const QImage&  image() const;
    const QString& imagePath() const;
    const QString& description() const;

    QString toString();

    void operator= (const Token& rhs);

    friend QDataStream& operator<< (QDataStream&, const Token&);
    friend QDataStream& operator>> (QDataStream&, Token&);

    virtual void activate();

private:
    QString m_name;
    QString m_description;
    QString m_imagePath;
    QImage  m_image;
};

#endif // TOKEN_H
