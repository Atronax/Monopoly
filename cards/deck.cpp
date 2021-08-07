#include "deck.h"

#include <QPainter>
#include <QDebug>

Deck::Deck(DeckType deckType, int maxSize)
{
    setDeckType(deckType);
    createDeck(maxSize);
}

Deck::~Deck()
{
    clear();

    delete m_cards;
}

void Deck::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED (option);
    Q_UNUSED (widget);

    if (m_cards->isEmpty())
        return;

    // Deck frame
    painter->setPen(QPen(QBrush(Qt::black), 1.0f));
    painter->drawRect(rect());
    painter->setPen(QPen(QBrush(Qt::darkGray), 0.5f));
    painter->drawRect(rect().adjusted(3, 3, -3, -3));

    // Preparations for card drawings
    painter->setPen(QPen(Qt::lightGray, 1.0f));
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setFont(QFont("Truetypewriter PolyglOTT", 11));

    // Cards themselves
    int shift = 0;
    int margin = 3;
    for (int i = 0; i < m_cards->count(); ++i)
    {
        Card *card = m_cards->at(i);

        QRect borderRect = QRect(rect().x() + shift,     rect().y() + shift,      rect().width(),     rect().height());
        QRect  imageRect = QRect(rect().x() + shift + margin, rect().y() + shift + margin, rect().width() - margin, 0.69f*(rect().height() - 2*margin));
        QRect   nameRect = QRect(rect().x() + shift + margin, rect().y() + shift + 0.7f*(rect().height() - 2*margin), rect().width() - margin, 0.3f*(rect().height() - 2*margin));
        // QRect  descrRect = QRect(rect().x() + shift + margin, rect().y() + shift + 0.6f*(rect().height() - 2*margin), rect().width() - margin, 0.4f*(rect().height() - 2*margin));

        // qDebug() << "Actual Rect of the card " << i << " is: " << borderRect;
        // painter->drawRect(borderRect.adjusted(-3,-3,3,3));

        if (card->isFrontSide())
        {
            painter->drawImage(borderRect, card->imageFrontBG().scaled(borderRect.width(), borderRect.height()));
            painter->drawImage(imageRect , card->imageFrontFG().scaled(imageRect.width(),  imageRect.height()));
            painter-> drawText(nameRect  , card->name(), QTextOption(Qt::AlignCenter | Qt::AlignTop));
            // painter-> drawText(descrRect , card->description(), QTextOption(Qt::AlignCenter | Qt::AlignTop));
        }
        else
            painter->drawImage(borderRect, card->imageBack().scaled(borderRect.width(), borderRect.height()));

        shift += 2;
    }
}

void Deck::setDeckType(const Deck::DeckType &deckType)
{
    m_deckType = deckType;
}

const Deck::DeckType &Deck::deckType() const
{
    return m_deckType;
}

void Deck::add(Card *card)
{
    if (!m_cards)
    {
        qDebug() << "Cards vector has not been initialized yet.";
        return;
    }

    if (m_cards->count() == m_maxSize)
    {
        qDebug() << "No more cards can be placed into this deck.";
        return;
    }

    if (!m_cards->contains(card))
        m_cards->append(card);
}

void Deck::remove(Card *card)
{
    if (!m_cards)
    {
        qDebug() << "Cards vector has not been initialized yet.";
        return;
    }

    if (m_cards->count() == 0)
    {
        qDebug() << "There aren't any cards in this deck.";
        return;
    }

    if (m_cards->contains(card))
        m_cards->removeOne(card);
}

bool Deck::isEmpty()
{
    return m_cards->isEmpty();
}

bool Deck::has(Card *card)
{
    return m_cards->contains(card);
}

int Deck::count()
{
    return m_cards->count();
}

Card *Deck::peekTop()
{
    Card* card = nullptr;
    if (!m_cards->isEmpty())
        card = m_cards->last();

    return card;
}

Card *Deck::takeTop()
{
    Card* card = nullptr;
    if (!m_cards->isEmpty())
        card = m_cards->last();

    if (card)
        remove(card);

    return card;
}

void Deck::createDeck(int maxCards)
{
    m_maxSize = maxCards;
    m_cards = new QVector<Card*>();
}

void Deck::shuffleDeck()
{
    int iterations = 20;

    // To shuffle the deck, we take some cards and insert them in some place of the original deck.
    // This could be done, among other ways,
    // by gathering part of the original deck between from and to indexes to some other deck
    // and implementing a method to insert a subdeck into a deck, or
    // by selecting two cards at indexes where, where + 1, ... and from, from + 1, ...
    // and swapping them afterwards.

    // In this case, lets do something like this:
    // 1. Select random index in first part of the deck.
    // 2. Select random range in second part of the deck.
    // 3. Move through the sequence of indexes and swap the cards pair by pair.

    for (int iteration = 0; iteration < iterations; ++iteration)
    {
        int where = rand() % (m_cards->size() / 2);
        int from = m_cards->size() / 2 + rand() % (m_cards->size() - m_cards->size() / 2);
        int to = from + rand() % (m_cards->size() - from);

        for (int i = where, j = from; i < to - from, j < to; ++i, ++j)
        {
            Card* card1 = m_cards->at(i);
            Card* card2 = m_cards->at(j);

            m_cards->replace(i, card2);
            m_cards->replace(j, card1);
        }
    }
}

void Deck::clear()
{
    if (m_cards == nullptr)
    {
        qDebug() << "Cards vector has not been initialized yet.";
        return;
    }

    for (int i = 0; i < m_cards->count(); ++i)
        delete m_cards->at(i);

    m_cards->clear();
}

int Deck::maxSize()
{
    return m_maxSize;
}

void Deck::use()
{

}
