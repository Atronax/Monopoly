#ifndef HAND_H
#define HAND_H

#include <QGraphicsRectItem>
#include <QList>

#include "nodes/tokens/actiontoken.h"
#include "nodes/tokens/ownershiptoken.h"
#include "cards/card.h"

// Remark: in case of circular references (for example, a -> b -> c -> a,
// where -> means left class includes right class) and guard clauses use
// just use simple predeclaration of the needed class and include in cpp

// Hand is a class, that is used for storage, rendering and interaction with gathered tokens and cards.
// There should be:
// - the list of tokens (methods to add new token to one of the lists and to remove it from there),
// - the list of bonus cards,
// - the rendering subsystem, that uses 1 of the 4 positions (on the top, bottom, left or right side of the scene: (enum?): TOP, RIGHT, BOTTOM, LEFT)

// Also:
// The trading subsystem, that activates when player hits exchange token.
// It should allow trading between hands of different players.
// It would use drag and drop to allow moving tokens from hand space to exchange grid.
// After that, base price will be calculated and player will have the ability to set additional percentage if needed.
// Detailed info in tokens class.

class Hand : public QGraphicsRectItem
{
public:
    enum class Side {UPPER, RIGHT, LOWER, LEFT, DEFAULT};
    enum class Layout {HORIZONTAL, VERTICAL};

    explicit Hand(Player *player);
    ~Hand();

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    // UI relative stuff
    void setSide(QPair<Side, QPainterPath>* side);

    // Interaction with tokens
    void addToken (OwnershipToken* token);
    void removeToken (Token* token);

    Token* takeTokenAtPosition(const QPointF& pixelPosition);
    Card*  takeCardAtPosition (const QPointF& pixelPosition);

    // Interaction with cards
    void addCard (Card* card);
    void removeCard (Card* card);

    // Interaction with gold
    const int& gold() const;
    void receive(int gold);
    bool pay    (int gold);

    int returns();

    bool isIncomeDoubled();
    bool isIncomeStopped();

    void setIncomeDoubled(bool value);
    void setIncomeStopped(bool value);

    OwnershipToken* randomCompany();
    void upgradeRandomCompany(int stars);
    int  topCompanyUpgradeLevel();

    void clear();

public:
    // Cleaning    
    void clearActionTokens();
    void clearOwnershipTokens();
    void clearCards();

    // UI
    Side m_side = Side::DEFAULT;
    QPainterPath m_shape;
    Layout m_layout;

    // Hand knows his body :)
    Player* m_player = nullptr;

    // Features
    // 1. Gold: Used to buy companies, to make upgrades. When some specific number of circles passed, it also indicates the winner.
    // 2. Double income: If true, players companies return double income on next node with a start token.
    // 3. Stopped income: If true, players companies won't return any income for 1 circle.
    int m_gold;

    bool m_isIncomeDoubled = false;
    bool m_isIncomeStopped = false;

    // Storages
    QList<ActionToken*> *m_actionTokens = nullptr;
    QList<OwnershipToken*> *m_ownershipTokens = nullptr;
    QList<Card*> *m_cards = nullptr;
};

#endif // HAND_H
