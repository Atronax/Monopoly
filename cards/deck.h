#ifndef DECK_H
#define DECK_H

#include <QGraphicsRectItem>
#include <QVector>

#include "card.h"

class Deck : public QGraphicsRectItem
{
public:
    enum class DeckType {POSITIVE, NEGATIVE};

    Deck(DeckType deckType, int maxSize);
    ~Deck();

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) override;

    void setDeckType(const DeckType& deckType);
    const DeckType& deckType() const;

    void add    (Card* card);
    void remove (Card* card);
    bool has (Card* card);
    bool isEmpty();
    int  maxSize();
    int  count  ();
    void clear  ();

    Card* peekTop();
    Card* takeTop();    
    void use();

private:
    void createDeck(int count);
    void shuffleDeck();

    DeckType m_deckType;

    int m_maxSize;
    QVector<Card*> *m_cards = nullptr;

};

#endif // DECK_H
