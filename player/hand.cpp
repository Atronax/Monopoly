#include "hand.h"

#include <QGraphicsScene>
#include <QPainter>
#include <QDebug>

#include "player.h"

Hand::Hand(Player *player)
{
    // Its a miracle! Creating new lists with tokens leads to CRASH after clicking UPGRADE button!
    // What does that mean? Maybe, there won't be UPGRADE ability after gathering new TOKENS? Maybe! Joke...
    m_player = player;

    // When all three lists are created, upgrade button begins to show devil dance.
    // Each combo of two works well, as well as each single one.
    // Seems it's some sort of magic, that should be uncovered.
    // Or just remove action tokens list at all will help. Will see.

    m_actionTokens    = new QList<ActionToken*>();
    m_ownershipTokens = new QList<OwnershipToken*>();
    m_cards           = new QList<Card*>();
}

Hand::~Hand()
{
    clearActionTokens();
    clearOwnershipTokens();
    clearCards();

    delete m_actionTokens;
    delete m_ownershipTokens;
    delete m_cards;
}

QRectF Hand::boundingRect() const
{
    // qDebug() << "Hand BR: " << m_shape.boundingRect();
    return m_shape.boundingRect();
}

void Hand::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // Do nothing, if the side has not been set yet.
    if (m_side == Side::DEFAULT)
        return;

    // 0. Prepare drawing instruments.
    QPen defaultPen = painter->pen();
    QPen pathPen    = QPen(QBrush(Qt::white), 2);
    QPen regionsPen = QPen(QBrush(Qt::white), 1);
    QBrush defaultBrush   = painter->brush();
    QBrush fillBrush    = QBrush(QColor("#eee"));
    QBrush regionsBrush = QBrush(QColor("#111"));
    QFont  basicFont = QFont("Comic Sans", 10);
    QFont  cardsFont = QFont("Comic Sans", 7);

    painter->setFont(basicFont);
    painter->setRenderHint(QPainter::Antialiasing);

    // 1. First, we should set the correct region for the hand and placement order for its objects.
    // In this case, we use the *m_side* and *m_layout* enum variables to choose the right side and order.
    // Having these, we can generate actual QPainterPath object using corresponding method and use it a a region to draw all other things.

    // qDebug() << "Region: " << m_shape;

    painter->setPen(pathPen);
    painter->drawPath(m_shape);
    painter->fillPath(m_shape, fillBrush);

    // 2. As for objects' layout, that depends on the shape itself.
    // If width greater than height, we should have horizontal placement of items.
    // And vice versa: if height is greater than width, we must set items in vertical order.
    // To do so, we could make two separate function for ordering and use some sort of enum to choose between them.
    // Or make one function with a switch operator, but that will affect readability a little.

    // Lets tear the shape into fragments, which will be later used to draw elements there.
    // First, we'll need place to fit gold information. Pretend it is simple text, that should be placed in rectangle gR.
    // We do nothing with triangle zones for now. As for width and height values, it depends.
    // There should also be some spaces between rectangles, in which the objects will be drawn,
    // Lets name it MARGIN in our example and set it to some value.
    // The rest of drawing zones are initialized in the same way.

    // Basic preparations
    int MARGIN_RECTS = 10;
    int MARGIN_TRIANGLE = 50;
    int MARGIN_BORDERS = 3;

    QRectF  bR = boundingRect();
    QPointF tL = boundingRect().topLeft();

    // Gold information block.
    QRectF  gR = QRectF(tL.x() + MARGIN_BORDERS, tL.y() + MARGIN_TRIANGLE, bR.width() - MARGIN_BORDERS*2, MARGIN_TRIANGLE);
    QPointF gBL = gR.bottomLeft() - QPointF(MARGIN_BORDERS, 0);

    QRectF nR = QRectF(gBL.x() + MARGIN_BORDERS, gBL.y() + MARGIN_RECTS, bR.width() - MARGIN_BORDERS*2, 1*bR.height()/7 - MARGIN_TRIANGLE*2);
    QPointF nBL = nR.bottomLeft() - QPointF(MARGIN_BORDERS, 0);

    // Collected tokens block.
    QRectF  tR = QRectF(nBL.x() + MARGIN_BORDERS, nBL.y() + MARGIN_RECTS, bR.width() - MARGIN_BORDERS*2, 5*bR.height()/11 - MARGIN_TRIANGLE*2);
    QPointF tBL = tR.bottomLeft() - QPointF(MARGIN_BORDERS, 0);

    // Collected cards block.
    QRectF cR = QRectF(tBL.x() + MARGIN_BORDERS, tBL.y() + MARGIN_RECTS, bR.width() - MARGIN_BORDERS*2,  4*bR.height()/9 - MARGIN_TRIANGLE*2 + MARGIN_RECTS*3);

    // qDebug() << "G: " << m_gold;

    // Draw rectangles
    painter->setPen(regionsPen);
    painter->setBrush(regionsBrush);
        painter->drawRoundedRect(gR, 4, 4);    
        painter->drawRoundedRect(tR, 4, 4);
        painter->drawRoundedRect(cR, 4, 4);
        painter->drawRoundedRect(nR, 4, 4);

    // Draw actual objects
    // 1. Rectangle gR is meant to be storage for gold information.
    // Lets make the string using the relevant value and place it in its center.
    painter->setPen(pathPen);
        painter->drawText(gR, QString("Gold: %1").arg(m_gold), QTextOption(Qt::AlignCenter));
    painter->setPen(m_player->color());
        painter->drawText(nR, m_player->name(), QTextOption(Qt::AlignCenter));

    // 2. Rectangle tR should contain all tokens collected by the player.
    // Since they are stored in a list, we check its existence, walk through each OT and call its paint event.
    // The problem: why they don't recognize negative scene coordinates? Check!
    painter->setBrush(defaultBrush);
    if (m_ownershipTokens)
    {
        for (int i = 0; i < m_ownershipTokens->count(); ++i)
        {
            // Set the rectangle for each OT and draw it.
            OwnershipToken* token = m_ownershipTokens->at(i);

            int MARGIN_TOKENS = 5;
            int size = tR.width() - MARGIN_TOKENS * 2;
            int spacing = tR.width() - MARGIN_TOKENS * 2 + 5;

            // Calculate rectangle for token thumbnail, that should be placed somewhere in the overall tokens rectangle.
            QRectF tR_tt = QRectF (tR.x() + MARGIN_TOKENS, tR.y() + MARGIN_TOKENS + spacing * i, size, size);
            token->setThumbnailRegion(tR_tt);

            bool tokenHasImage  = !(token->image().isNull());
            bool tokenIsVisible = (tR_tt.bottomRight().y() < tR.bottomRight().y());

            if (tokenIsVisible && tokenHasImage)
            {
                painter->fillRect(tR_tt, Qt::lightGray);
                painter->drawImage(tR_tt, token->image());
                painter->drawRect(tR_tt);
            }
        }
    }

    painter->setFont(cardsFont);
    if (m_cards)
    {
        for (int j = 0; j < m_cards->count(); ++j)
        {
            Card* card = m_cards->at(j);

            int MARGIN_TOKENS = 5;
            int width = cR.width() - MARGIN_TOKENS * 2;
            int height = 1.5f * width;
            int spacing = cR.width() - MARGIN_TOKENS * 2 + 5;

            QRectF cR_ct = QRectF(cR.x() + MARGIN_TOKENS, cR.y() + MARGIN_TOKENS + spacing * j, width, height);
            QRectF cR_ct_image = QRectF(cR_ct.x(), cR_ct.y(), cR_ct.width(), 2.0f * cR_ct.height() / 3.0f);
            QRectF cR_ct_name  = QRectF(cR_ct.x(), cR_ct.y() + 2.0f * cR_ct.height() / 3.0f, cR_ct.width(), 1.0f * cR_ct.height() / 3.0f);
            card->setThumbnailRegion(cR_ct);

            painter->drawRect(cR_ct);
            qDebug() << "Card has front image: " << !card->imageFrontFG().isNull();
            if (!card->imageFrontFG().isNull())
                painter->drawImage(cR_ct_image, card->imageFrontFG());

            if (painter == nullptr)
                qDebug() << "no painter!";

            painter->drawText(cR_ct_name, card->name(), QTextOption(Qt::AlignCenter));
        }
    }
}

void Hand::addToken(OwnershipToken *token)
{
    Q_ASSERT_X(m_ownershipTokens != nullptr, "Hand::addToken(OwnershipToken)", "Ownership tokens list has not been initialized yet.");
    if (!m_ownershipTokens->contains(token))
        m_ownershipTokens->append(token);
}

void Hand::setSide(QPair<Side, QPainterPath>* side)
{
    // Can be set, if this side is not currently used by any other hand. For this we could check the hands of all the players.
    // Instead of just setting new active side for this hand, we also change the layout of items,
    // which will be used later for painting the objects in correct positions and order.

    m_side = side->first;
    switch (m_side)
    {
        case Side::UPPER: case Side::LOWER:
            m_layout = Layout::HORIZONTAL;
            break;

        case Side::LEFT: case Side::RIGHT:
            m_layout = Layout::VERTICAL;
            break;

        case Side::DEFAULT:
        break;
    }

    m_shape = side->second;
}

void Hand::removeToken(Token *token)
{
    OwnershipToken* ot = dynamic_cast<OwnershipToken*>(token);
    if (ot && m_ownershipTokens->contains(ot))
        m_ownershipTokens->removeOne(ot);
}

Token *Hand::takeTokenAtPosition(const QPointF &pixelPosition)
{
    Token* token = nullptr;

    for (int i = 0; i < m_ownershipTokens->count(); ++i)
    {
        OwnershipToken* ot = m_ownershipTokens->at(i);
        if (ot->thumbnailRegion().contains(pixelPosition))
        {
            token = ot;
            break;
        }
    }

    return token;
}

Card *Hand::takeCardAtPosition(const QPointF &pixelPosition)
{
    Card* card = nullptr;

    for (int i = 0; i < m_cards->count(); ++i)
    {
        Card* toCheck = m_cards->at(i);
        if (toCheck->thumbnailRegion().contains(pixelPosition))
        {
            card = toCheck;
            break;
        }
    }

    return card;
}

void Hand::addCard(Card *card)
{
    Q_ASSERT_X(m_cards != nullptr, "Hand::addCard", "Cards list has not been initialized yet.");
    if (!m_cards->contains(card))
        m_cards->append(card);
}

void Hand::removeCard(Card *card)
{
    if (m_cards->contains(card))
        m_cards->removeOne(card);
}

const int &Hand::gold() const
{
    return m_gold;
}

void Hand::receive(int gold_toReceive)
{
    Q_ASSERT_X(gold_toReceive > 0, "Hand::receive", "Gold_toReceive should be greater than zero.");
    m_gold += gold_toReceive;
}

bool Hand::pay(int gold_toPay)
{
    Q_ASSERT_X(gold_toPay > 0, "Hand::pay", "Gold_toPay should be greater than zero.");
    if (gold_toPay > m_gold)
    {
        qDebug() << "There is no enough money to make this deal!";
        return false;
    }
    else
    {
        m_gold -= gold_toPay;
        return true;
    }
}

int Hand::returns()
{
    int returns = 0;

    for (int i = 0; i < m_ownershipTokens->count(); ++i)
    {
        OwnershipToken* company = m_ownershipTokens->at(i);
        returns += company->income();
    }

    if (m_isIncomeDoubled)
    {
        returns *= 2;
        setIncomeDoubled(false);
    }

    if (m_isIncomeStopped)
    {
        returns *= 0;
        setIncomeStopped(false);
    }

    return returns;
}

bool Hand::isIncomeDoubled()
{
    return m_isIncomeDoubled;
}

bool Hand::isIncomeStopped()
{
    return m_isIncomeStopped;
}

void Hand::setIncomeDoubled(bool value)
{
    qDebug() << "Income of OT is doubled for 1 circle.";

    m_isIncomeDoubled = value;
}

void Hand::setIncomeStopped(bool value)
{
    m_isIncomeStopped = value;
}

OwnershipToken *Hand::randomCompany()
{
    if (m_ownershipTokens->count() > 0)
    {
        int index = rand() % m_ownershipTokens->count();
        return m_ownershipTokens->at(index);
    }
    else
    {
        qDebug() << "This player hasn't any companies yet.";
        return nullptr;
    }
}

int Hand::topCompanyUpgradeLevel()
{
    int top = 0;

    for (int i = 0; i < m_ownershipTokens->count(); ++i)
    {
        OwnershipToken* OT = m_ownershipTokens->at(i);
        if (OT->upgradeLevel() > top)
            top = OT->upgradeLevel();
    }

    return top;
}

void Hand::upgradeRandomCompany(int stars)
{
    Q_ASSERT_X(stars >= 1 && stars <= 3, "Hand::upgradeCompany", "Stars parameter should be in range [1;3].");

    if (m_ownershipTokens->count() == 0)
    {
        qDebug() << "There is no company to upgrade";
        return;
    }

    int index = rand() % m_ownershipTokens->count();

    OwnershipToken* OT = m_ownershipTokens->at(index);
    for (int i = 0; i < stars; ++i)
        OT->upgrade(true);
}

void Hand::clear()
{
    qDebug() << "in Hand::clear";
    qDebug() << "AT count: " << m_actionTokens->count();
    qDebug() << "OT count: " << m_ownershipTokens->count();
    qDebug() << "C  count: " << m_cards->count();

    clearActionTokens();
    clearOwnershipTokens();
    clearCards();
}

void Hand::clearActionTokens()
{
    if (!m_actionTokens->isEmpty())
    {
        for (int i = 0; i < m_actionTokens->count(); ++i)
            delete m_actionTokens->at(i);

        m_actionTokens->clear();

        qDebug() << "Hands :: AT list cleared";
        qDebug() << "AT count: " << m_actionTokens->count();
    }
}

void Hand::clearOwnershipTokens()
{
    if (!m_ownershipTokens->isEmpty())
    {
        // Leave deletion to table instance or use shared pointer.
        // delete m_ownershipTokens->at(i); leads to crash, because table instance tries to use it afterwards.
        for (int i = 0; i < m_ownershipTokens->count(); ++i)
            m_ownershipTokens->removeAt(i);

        m_ownershipTokens->clear();

        qDebug() << "Hands :: OT list cleared";
        qDebug() << "OT count: " << m_ownershipTokens->count();
    }
}

void Hand::clearCards()
{
    if (!m_cards->isEmpty())
    {
        for (int i = 0; i < m_cards->count(); ++i)
            m_cards->removeAt(i);

        m_cards->clear();

        qDebug() << "Hands :: Cards list cleared";
    }
}
