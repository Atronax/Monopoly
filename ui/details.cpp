#include "details.h"

#include <QGraphicsScene>
#include <QPainter>
#include <QDebug>

#include "player/player.h"
#include "ui/uielementfactory.h"

Details::Details()
{
}

Details::~Details()
{
    deleteButtons();
}

QRectF Details::boundingRect() const
{
    return rect();
}

void Details::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (m_actionToken || m_ownershipToken || m_card)
        drawBackground(painter);

    if (m_actionToken)    drawAT(painter);
    if (m_ownershipToken) drawOT(painter);
    if (m_card)           drawCard(painter);
}

void Details::setToken(Token *token)
{
    m_actionToken    = dynamic_cast<ActionToken*>   (token);
    m_ownershipToken = dynamic_cast<OwnershipToken*>(token);

    qDebug() << QString("Action token: %1. Ownership token: %2.").arg(m_actionToken != nullptr).arg(m_ownershipToken != nullptr);
}

void Details::setCard(Card *card)
{
    m_card = card;
}

ActionToken *Details::AT() const
{
    return m_actionToken;
}

OwnershipToken *Details::OT() const
{
    return m_ownershipToken;
}

Card *Details::card() const
{
    return m_card;
}
void Details::setPlayer(Player *player)
{
    m_currentPlayer = player;
}

UIElement *Details::buttonAtPosition(const QPoint &pixelPosition)
{
//    qDebug() << "PP:  " << pixelPosition;
//    qDebug() << "BBR: " << m_buyButton->rect();

    if (!isActive())
        return nullptr;

    // Returns button, if it is created, it is visible, it's shape contains specific point. And nullptr in rest of cases.
    if (m_buyButton     && !m_buyButton->isHidden()     && m_buyButton    ->boundingRect().contains(pixelPosition)) return m_buyButton;
    if (m_upgradeButton && !m_upgradeButton->isHidden() && m_upgradeButton->boundingRect().contains(pixelPosition)) return m_upgradeButton;
    if (m_useButton     && !m_useButton->isHidden()     && m_useButton    ->boundingRect().contains(pixelPosition)) return m_useButton;
    if (m_closeButton   && !m_closeButton->isHidden()   && m_closeButton  ->boundingRect().contains(pixelPosition)) return m_closeButton;

    return nullptr;
}

void Details::clearSelection()
{
    m_actionToken = nullptr;
    m_ownershipToken = nullptr;
    m_card = nullptr;

    // update(rect());
}

void Details::createButtons()
{        
    // This is lazy initialization, which invokes just once when first token met and set as an inner member variable. Creates both buttons and fills their details.
    if (m_buttonsCreated == false)
    {
        QSize szBuy (80,30), szUpgrade (120,30), szUse (80,30), szClose (15,15);

        QPoint ptBuy     (rect().x() + rect().width() - szBuy.width() * 2 - 30, rect().y() + rect().height() - szBuy.height() - 10);
        QPoint ptUpgrade (rect().x() + rect().width() - szUpgrade.width() - 10, rect().y() + rect().height() - szUpgrade.height() - 10);
        QPoint ptUse     (rect().x() + rect().width() - szUse.width() * 2 - 30, rect().y() + rect().height() - szUse.height() - 10);
        QPoint ptClose   (rect().x() + rect().width() - szClose.width()   - 2, rect().y() + 2);

        m_buyButton     = UIElementFactory::create(UIElement::Role::BUTTON, "Buy",     szBuy,     ptBuy);
        m_upgradeButton = UIElementFactory::create(UIElement::Role::BUTTON, "Upgrade", szUpgrade, ptUpgrade);
        m_useButton     = UIElementFactory::create(UIElement::Role::BUTTON, "Use",     szUse,     ptUse);
        m_closeButton   = UIElementFactory::create(UIElement::Role::BUTTON, "X",       szClose,   ptClose);

        m_buttonsCreated = true;
    }
}

void Details::deleteButtons()
{    
    delete m_buyButton;
    delete m_upgradeButton;
    delete m_useButton;
    delete m_closeButton;

    m_buyButton = nullptr;
    m_upgradeButton = nullptr;
    m_useButton = nullptr;
    m_closeButton = nullptr;
}

void Details::hideButtons()
{
    hideButton(m_buyButton);
    hideButton(m_upgradeButton);
    hideButton(m_useButton);
    // hideButton(m_closeButton);
}

void Details::hideButton(UIElement* button)
{
    if (!button)
    {
        qDebug() << "This button was not created yet.";
        return;
    }

    if (scene() && !button->isHidden())
    {        
        button->setHidden(true);        
        scene()->removeItem(button);
    }
    else
        qDebug() << "There is no active pointer to the scene currently.";
}

void Details::showButton(UIElement *button)
{
    if (!button)
        qDebug() << "Button is nullptr.";

    if (scene() && button->isHidden())
    {
        button->setHidden(false);
        scene()->addItem(button);
    }
    else
        qDebug() << "There is no active pointer to the scene currently.";
}

void Details::showButtons()
{
    // Hide all buttons.
    // hideButtons();

    // Show buy or upgrade button for OT, depending on its owner.
    if (m_ownershipToken)
    {
        if (m_ownershipToken->hasOwner())
        {
            if (m_ownershipToken->owner() == m_currentPlayer)
            {
                m_upgradeButton->setText(upgradeMessage());
                showButton(m_upgradeButton);
            }
        }
        else        
            showButton(m_buyButton);
    }

    // Show use button for C.
    if (m_card)    
        showButton(m_useButton);    

    showButton(m_closeButton);
}

void Details::drawBackground(QPainter *painter)
{
    QPen borderPen = QPen(QBrush(Qt::darkGray), 3.0f);
    QColor backgroundColor = QColor("#333"); backgroundColor.setAlpha(200);

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(borderPen);
    painter->drawRect(rect());
    painter->fillRect(rect(), backgroundColor);

    if (m_closeButton)
        m_closeButton->paint(painter, nullptr, nullptr);
}

void Details::drawAT(QPainter* painter)
{
    // 1. Prepare drawing instruments    
    QPen textPen = QPen(Qt::white);

    // 2. Draw action token details
    painter->setPen(textPen);
    painter->setFont(QFont("Comic Sans", 7));
    painter->drawImage(QRectF(rect().x(),       rect().y(),      300, 300), m_actionToken->image());
    painter->drawText(QRectF (rect().x() + 310, rect().y(),      190,  20), m_actionToken->name(), QTextOption(Qt::AlignCenter));
    painter->drawLine(rect().x() + 330, rect().y() + 20, rect().x() + 480, rect().y() + 20);
    painter->drawText(QRectF (rect().x() + 310, rect().y() + 30, 190, 290), m_actionToken->description(), QTextOption(Qt::AlignCenter | Qt::AlignTop));
}

void Details::drawOT(QPainter* painter)
{
    // 1. Prepare data for drawing.
    QVector<int> income = m_ownershipToken->upgradeIncome();
    QVector<int> cost = m_ownershipToken->upgradeCost();

    QString owner = (m_ownershipToken->owner() != nullptr) ? m_ownershipToken->owner()->name() : "Nobody"; // Check why this line crashes the app
    QString buyingCost    = QString::number(m_ownershipToken->buyingCost());
    QString currentIncome = QString::number(m_ownershipToken->income()); // source of errors
    QString currentUpgradeLevel  = QString::number(m_ownershipToken->upgradeLevel());
    QString currentUpgradeCost   = QString::number(m_ownershipToken->cUpgradeCost());
    QString overviewUpgradeIncome = QString("%1, %2, %3.").arg(income.at(0)).arg(income.at(1)).arg(income.at(2));
    QString overviewUpgradeCost   = QString("%1, %2, %3.").arg(cost.at(0)).arg(cost.at(1)).arg(cost.at(2));

    QImage upgradeImage = QImage(QString("D:/monopoly/at/stars_%1.png").arg(m_ownershipToken->upgradeLevel()));

    // 2. Prepare drawing instruments
    QPen borderPen = QPen(QBrush(Qt::darkGray), 5);
    QPen basicTextPen = QPen(Qt::white);
    QPen buyingTextPen = QPen(Qt::green);
    QPen upgradeTextPen = QPen(Qt::lightGray);

    // 4. Draw ownership token details
    int margin = 5;

    painter->setPen(basicTextPen);
    painter->setFont(QFont("Comic Sans", 7));
        painter->drawImage(QRectF(rect().x(),                rect().y(),      300, 300), m_ownershipToken->image());
        painter->drawImage(QRectF(rect().x(),                rect().y(),      300, 300), upgradeImage);
        painter->drawText(QRectF (rect().x() + 300 + margin, rect().y(),      190,  20), m_ownershipToken->name(), QTextOption(Qt::AlignCenter));
        painter->drawLine(QPointF(rect().x() + 330, rect().y() + 20), QPointF(rect().x() + 470, rect().y() + 20));
        painter->drawText(QRectF (rect().x() + 300 + margin, rect().y() + 30, 190, 15), "Owner:" + owner, QTextOption(Qt::AlignCenter));
        painter->drawText(QRectF (rect().x() + 300 + margin, rect().y() + 45, 190, 290), m_ownershipToken->description(), QTextOption(Qt::AlignCenter | Qt::AlignTop));
    painter->setPen(buyingTextPen);
        painter->drawText(QPointF(rect().x() + margin,       rect().y() + 310), "Buying cost: " + buyingCost);
    painter->setPen(basicTextPen);
        painter->drawText(QPointF(rect().x() + margin,       rect().y() + 325), "Income: " + currentIncome);
        painter->drawText(QPointF(rect().x() + margin,       rect().y() + 340), "Upgrade level: " + currentUpgradeLevel);
        painter->drawText(QPointF(rect().x() + margin,       rect().y() + 355), "Upgrade cost: " + currentUpgradeCost);
    painter->setPen(upgradeTextPen);
        painter->drawText(QPointF(rect().x() + margin,       rect().y() + 370), "(Overview) Upgrade income: " + overviewUpgradeIncome);
        painter->drawText(QPointF(rect().x() + margin,       rect().y() + 385), "(Overview) Upgrade cost: " + overviewUpgradeCost);

    // 5. Draw buttons
        qDebug() << "Paint. BBR:  " << m_buyButton->boundingRect();
        qDebug() << "Paint. UgBR: " << m_upgradeButton->boundingRect();
        qDebug() << "Paint. UsBR: " << m_useButton->boundingRect();

        // painter->drawRect(m_buyButton->boundingRect());
        // painter->drawRect(m_upgradeButton->boundingRect());


    if (m_buyButton && !m_buyButton->isHidden())
        m_buyButton->paint(painter, nullptr, nullptr);

    if (m_upgradeButton && !m_upgradeButton->isHidden())
        m_upgradeButton->paint(painter, nullptr, nullptr);

    qDebug() << "paint event over";
}

void Details::drawCard(QPainter *painter)
{
    // 1. Prepare drawing instruments
    QPen textPen = QPen(Qt::white);

    // qDebug() << m_card->imageFrontFG().width() << ":" << m_card->imageFrontFG().height();

    // 2. Draw action token details
    painter->setPen(textPen);
    painter->setFont(QFont("Comic Sans", 7));
    painter->drawImage(QRectF(rect().x(),       rect().y(),      300, 300), m_card->imageFrontFG());
    painter->drawText(QRectF (rect().x() + 310, rect().y(),      190,  20), m_card->name(), QTextOption(Qt::AlignCenter));
    painter->drawLine(rect().x() + 330, rect().y() + 20, rect().x() + 480, rect().y() + 20);
    painter->drawText(QRectF (rect().x() + 310, rect().y() + 30, 190, 290), m_card->description(), QTextOption(Qt::AlignCenter | Qt::AlignTop));

    // 3. Draw use button
    if (m_useButton && !m_useButton->isHidden())
        m_useButton->paint(painter, nullptr, nullptr);
}

QString Details::upgradeMessage()
{
    // Prepare message for the upgrade button.
    if (!m_ownershipToken)
    {
        qDebug() << "there is no OT";
        return "";
    }

    QString upgradeMessage;
    int nextLevel = m_ownershipToken->upgradeLevel() + 1;
    if (nextLevel > 3)
        upgradeMessage = QString("Fully upgraded");
    else
        upgradeMessage = QString("Upgrade to %1").arg(nextLevel);

    return upgradeMessage;
}

UIElement* Details::createButton(const QString &text, const QSize &size, const QPoint &position)
{
    UIElement* button = new UIElement (text);
    button->setRole(UIElement::Role::BUTTON);
    button->setPos(position);
    button->setSize(size);
    button->setRect(position.x(), position.y(), size.width(), size.height());
    button->setShape(UIElement::Shape::RECTANGLE);
    button->setBorderWidth(2);
    button->setPen(QPen(Qt::white));
    button->setFont(QFont("Comic Sans", 10));

    return button;
}
