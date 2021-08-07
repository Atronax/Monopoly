#include "table.h"

#include <QDebug>
#include <QPoint>
#include <QScrollBar>
#include <QFileDialog>

#include <QApplication>
#include <QThread>

#include "nodes/nodeeditor.h"

Table::Table(QWidget *parent)
    : QWidget (parent)
{    
    activateHistory();
    prepareScene();
    addMenu();

    setWindowIcon(QIcon("D:/monopoly/icon.png"));
}

Table::~Table()
{
    clearHands();
    clearUnits();
    clearDecks();
    clearNodes();
    clearScene();
    clearActionTokensData();
    clearOwnershipTokensData();
    clearCardsData();

    freePointers();
    nullifyPointers();
}

// ******************************** CLEANING

void Table::clearScene()
{
    m_scene->clear();    
}

void Table::clearNodes()
{
    if (!m_nodes)
        return;

    for (int i = 0; i < m_nodes->count(); ++i)
    {
        Node* node = m_nodes->at(i);
        delete node;
    }

    m_nodes->clear();
}

void Table::clearOwnershipTokensData()
{
    if (!m_OTDescription)
    {
        qDebug() << "No ownership tokens description list.";
        return;
    }

    for (int i = 0; i < m_OTDescription->count(); ++i)
        delete m_OTDescription->at(i);

    m_OTDescription->clear();
}

void Table::clearActionTokensData()
{
    if (!m_ATDescription)
    {
        qDebug() << "No action tokens description list.";
        return;
    }

    for (int i = 0; i < m_ATDescription->count(); ++i)
        delete m_ATDescription->at(i);

    m_ATDescription->clear();
}

void Table::clearCardsData()
{
    if (!m_CDescription)
    {
        qDebug() << "No cards description list.";
        return;
    }

    for (int i = 0; i < m_CDescription->count(); ++i)
        delete m_CDescription->at(i);

    m_CDescription->clear();
}

void Table::clearHands()
{
    if (!m_hands)
    {
        qDebug() << "No hands list.";
        return;
    }

    for (int i = 0; i < m_hands->count(); ++i)
    {
        QPair<Hand::Side, QPainterPath> *pair = m_hands->at(i);
        m_hands->removeOne(pair);
        delete pair;
    }

    m_hands->clear();
}

void Table::clearUnits()
{
    if (!m_units)
    {
        qDebug() << "No units list.";
        return;
    }

    // delete all the players from the list
    for (int i = 0; i < m_units->count(); ++i)
        removeUnit(m_units->at(i));

    m_units->clear();    

    // also reinitialize the current player variables    
    m_currentPlayer = nullptr;
    m_currentPlayerIndex = 0;
}

void Table::clearDecks()
{
    if (!m_cardsP && !m_cardsN)
    {
        qDebug() << "Decks weren't created yet.";
        return;
    }

    m_cardsP->clear();
    m_cardsN->clear();    
}

void Table::freePointers()
{
    // Descriptions
    delete m_ATDescription;
    delete m_OTDescription;
    delete m_CDescription;

    // Widgets
    pb_editMode->deleteLater();
    pb_saveMap->deleteLater();
    pb_loadMap->deleteLater();
    pb_turn->deleteLater();
    l_turn->deleteLater();
    m_layout->deleteLater();

    // Items
    delete m_menu;
    delete m_details;
    delete m_status;
    delete m_currentCard;
    delete m_cardsP;
    delete m_cardsN;
    delete m_hands;
    delete m_units;
    delete m_nodes;
    delete m_view;
    delete m_scene;
}

void Table::nullifyPointers()
{
    m_ATDescription = nullptr;
    m_OTDescription = nullptr;
    m_CDescription = nullptr;
    m_currentCard = nullptr;
    m_details = nullptr;
    m_status = nullptr;
    m_cardsP = nullptr;
    m_cardsN = nullptr;
    m_hands = nullptr;
    m_units = nullptr;
    m_nodes = nullptr;
    m_view = nullptr;
    m_scene = nullptr;
}

// ********************************** KEYBOARD AND MOUSE EVENTS

void Table::keyPressEvent(QKeyEvent *event)
{
    if (m_currentPlayer == nullptr)
        return;

    switch (event->key())
    {
        case Qt::Key_F1:
        if (event->modifiers() | Qt::Key_Control)
        {
            if (l_history)
                l_history->logToFile("log_test.txt");
        }
        break;

        case Qt::Key_Escape:
        if (m_menu->isHidden())
            showMenu();
        else
            hideMenu();
        break;

        case Qt::Key_A:
        step (m_currentPlayer, Player::Direction::LEFT);
        break;

        case Qt::Key_D:
        step (m_currentPlayer, Player::Direction::RIGHT);
        break;

        case Qt::Key_W:
        step (m_currentPlayer, Player::Direction::UP);
        break;

        case Qt::Key_S:
        step (m_currentPlayer, Player::Direction::DOWN);
        break;

        case Qt::Key_Space:
        break;

    }
}

void Table::mousePressEvent(QMouseEvent *event)
{
    QPoint handsError = QPoint(handwidth, handwidth);
    QPoint mousePosition = event->pos() - m_view->pos() - handsError;

    // App should react differently in playing and editing state, so lets use enum for this kind of stuff.
    switch (m_mode)
    {
        case Mode::MENU:
        {            
            mousePosition += handsError; // remove this

            Menu* menu = lookForMenuAt(mousePosition);
            if (menu)
            {
                qDebug() << "There was a click somewhere in the menu: ";
                UIElement* button = menu->buttonAtPosition(mousePosition);
                if (button)
                {
                    qDebug() << "User clicked on button: " << button->text();
                    if (button->defaultText() == "New Game")
                        newGame();

                    if (button->defaultText() == "Quit")
                        quit();
                }
            }            
        }
        break;

        case Mode::EDIT:
        {
            Node *node = getNodeAt(mousePosition, false);
            if (event->button() == Qt::LeftButton && node)
                qDebug() << "There is a node at " << mousePosition;

            // If user clicks LMB on a valid position, a new node is created there.
            if (event->button() == Qt::LeftButton && addingIsValid(mousePosition))
                 createNode(gridPosition(mousePosition));

            // If user clicks RMB on a valid position, a node is removed from there.
            if (event->button() == Qt::RightButton && node && removalIsValid(node))
                removeNode(node);
        }
        break;

        case Mode::PLAY:
        if (event->button() == Qt::LeftButton)
        {
            QGraphicsItem* item = m_scene->itemAt(mousePosition, QTransform());

            // User clicked on one of the token thumbnails.
            Token* tokenThumbnail = m_currentPlayer->hand()->takeTokenAtPosition(mousePosition);
            if (tokenThumbnail)
            {
                OwnershipToken* ownershipToken = dynamic_cast<OwnershipToken*>(tokenThumbnail);
                if (ownershipToken)
                {
                    l_history->addMessage(QString("Player %1 selected ownership token thumbnail. Name: %2.").arg(m_currentPlayer->name()).arg(ownershipToken->name()));

                    m_details->show();
                    m_details->setZValue(1.0f);
                    m_details->clearSelection();
                    m_details->setToken(ownershipToken);
                    m_details->hideButtons();
                    // m_details->showClose();
                    m_details->update();
                }
            }

            // User clicked on one of the card thumbnails.
            Card* cardThumbnail  = m_currentPlayer->hand()->takeCardAtPosition (mousePosition);
            if (cardThumbnail)
            {
                l_history->addMessage(QString("Player %1 selected the card thumbnail. Name: %2. Description: %3.").arg(m_currentPlayer->name()).arg(cardThumbnail->name()).arg(cardThumbnail->description()));

                m_currentCard = cardThumbnail;

                m_details->show();
                m_details->setZValue(1.0f);
                m_details->clearSelection();
                m_details->setCard(cardThumbnail);
                m_details->showButtons();
                m_details->update();
            }

            // User clicked somewhere in the details region.
            Details* details = dynamic_cast<Details*>(item);
            if (details)
            {
                UIElement* button = details->buttonAtPosition(mousePosition);
                if (!button)
                {
                    l_history->addMessage("There isn't any buttons here.");
                    return;
                }
                else
                    l_history->addMessage(QString("User selected a button. It's name is %1").arg(button->text()));

                // If user clicked on close button
                if (button->text() == "X")
                {
                    details->clearSelection();
                    l_history->addMessage("User closed the details item.");
                }

                // If user clicked on buy or upgrade button, when details item shows ownership token information.
                OwnershipToken* ownershipToken = details->OT();
                if (ownershipToken)
                {
                    if (button->text() == "Buy")
                    {
                        m_currentPlayer->takeOwnershipToken(ownershipToken);
                        l_history->addMessage(QString("Player %1 bought company %2 for %3 gold.").arg(m_currentPlayer->name()).arg(ownershipToken->name()).arg(ownershipToken->buyingCost()));
                    }

                    if (button->text().startsWith("Upgrade"))
                    {
                        ownershipToken->upgrade(false);
                        l_history->addMessage(QString("Player %1 upgraded company %2 to level %3.").arg(m_currentPlayer->name()).arg(ownershipToken->name()).arg(ownershipToken->upgradeLevel()));
                    }
                }

                // If user clicked on use
                Card* card = details->card();
                if (card)
                {
                    qDebug() << QString("Card selected. Name: %1. Type: %2.").arg(card->name()).arg(card->typeToString(card->cardType()));

                    if (button->text().startsWith("Use"))
                        activate(card);

                    details->clearSelection();
                }

                m_details->update();
                m_currentPlayer->hand()->update();
            }

            // User clicked on one of the decks.
            Deck* deck = dynamic_cast<Deck*> (m_scene->itemAt(mousePosition, QTransform()));
            if (deck)
            {
                if (!deck->isEmpty())
                {
                    qDebug() << "There is a deck and it is not empty.";
                    deck->peekTop()->turnAround();
                    deck->update();
                }
            }

            // User clicked on one of the tokens on table.
            Token* token = dynamic_cast<Token*> (item);
            if (token)
            {
                ActionToken* at = dynamic_cast<ActionToken*>(token);
                if (at)
                {
                    // Do something with action tokens.
                    qDebug() << "This is action token";
                }

                OwnershipToken* ot = dynamic_cast<OwnershipToken*>(token);
                if (ot)
                {
                    // Do something with ownership token.
                    qDebug() << "This is ownership token.";
                }
            }

        }

        qDebug () << "mouse event OVER";
        break;
    }
}

void Table::mouseDoubleClickEvent(QMouseEvent *event)
{
    QPoint handsError = QPoint (handwidth, handwidth);
    QPoint mousePosition = event->pos() - m_view->pos() - handsError;

    switch (m_mode)
    {
        case Mode::MENU:
        {


        }
        break;

        case Mode::EDIT:
        if (event->button() == Qt::LeftButton)
        {            
            qDebug() << "Double click LMB";
            Node* node = getNodeAt(mousePosition, false);
            if (node)
                editNode (node);
        }
        break;

        case Mode::PLAY:
        if (event->button() == Qt::LeftButton)
        {
            /*
            Deck* deck = lookForDeckAt(mousePosition);
            if (deck)
            {
                if (!deck->isEmpty())
                {
                    // deck->use();
                    // deck->update();

                    qDebug() << "Deck has " << deck->count() << " cards.";
                }
            }
            */
        }
        break;
    }
}

void Table::activateHistory()
{
    // History: allows to add messages to single spot,
    // move between them using mouse wheel and
    // flush the messages into some log file.
    l_history   = new HistoryLabel("History");
    l_history->setMaximumHeight(23);
}

void Table::setMode(const Table::Mode &mode)
{
    m_mode = mode;

    switch (m_mode)
    {
        case Mode::MENU:
        l_history->addMessage("The applications' state has been changed to MENU.");
        break;

        case Mode::PLAY:
        l_history->addMessage("The applications' state has been changed to PLAY.");
        break;

        case Mode::EDIT:
        l_history->addMessage("The applications' state has been changed to EDIT.");
        break;
    }
}

void Table::hideMenu()
{
    m_menu->setHidden(true);
    m_scene->removeItem(m_menu);
}

void Table::showMenu()
{
    m_scene->addItem(m_menu);
    m_menu->setHidden(false);
}

void Table::newGame()
{    
    hideMenu();

    addGrid();
    addNodes();
    addDecks();
    addHands();
    addUnits();
    addUIWidgets();
    addUIItems();

    loadDescriptions("action_tokens",    "d:/monopoly/at/at.xml");
    loadDescriptions("ownership_tokens", "d:/monopoly/ot/ot.xml");
    loadDescriptions("cards",            "d:/monopoly/cards/cards.xml");
    l_history->addMessage(QString("Descriptions were loaded. Among them there're %1 action tokens, %2 ownership tokens and %3 cards. Total objects: %4.")
                          .arg(m_ATDescription->count()).arg(m_OTDescription->count()).arg(m_CDescription->count())
                          .arg(m_ATDescription->count() + m_OTDescription->count() + m_CDescription->count()));

    setMovementConstraint(Constraint::COUNTER_CLOCKWISE);
    setMode(Mode::PLAY);
    onDefaults();
}

void Table::quit()
{
    qApp->quit();
}

// *************************************** OBJECTS CONTROLLERS

Node *Table::createNode(const QPoint& gridPosition)
{
    Node *node = new Node(gridPosition);
    node->setRect(QRect(gridPosition.x() * NODE_WIDTH, gridPosition.y() * NODE_HEIGHT, NODE_WIDTH, NODE_HEIGHT));    

    addNode(node);
    return node;
}

void Table::addNode(Node *n)
{
    if (!m_nodes->contains(n))
    {
        m_nodes->append(n);
        m_scene->addItem(n);
    }
}

void Table::removeNode(Node *n)
{
    if (m_nodes->contains(n))
    {
        m_nodes->removeOne(n);
        m_scene->removeItem(n);

        delete n;
    }
}

Player *Table::createUnit(const QPoint& gridPosition, const QString &name, const QColor& color, const QString &imagePath)
{
    Node *node = getNodeAt(gridPosition, true);
    Player *unit = nullptr;

    if (node)
    {
        unit = new Player (gridPosition, name, color, imagePath);
        unit->setSize(QSize(UNIT_WIDTH, UNIT_HEIGHT));        
        unit->setRect(node->rect());        
        addUnit(unit);
    }

    return unit;
}

void Table::addUnit(Player *u)
{
    if (m_units->contains(u))
        return;

    m_units->append(u);
    qDebug() << QString("There are %1 players in units list").arg(m_units->count());

    m_scene->addItem(u);
    m_scene->addItem(u->hand());
}

void Table::removeUnit(Player *u)
{
    if (m_units->contains(u))
    {
        if (u->hand())
            u->hand()->clear();

        m_units->removeOne(u);
        m_scene->removeItem(u->hand());
        m_scene->removeItem(u);

        qDebug() << QString("There are %1 players in units list").arg(m_units->count());

        delete u;
        u = nullptr;
    }
}

Card *Table::createCard(const QString &name, const QString &description, const QString &imagePath, bool isPositive)
{
    Card *card = new Card (name, description, imagePath);

    addCard(card, isPositive);
    return card;
}

void Table::addCard(Card *card, bool isPositive)
{
    Deck *deck = (isPositive) ? m_cardsP : m_cardsN;

    if(!deck->has(card))
        deck->add(card);
}

void Table::removeCard(Card *card, bool isPositive)
{
    Deck *deck = (isPositive) ? m_cardsP : m_cardsN;

    if (deck->has(card))
    {
        deck->remove(card);

        // Move card representation somewhere to stash       
        m_scene->removeItem(card);
        // delete c;
    }
}

void Table::editNode(Node *node)
{
    NodeEditor editor (node);
    editor.makeSelector(Description::ObjectType::ACTION_TOKEN, m_ATDescription);
    editor.makeSelector(Description::ObjectType::OWNERSHIP_TOKEN, m_OTDescription);

    qDebug() << "in editNode::node is " << (node == nullptr);

    if (QDialog::Accepted == editor.exec())
    {
        // Last items in both selectors are left for default choose.
        int ATidx = editor.activatedTokenIndex(Description::ObjectType::ACTION_TOKEN);
        int OTidx = editor.activatedTokenIndex(Description::ObjectType::OWNERSHIP_TOKEN);

        bool ownershipTokenSelected = (OTidx >= 0 && OTidx < m_OTDescription->count() - 1);
        bool actionTokenSelected    = (ATidx >= 0 && ATidx < m_ATDescription->count() - 1);

        if (actionTokenSelected || ownershipTokenSelected)
        {
            // Take previous token and remove it
            Token* old = node->token();
            delete old;
            old = nullptr;

            // Generate new action token using description at relevant index and set it for node.
            Token* token = nullptr;
            if (actionTokenSelected)
                token = ATFor(ATidx);
            else if (ownershipTokenSelected)
                token = OTFor(OTidx);

            node->setToken(token);

            // History relevant stuff.
            l_history->addMessage(QString("User chose action token %1 in the editor.").arg(token->name()));
        }
        else
        {
            // Just take the data from the editor and replace the data of current node.
            Token *token = node->token();
            token->setName(editor.name());
            token->setDescription(editor.description());
            token->setImage(editor.imagePath());
        }

        node->update();
    }
}

// ****************************************** INTERACTIONS

void Table::action(ActionToken::ActionType actionType)
{
    switch (actionType)
    {
    // + give player some gold, set set num of passed circles, update hand region
    case ActionToken::ActionType::START:
        {
            m_currentPlayer->circle();                // passed circles stats
            m_currentPlayer->hand()->receive(20000);  // wage per passed circle
            m_currentPlayer->hand()->returns();       // returns from the ownings

            m_scene->update(m_currentPlayer->hand()->rect());
        }
        break;

    // + find next node with the start action token and move player there, activate start action
    case ActionToken::ActionType::PORTAL:
        {
            Node* start = findNodeByName("start");
            if (start)
            {
                m_currentPlayer->setGridPosition(start->gridPosition());
                m_currentPlayer->setRect(start->rect());

                action(ActionToken::ActionType::START);
            }
        }
        break;

    // +- block current player for 1 turn and pass the turn to another player
    case ActionToken::ActionType::PRISON:        
        m_currentPlayer->setBlocked(1);
        break;

    // - open trading zone, that allows exchanging of tokens
    case ActionToken::ActionType::EXCHANGE:
        break;

    // + additional movement forwards
    case ActionToken::ActionType::MOVE_FORWARD:
        m_stepsLeft = dropDie(1,6);
        startMovement(m_currentPlayer);
        break;

    // + additional movement backwards : set opposite direction and leave everything else like in moving forwards technique
    case ActionToken::ActionType::MOVE_BACKWARD:
        setMovementConstraint((m_constraintCurrent == Constraint::COUNTER_CLOCKWISE) ? Constraint::CLOCKWISE : Constraint::COUNTER_CLOCKWISE);
        m_stepsLeft = dropDie(1,6);
        startMovement(m_currentPlayer);
        break;

    // +- take the card from the deck of positive bonuses
    case ActionToken::ActionType::CARD_POSITIVE:        
        m_currentPlayer->takeCard(m_cardsP->takeTop());
        m_scene->update(m_currentPlayer->hand()->rect());
        break;

    // +- take the card from the deck of negative bonuses
    case ActionToken::ActionType::CARD_NEGATIVE:
        m_currentPlayer->takeCard(m_cardsN->takeTop());
        m_scene->update(m_currentPlayer->hand()->rect());
        break;
    }
}

void Table::activate(Card* card)
{
    qDebug() << "Card activated: " << card->typeToString(card->cardType());

    if (m_units->count() < 2)
        qDebug() << "This is a game with one gate.";

    if (m_currentPlayer == nullptr)
        qDebug() << "There is no actual pointer to current player.";

    bool cardActivated = true;


    Card::CardType cardType = card->cardType();
    switch (cardType)
    {

    // Positive cards
    case Card::CardType::TREASURE:
        {
            // * + TREASURE          - immediately gives the player some random count of gold.
            // 1. Generate random value of gold player receives after digging the treasure from the cold ground.
            //    The amount of gold is in range [5000; 10000] with a step of 500.
            qDebug() << "Treasure card activated";

            int gold = 5000 + 500 * (0 + rand()%11);
            qDebug() << QString("The player %1 is about to receive %2 gold.").arg(m_currentPlayer->name()).arg(gold);
            qDebug() << QString("He has hands to hold his goods: %1.").arg(m_currentPlayer->hand() != nullptr);

            m_currentPlayer->hand()->receive(gold);
            m_scene->update(m_currentPlayer->hand()->rect());
        }
        break;

    case Card::CardType::OVERTIME:
        {
            // * + OVERTIME          - doubles the returns from players' companies for one circle.
            // 1. Activate mode of double income from all companies for one circle.
            // When player moves across start node, he will get double income and flag turns back to false.
            qDebug() << "Overtime card activated";

            m_currentPlayer->hand()->setIncomeDoubled(true);
        }
        break;

    case Card::CardType::MASTERCHEF:
        {
            // * + MASTERCHEF        - allows to make additional movement with the chance of its doubling without need to turn.
            // 1. Generate random values for amount of steps, chance of its doubling.
            // 2. Check if there is actual doubling by generating random value in range [0, 100] and checking if it is in range [0, 10]
            // 3. Actual movement.
            qDebug() << "Masterchef card activated";

            int  chance   = 100;              // 10 percent chance to double the count of steps
            int  drop     = rand()%101;
            bool success  = (drop <= chance); // if random value is in range [0; 10], then success is true, otherwise false
            qDebug() << QString("Chance is %1%. Dropped: %2. Success: %3").arg(chance).arg(drop).arg(success ? "yes" : "no");

            m_stepsLeft = dropDie(1,6); // raw count of steps left on this turn
            m_stepsLeft = (success) ? 2 * m_stepsLeft : m_stepsLeft; // count of steps including chance of doubling
            qDebug() << QString("The player will make %1 steps forward.").arg(m_stepsLeft);

            startMovement(m_currentPlayer);
        }
        break;

    case Card::CardType::FAST_AND_FURIOUS:
        {
            // * + FAST_AND_FURIOUS  - moves the player half ring forwards (or backwards, no matter).
            // For example, if map is 8x8, and player stands on (2,0), he will move to symmetric position (5,7) and vice versa.
            // In this case, it is also important to leave the ability to grab start bonus, if the player passes that node. So,
            // 1. Speed up the movement by decreasing the step timer (dividing it by some number).
            // 2. Calculate steps count to pass half of the circle.
            // 3. Actual move.
            qDebug() << "Fast and furious card activated";

            setMovementSpeed(5);
            m_stepsLeft = NODES_PER_ROW + NODES_PER_COLUMN - 1; // half-circle, move this to some method later
            startMovement(m_currentPlayer);
        }
        break;

    case Card::CardType::BIRTHDAY:
        {
            // * + BIRTHDAY          - other players present 5000 to current player.
            // 1. Walk through each player in the list and if it not the same as current,
            // 2. Grab specific amount of gold or everything he has and give it to current.
            qDebug() << "Birthday card activated";
            qDebug() << QString("There are %1 players total.").arg(m_units->count());

            for (int i = 0; i < m_units->count(); ++i)
            {
                Player* p = m_units->at(i);
                if (p != m_currentPlayer)
                {
                    int gold = p->hand()->gold();

                    p->hand()->pay(gold >= 5000 ? 5000 : gold);
                    m_currentPlayer->hand()->receive(gold >= 5000 ? 5000 : gold);
                }
            }
        }
        break;

    case Card::CardType::SCIENTIST:
        {
            // * + SCIENTIST         - plus 1 star to random company without need to pay for it.
            // 1. Choose random company from the list of current players' ownings and add 1 star to it without any payments. Our scientist works for food!
            qDebug() << "Scientist card activated";

            m_currentPlayer->hand()->upgradeRandomCompany(1);
        }
        break;

    case Card::CardType::TOGETHER:
        {
            // * + TOGETHER            - moves all the players using the same count of steps
            // !
            // 1. Choose some random value in range [1;6] as count of steps for all the players.
            // 2. Store the current player and make movement with each, stored in the list.
            // 3. Restore the current player.
            qDebug() << "Together card activated";

            m_stepsLeft = dropDie(6,12);
            m_stepsLeft *= m_units->count();

            qDebug() << QString("There are %1 players total.").arg(m_units->count());
            qDebug() << QString("Each of them should make %1 steps.").arg(m_stepsLeft);

            connect(&m_autoMovementTogetherTimer, SIGNAL(timeout()), this, SLOT(autoMovementTogether()));
            m_autoMovementTogetherTimer.start(150);
        }
        break;

    // Negative cards
    case Card::CardType::THIEF:
        {
            // Cards with negative effect can be used to harm opposite players.
            // * + THIEF         - steals some random count of gold from random player.
            // 0. Gather all the opponents into separate list.
            // 1. Take random player from the list of opponents.
            // 2. Check how many gold he has and set "random" amount of gold for thief to steal.
            // 3. If player has less gold than this "random" amount, steal what he has. Otherwise, steal that "random" amount.
            qDebug() << "Thief card activated";

            Player* opponent = randomOpponent();

            if (opponent)
            {
                int goldOfOpponent = opponent->hand()->gold();
                int goldToSteal = 5000 + 250*rand()%11; // from 5000 to 7500

                opponent->hand()->pay(goldToSteal <= goldOfOpponent ? goldToSteal : goldOfOpponent);
                m_currentPlayer->hand()->receive(goldToSteal <= goldOfOpponent ? goldToSteal : goldOfOpponent);
            }
            else
            {
                qDebug() << "Card was not activated. There is no opponents.";
                cardActivated = false;
            }
        }
        break;

        // Check this card
    case Card::CardType::DIVERSION:
        {
            // * + DIVERSION     - forces opposite player to walk backwards.
            // 1. Store current player, select random opponent.
            // 2. Force him to move backwards.
            // 3. Restore current player.
            qDebug() << "Diversion card activated";

            Player* opponent = randomOpponent();
            if (opponent)
            {
                qDebug() << QString("Current player: %1. Opponent player: %2.").arg(m_currentPlayer->name()).arg(opponent->name());

                // todo: change movement direction of the opponent after he has done the movement in opposite direction.
                // also: moving player should not activate node objects, current player holds his turn.

                m_stepsLeft = dropDie(1,6);
                if (m_constraintCurrent == Constraint::COUNTER_CLOCKWISE)
                    setMovementConstraint(Constraint::CLOCKWISE);
                else
                    setMovementConstraint(Constraint::COUNTER_CLOCKWISE);
                startMovement(opponent);

            }
            else
            {
                qDebug() << "Card was not activated. There is no opponents.";
                cardActivated = false;
            }
        }
        break;

    case Card::CardType::SABOTAGE:
        {
            // * + SABOTAGE      - chance to stop opposite players' companies income for one circle.
            // 1. Take random opponent.
            // 2. Calculate success based on specific chance.
            // 3. Stop income of opponents companies for one circle, if success check has been passed.
            qDebug() << "Sabotage card activated";

            for (int i = 0; i < m_units->count(); ++i)
            {
                Player* p = m_units->at(i);
                if (p != m_currentPlayer)
                {
                    // separate chance for each opponent
                    int chance = 100;
                    int drop = rand()%101;
                    bool success = (drop <= chance) ? true : false;
                    qDebug() << QString("Chance is %1%. Dropped: %2. Success: %3").arg(chance).arg(drop).arg(success ? "yes" : "no");

                    if (success)
                    {
                        p->hand()->setIncomeStopped(true);
                        qDebug() << QString("Income for companies of %1 is stopped for 1 circle.").arg(p->name());
                    }
                }
            }
        }
        break;

    case Card::CardType::RAID:
        {
            // * + RAID          - chance to steal one of the opposite players' companies.
            // 1. Set the chance and pass the success check.
            // 2. If success, take random ownership token from one of the opponents (may be nullptr, so there may be need to update the method)
            // 3. Remove the OT from the opponent and place it in current players hand.
            qDebug() << "Raid card activated";

            int chance = 100;
            int drop = rand()%101;
            bool success = (drop <= chance) ? true : false;
            qDebug() << QString("Chance is %1%. Dropped: %2. Success: %3").arg(chance).arg(drop).arg(success ? "yes" : "no");

            if (success)
            {
                Player* opponent = randomOpponent();

                qDebug() << "here";
                if (opponent)
                {
                    OwnershipToken* OT = opponent->hand()->randomCompany();

                    if (OT)
                    {
                        opponent->hand()->removeToken(OT);
                        m_currentPlayer->hand()->addToken(OT);

                        m_scene->update(opponent->hand()->rect());
                        m_scene->update(m_currentPlayer->hand()->rect());
                    }
                    else
                    {
                        qDebug() << QString("Card was not activated. The player %1 hasn't any companies yet.").arg(opponent->name());
                        cardActivated = false;
                    }
                }
                else
                {
                    qDebug() << QString("Card was not activated. There is no opponents.");
                    cardActivated = false;
                }
            }
        }
        break;

    case Card::CardType::BRIBE:
        {
            // * + BRIBE         - chance to put one of the opposite players to jail for some turns.
            // 1. Calculate some random value of gold as bribe payment.
            // If the player has enough, initiate the rest of code. Otherwise, do nothing.
            // 2. Make the chance check.
            // 3. If the chance check has been passed ok, calculate random count of imprisonment turns.
            // 4. Pay the gold.
            // 5. Select the random opponent, find the jail node and move him there.
            // 6. Initiate the jail action.
            qDebug() << "Bribe card activated";

            int gold = 2500 + 250*rand()%11; // [2500;5000] for bribe
            if (m_currentPlayer->hand()->gold() < gold)
            {
                qDebug() << QString("Card was not activated. Player %1 hasn't enough money to initiate the bribe.").arg(m_currentPlayer->name());
                cardActivated = false;
                return;
            }

            int chance = 100;
            int drop = rand()%101;
            bool success = (drop <= chance) ? true : false;
            qDebug() << QString("Chance is %1%. Dropped: %2. Success: %3").arg(chance).arg(drop).arg(success ? "yes" : "no");

            if (success)
            {
                m_currentPlayer->hand()->pay(gold);

                int turns = 1 + rand() % (1 + rand() % 4); // [1;4] turns of jail with significantly lower chance to get more turns

                Node* jailNode = findNodeByName("prison");
                if (jailNode)
                {
                    Player* opponent = randomOpponent();
                    opponent->setRect(jailNode->rect());
                    opponent->setGridPosition(jailNode->gridPosition());
                    opponent->setBlocked(turns);
                }
                else
                {
                    qDebug() << QString("Card was not activated. There is no prison on this map.");
                    cardActivated = false;
                }

                // select opponent as current player and start action, return the pointer back
                // Player* current = m_currentPlayer;
                //    m_currentPlayer = opponent;
                //     action(ActionToken::ActionType::PRISON);
                // m_currentPlayer = current;
            }
        }
        break;

    case Card::CardType::SNEAK:
        {
            // * + SNEAK         - immediately moves player to random opponent.
            // 1. Take random opponent, grab node under his foot.
            // 2. Move current player there.
            qDebug() << "Sneak card activated";

            Player* opponent = randomOpponent();

            if (opponent)
            {
                Node* node = getNodeAt(opponent->gridPosition(), true);
                m_currentPlayer->setGridPosition(node->gridPosition());
                m_currentPlayer->setRect(node->rect());
            }
            else
            {
                qDebug() << QString("Card was not activated. There is no prison on this map.");
                cardActivated = false;
            }
        }
        break;

    case Card::CardType::SPY:
        {
            // * + SPY           - chance to learn technologies of other player and add the count of stars of his best company to one of the own companies.
            // 1. Set chance and pass the chance check.
            // 2. If success, take random oppopent and get upgrade level of his best company.
            // 3. Upgrade random company of current player by the same count of stars.
            qDebug() << "Spy card activated";

            int chance = 100;
            int drop   = rand()%101;
            bool success = (drop <= chance) ? true : false;

            qDebug() << QString("Chance is %1%. Dropped: %2. Success: %3").arg(chance).arg(drop).arg(success ? "yes" : "no");

            if (success)
            {
                Player* opponent = randomOpponent();

                if (opponent)
                {
                    int stars = opponent->hand()->topCompanyUpgradeLevel(); // returns 0 if opponent hasn't any companies
                    if (stars > 0)
                        m_currentPlayer->hand()->upgradeRandomCompany(stars);
                    else
                    {
                        qDebug() << QString("Card was not activated. Opponent %1 hasn't any companies with upgrades.").arg(opponent->name());
                        cardActivated = false;
                    }
                }
                else
                {
                    qDebug() << QString("Card was not activated. There is no opponents.");
                    cardActivated = false;
                }
            }
        }
        break;

    case Card::CardType::DEFAULT:
        break;
    }

    // remove the used card from the deck, from the scene and delete the card nullify the pointer after activation of the card
    if (cardActivated)
    {
        // Remove the card only if it was activated.

        // if (m_cardsP->has(m_currentCard)) m_cardsP->remove(m_currentCard);
        // if (m_cardsN->has(m_currentCard)) m_cardsN->remove(m_currentCard);

        qDebug() << "Deleting card: " << m_currentPlayer->name();
        m_currentPlayer->hand()->removeCard(m_currentCard);

        m_scene->update(m_currentPlayer->hand()->rect());
        m_scene->removeItem(m_currentCard);
        m_details->hideButtons();
        m_details->hide();

        delete m_currentCard;
        m_currentCard = nullptr;
    }
}

// ***************************************** SCENE GENERATORS

void Table::prepareScene()
{
    basewidth  = NODE_WIDTH  * NODES_PER_ROW;
    baseheight = NODE_HEIGHT * NODES_PER_COLUMN;        

    m_scene =  new QGraphicsScene (0, 0, basewidth, baseheight, this);
    m_scene->setBackgroundBrush(QBrush(QColor("#444")));

    m_view  = new View (m_scene, this);   
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);    
    connect(m_view, SIGNAL(mousePositionChanged(const QPoint&)), this, SLOT(viewMousePositionChanged(const QPoint&)));

    m_nodes = new QList<Node*>();
    m_units = new QList<Player*>();
    m_currentPlayer = nullptr;

    m_ATDescription = new QList<Description*>();
    m_OTDescription = new QList<Description*>();
    m_CDescription  = new QList<Description*>();

    // setMouseTracking(true);
    setFixedWidth(m_scene->width());
    setFixedHeight(m_scene->height() + 26);
    setMode(Mode::MENU);    
}

void Table::addGrid()
{
    QPen pen;
    QPen pen_inner (QBrush(Qt::gray), 1, Qt::DashLine);
    QPen pen_border (QBrush(Qt::darkGray), 2, Qt::SolidLine);

    // Vertical lines
    for (int x = 0; x <= basewidth; x += NODE_WIDTH)
    {
        pen = (x == 0 || x == basewidth) ? pen_border : pen_inner;
        m_scene->addLine(QLineF(x, 0, x, baseheight), pen);
    }

    // Horizontal lines
    for (int y = 0; y <= baseheight; y += NODE_HEIGHT)
    {
        pen = (y == 0 || y == baseheight) ? pen_border : pen_inner;
        m_scene->addLine(QLineF(0, y, basewidth, y), pen);
    }   
}

void Table::addHands()
{
    // Diagonals:
    // Lets calculate points positions and use them to generate dialogal lines,
    // as well as regions, that will be sent as parameters to hands later.
    // 0      T       3
    //  *            *
    //   1----------2
    //   *----------*
    // L *----------* R
    //   *----------*
    //   *----------*
    //   4----------5
    //  *            *
    // 6      B       7

    handwidth  = HANDS_SPACE + SCENE_MARGIN;

    // Resize scene and update Table constraints
    m_scene->setSceneRect(QRectF(-handwidth, -handwidth, basewidth + handwidth*2, baseheight + handwidth*2));
    setFixedWidth(m_scene->width());
    setFixedHeight(m_scene->height() + 26);

    // Find positions of points in the scene with hands regions:
    QPointF p0 (-handwidth, -handwidth);
    QPointF p1 (0.f, 0.f);
    QPointF p2 (basewidth, 0.f);
    QPointF p3 (basewidth+handwidth, -handwidth);

    QPointF p4 (0.f, baseheight);
    QPointF p5 (basewidth, baseheight);
    QPointF p6 (-handwidth, baseheight+handwidth);
    QPointF p7 (basewidth+handwidth, baseheight+handwidth);

    // qDebug() << "Points are: " << p0 << "," << p1 << "," << p2 << "," << p3 << "," << p4 << "," << p5 << "," << p6 << "," << p7;

    // Gather hands spaces into 4 painter pathes, that will be sent to hand as a parameter together with side type.

    QVector<QPointF> shapeUpper, shapeRight, shapeLower, shapeLeft;
    shapeUpper << p0 << p1 << p2 << p3;
    shapeRight << p3 << p2 << p5 << p7;
    shapeLower << p7 << p5 << p4 << p6;
    shapeLeft  << p6 << p4 << p1 << p0;

    // Create the list of regions data for player hands.
    // Another plus is easy swapping of the hands regions.
    m_hands = new QList<QPair<Hand::Side, QPainterPath>*>();
    addPainterPath(Hand::Side::UPPER, shapeUpper);
    addPainterPath(Hand::Side::RIGHT, shapeRight);
    addPainterPath(Hand::Side::LOWER, shapeLower);
    addPainterPath(Hand::Side::LEFT,  shapeLeft);
}

void Table::addUIWidgets()
{
    pb_editMode = new QPushButton ("Edit mode");
    pb_saveMap  = new QPushButton ("Save map");
    pb_loadMap  = new QPushButton ("Load map");
    pb_defaults = new QPushButton ("Defaults");
    pb_turn     = new QPushButton ("Make turn");

    pb_editMode->setCheckable(true);
    pb_editMode->setChecked(false);

    pb_editMode->setFixedWidth(100);
    pb_saveMap->setFixedWidth(100);
    pb_loadMap->setFixedWidth(100);
    pb_defaults->setFixedWidth(100);
    pb_turn->setFixedWidth(75);

    connect (pb_editMode, SIGNAL(clicked()), this, SLOT(onEditMode()));
    connect (pb_saveMap, SIGNAL(clicked()), this, SLOT(onSaveMap()));
    connect (pb_loadMap, SIGNAL(clicked()), this, SLOT(onLoadMap()));
    connect (pb_defaults, SIGNAL(clicked()), this, SLOT(onDefaults()));
    connect (pb_turn, SIGNAL(clicked()), this, SLOT(onTurn()));

    l_turn  = new QLabel ("Current turn: ");
    l_steps = new QLabel ("Steps left: ");

    m_layout = new QGridLayout (this);
    m_layout->setSpacing(0);
    m_layout->setMargin(0);
    m_layout->addWidget(m_view,      0, 0, 3, 7);
    m_layout->addWidget(l_history,   3, 0, 1, 7);
    m_layout->addWidget(pb_editMode, 4, 0, 1, 1);
    m_layout->addWidget(pb_saveMap,  4, 1, 1, 1);
    m_layout->addWidget(pb_loadMap,  4, 2, 1, 1);
    m_layout->addWidget(pb_defaults, 4, 3, 1, 1);
    m_layout->addWidget(pb_turn,     4, 4, 1, 1);
    m_layout->addWidget(l_turn,      4, 5, 1, 1);
    m_layout->addWidget(l_steps,     4, 6, 1, 1);
    setLayout(m_layout);

    setFixedWidth(m_scene->width());
    setFixedHeight(m_scene->height() + 26*2); // m_layout->rowMinimumHeight(4) + m_layout->rowMinimumHeight(3));
}

void Table::addMenu()
{
    QSize szMenu (260, 380);
    QRect rtMenu ((basewidth - szMenu.width()) / 2, (baseheight - szMenu.height()) / 2, szMenu.width(), szMenu.height());

    l_history->addMessage(QString("Menu rt: (P%1,%2, S%3,%4)").arg(rtMenu.x()).arg(rtMenu.y()).arg(rtMenu.width()).arg(rtMenu.height()));

    m_menu = new Menu();
    m_menu->setRect(rtMenu);

    m_scene->addItem(m_menu);
    m_menu->createButtons();
    m_menu->showButtons();

    l_history->addMessage("Test message 2");
}

void Table::addNodes()
{
    createNode(QPoint(0,0));
}

void Table::addUnits()
{
    Player* p1 = createUnit(QPoint(0,0), "404fac", QColor("#404fac"), "P1");
    p1->setShape(Player::Shape::SQUARE);
    p1->hand()->setSide(handAt(Hand::Side::LEFT));
    p1->hand()->receive(64623);

    Player* p2 = createUnit(QPoint(0,0), "fac404", QColor("#fac404"), "P2");
    p2->setShape(Player::Shape::CIRCLE);
    p2->hand()->setSide(handAt(Hand::Side::RIGHT));
    p2->hand()->receive(54733);

    m_currentPlayer = m_units->at(0);
    m_currentPlayerIndex = 0;
}

void Table::addDecks()
{    
    // Instantiate decks for cards with positive and negative bonus.
    // Rotation based on center pivot point is used in this case.
    m_cardsP = new Deck(Deck::DeckType::POSITIVE, 7);
    m_cardsP->setRect(NODE_WIDTH * 2 - CARD_WIDTH / 2, NODE_HEIGHT * 2 - CARD_HEIGHT / 2, CARD_WIDTH, CARD_HEIGHT);
    m_cardsP->setTransformOriginPoint(m_cardsP->rect().center());
    m_cardsP->setRotation(-CARD_ANGLE);

    m_cardsN = new Deck(Deck::DeckType::NEGATIVE, 7);
    m_cardsN->setRect((basewidth - NODE_WIDTH * 2) - CARD_WIDTH / 2, (baseheight - NODE_HEIGHT * 2) - CARD_HEIGHT / 2, CARD_WIDTH, CARD_HEIGHT);
    m_cardsN->setTransformOriginPoint(m_cardsN->rect().center());
    m_cardsN->setRotation(CARD_ANGLE);

    // Fill decks with cards.
    // createCard("+1", "bonus +1", "D:/monopoly/pl_00000.png", true);
    // createCard("+2", "bonus +2", "D:/monopoly/pl_00000.png", true);
    // createCard("-1", "bonus -1", "D:/monopoly/pl_00000.png", false);
    // createCard("-2", "bonus -2", "D:/monopoly/pl_00000.png", false);

    // Add decks to the scene.
    m_scene->addItem(m_cardsP);
    m_scene->addItem(m_cardsN);
}

void Table::addUIItems()
{
    QSize szStatus (300,60), szDetails (500,400);

    // Status message with the description of current players' turn: his name and left steps.
    m_status = new UIElement ("a");
    m_status->setRole(UIElement::Role::TEXT);
    m_status->setPos(5*basewidth / 8, 1.25f*baseheight / 8);
    m_status->setSize(szStatus);
    m_status->setShape(UIElement::Shape::ROUNDED_RECTANGLE);
    m_status->setFont(QFont("Comic Sans", NODE_WIDTH / 8, false));
    m_status->setBorderWidth(3);
    m_status->setHidden(false);

    m_details = new Details();
    m_details->setRect((basewidth - szDetails.width()) / 2, (baseheight - szDetails.height()) / 2, szDetails.width(), szDetails.height());
    m_details->createButtons();
}

void Table::showUIItems()
{
    m_scene->addItem(m_status);
    m_scene->addItem(m_details);
}

void Table::hideUIItems()
{
    m_scene->removeItem(m_status);
    m_scene->removeItem(m_details);
}

void Table::updateUI()
{
    // LATER: remove labels and move everything to text rectangles.

    // 1. Generate new message and set it to status widgets and items.
    QString text = QString("P: %1 (%2). Steps: %3. Blocked: %4").arg(m_currentPlayer->name()).arg(m_currentPlayerIndex).arg(m_stepsLeft).arg(m_currentPlayer->blocked());
    l_turn ->setText(QString("Player: %1 (%2).").arg(m_currentPlayer->name()).arg(m_currentPlayerIndex));
    l_steps->setText(QString("Steps left: %1").arg(m_stepsLeft));

    m_status->setText(text);

    // 2. Update view for immediate redraw.
    // m_scene->update(m_details->rect());
    m_scene->update(m_status->rect());
}

// ***************************************** HANDS SPECIFIC METHODS

void Table::addPainterPath(Hand::Side side, const QVector<QPointF>& polygon)
{
    // Create path out of polygon.
    QPainterPath path;
    path.moveTo(polygon.at(0));
    path.lineTo(polygon.at(1));
    path.lineTo(polygon.at(2));
    path.lineTo(polygon.at(3));
    path.lineTo(polygon.at(0));

    // Check if this side is free.
    // Fill it with new painter path if so.
    // Do nothing otherwise.
    if (!handAt(side))
        m_hands->append(new QPair<Hand::Side, QPainterPath>(side, path));
    else
        qDebug() << "This side is currently used by someone.";
}

QPair<Hand::Side, QPainterPath>* Table::handAt(const Hand::Side &side)
{
    // If list hasn't been initialized yet, let the user know of it and return nullptr.
    if (m_hands == nullptr)
    {
        qDebug() << "m_handZones list has not been initialized yet.";
        return nullptr;
    }

    // Otherwise, try to find the relevant pair with the side, that is the same as sent as parameter.
    // And return the corresponding painter path afterwards.
    for (int i = 0; i < m_hands->count(); ++i)
    {
        QPair<Hand::Side, QPainterPath> *found = m_hands->at(i);
        if (found->first == side)
            return found;
    }

    // If there is not any paths is the list with that side, just return the nullptr.
    return nullptr;
}

// ********************************************* NEIGHBOURS CHECKS

bool Table::hasNeighbourNode(const QPoint& pixelPosition, Player::Direction direction)
{
    return (getNodeAt(gridPosition(pixelPosition) + directionToVector(direction), true) != nullptr);
}

QBitArray Table::checkNeighbours (const QPoint &pixelPosition)
{
    // Neighbours are stored in an bitfield using next sequence: L-R-U-D-LU-LD-RU-RD

    QBitArray neighbours (8);

    neighbours.setBit(0, hasNeighbourNode(pixelPosition, Player::Direction::LEFT)  ? 1 : 0);
    neighbours.setBit(1, hasNeighbourNode(pixelPosition, Player::Direction::RIGHT) ? 1 : 0);
    neighbours.setBit(2, hasNeighbourNode(pixelPosition, Player::Direction::UP)    ? 1 : 0);
    neighbours.setBit(3, hasNeighbourNode(pixelPosition, Player::Direction::DOWN)  ? 1 : 0);

    neighbours.setBit(4, hasNeighbourNode(pixelPosition, Player::Direction::LEFT_UP)    ? 1 : 0);
    neighbours.setBit(5, hasNeighbourNode(pixelPosition, Player::Direction::LEFT_DOWN)  ? 1 : 0);
    neighbours.setBit(6, hasNeighbourNode(pixelPosition, Player::Direction::RIGHT_UP)   ? 1 : 0);
    neighbours.setBit(7, hasNeighbourNode(pixelPosition, Player::Direction::RIGHT_DOWN) ? 1 : 0);

    return neighbours;
}

// ************************************ OPERATIONS VALIDITY CHECKS

bool Table::addingIsValid(const QPoint &pixelPosition)
{
    // If there is an item on that position, do nothing.
    QGraphicsItem *item = m_view->itemAt(pixelPosition);
    if (item)
        return false;

    // Otherwise, find nearby neighbours.
    QBitArray neighbours = checkNeighbours(pixelPosition);

    int totalNeighbours = 0;
    for (int i = 0; i < neighbours.size(); ++i)
        totalNeighbours += neighbours.at(i);

    qDebug() << QString("L  - %1. R  - %2. U  - %3. D  - %4.").arg(neighbours.at(0)).arg(neighbours.at(1)).arg(neighbours.at(2)).arg(neighbours.at(3));
    qDebug() << QString("LU - %1. LD - %2. RU - %3. RD - %4.").arg(neighbours.at(4)).arg(neighbours.at(5)).arg(neighbours.at(6)).arg(neighbours.at(7));
    qDebug() << "There are " << totalNeighbours << " neighbours for position" << pixelPosition;

    return true;

    // Various constraints based on neighbouring items may be set here for placing new nodes to the grid.
    // if ((left && right) || (top && bottom)) return false;
    // if ((left && !top && !bottom) || (right && !top && !bottom) || (top && !left && !right) || (bottom && !left && !right)) return true;
    // else return false;
}

bool Table::removalIsValid(Node *node)
{
    if (m_nodes->count() == 1)
        return false;

    // If removed node will lead to a gap, we should leave it in piece.
    // Otherwise, send it to a sea of dangled pointers in memory oceans.

    QPoint actualPosition = node->boundingRect().center().toPoint();

    bool left   = hasNeighbourNode(actualPosition, Player::Direction::LEFT);
    bool right  = hasNeighbourNode(actualPosition, Player::Direction::RIGHT);
    bool top    = hasNeighbourNode(actualPosition, Player::Direction::UP);
    bool bottom = hasNeighbourNode(actualPosition, Player::Direction::DOWN);

    if ((left && right) || (left && top) || (left && bottom) || (right && top) || (right && bottom) || (top && bottom))
        return false;
    else
        return true;
}

// ******************************************* LOOKING FOR OBJECTS

Deck *Table::lookForDeckAt(const QPoint &pixelPosition)
{
    QTransform transform;
    transform.rotate(CARD_ANGLE);

    return dynamic_cast<Deck*> (m_scene->itemAt(pixelPosition, transform));
}

Token *Table::lookForTokenAt(const QPoint &pixelPosition)
{
    Node* node = getNodeAt(pixelPosition, false);
    if (node == nullptr)
        return nullptr;

    Token* token = node->token();
    return token;
}

Menu *Table::lookForMenuAt(const QPoint &pixelPosition)
{
    // Take an item on that position and try to convert the received pointer to the pointer to an instance of type Menu.
    // If the pointer is not nullptr, we'll received actual instance of Menu, nullptr otherwise.
    return dynamic_cast<Menu*>(m_scene->itemAt(pixelPosition, QTransform()));
}

UIElement *Table::lookForButtonAt(const QPoint &pixelPosition)
{
    // If there is no items in that position, return nullptr.
    QList<QGraphicsItem*> items = m_scene->items(pixelPosition);
    if (items.count() == 0)
        return nullptr;

    // Check objects in the list. If there is TokenDetails UI element there, set its pointer.
    Details* details = nullptr;
    for (int i = 0; i < items.count(); ++i)
    {
        QGraphicsItem* item = items.at(i);
        details = dynamic_cast<Details*>(item);
        if (details)
            break;
    }

    // If details item has been found, look for one of its buttons and return it if found.
    UIElement* button = nullptr;
    if (details)
    {
        button = details->buttonAtPosition(pixelPosition);
        if (button && button->role() == UIElement::Role::BUTTON)
        {
            qDebug() << "Found button with name: " << button->text();
            return button;
        }
    }

    return nullptr;
}

Node *Table::getNodeAt(const QPoint &position, bool onGrid)
{
    // If onGrid is true, position represents row and column indexes of the cell.
    // If onGrid is false, position represents simple pixel location somewhere in the cell.
    // For conversions between those, we have pixelPosition(gridPosition) and gridPosition(pixelPosition) methods.
    // Result could be either node with a calculated grid position or nullptr.

    Node* node = lookForNodeAt((onGrid) ? position : gridPosition(position));
    return node;
}

Node *Table::lookForNodeAt(const QPoint &gridPosition)
{
    qDebug() << "Looking for node at" << gridPosition;

    for (int i = 0; i < m_nodes->count(); ++i)
    {
        Node *node = m_nodes->at(i);

        if (node)
        {
            if (node->gridPosition() == gridPosition)
            {
                qDebug() << "Node found!";
                return node;
            }
        }
    }

    return nullptr;
}

Node *Table::findNodeByName(const QString &name)
{    
    Node *pretender = nullptr;
    for (int i = 0; i < m_nodes->count(); ++i)
    {
        Node* node = m_nodes->at(i);
        Token* token = node->token();
        ActionToken *at = dynamic_cast<ActionToken*>(token);

        if ((name == "start" && at && at->actionType() == ActionToken::ActionType::START) ||
            (name == "prison" && at && at->actionType() == ActionToken::ActionType::PRISON))
        {
            pretender = node;
            break;
        }
    }

    return pretender;
}

// ************************************************** CONVERSIONS

QPoint Table::gridPosition(const QPoint &pixelPosition)
{
    QPoint gridPosition = QPoint ((pixelPosition.x() - pixelPosition.x() % NODE_WIDTH) / NODE_WIDTH,
                                  (pixelPosition.y() - pixelPosition.y() % NODE_HEIGHT) / NODE_HEIGHT);

    // qDebug() << "Grid position: " << gridPosition;

    return gridPosition;
}

QPoint Table::pixelPosition(const QPoint &gridPosition)
{
    QPoint pixelPosition = QPoint (gridPosition.x() * NODE_WIDTH + NODE_WIDTH / 2,
                                   gridPosition.y() * NODE_HEIGHT + NODE_HEIGHT / 2);

    qDebug() << "Pixel position: " << pixelPosition;

    return pixelPosition;
}

QPoint Table::directionToVector(Player::Direction direction)
{
    switch (direction)
    {
        case Player::Direction::NO_MOVE:
            return QPoint(0,0);
            break;

        case Player::Direction::UP:
            return QPoint(0,-1);
            break;

        case Player::Direction::LEFT:
            return QPoint(-1,0);
            break;

        case Player::Direction::RIGHT:
            return QPoint(1,0);
            break;

        case Player::Direction::DOWN:
            return QPoint(0,1);
            break;

        case Player::Direction::LEFT_UP:
            return QPoint(-1,-1);
            break;

        case Player::Direction::LEFT_DOWN:
            return QPoint(-1,1);
            break;

        case Player::Direction::RIGHT_UP:
            return QPoint(1,-1);
            break;

        case Player::Direction::RIGHT_DOWN:
            return QPoint(1,1);
            break;
    }

    return QPoint();
}

// ****************************************** SAVING AND LOADING

void Table::saveTo(const QString &filename)
{
    QFile file (filename);
    if (file.open(QIODevice::WriteOnly))
    {
        QDataStream stream (&file);

        stream << m_nodes->count();

        for (int i = 0; i < m_nodes->count(); ++i)
            stream << *m_nodes->at(i);

        file.close();
    }
}

void Table::loadFrom(const QString &filename)
{
    QFile file (filename);
    if (file.open(QIODevice::ReadOnly))
    {
        QDataStream stream (&file);

        int nodesCount;
        stream >> nodesCount;

        if (nodesCount > 0)
        {
            clearNodes();
            clearUnits();
            hideUIItems();            

            for (int i = 0; i < nodesCount; ++i)
            {
                Node loaded;
                stream >> loaded;

                createNode(loaded.gridPosition());
            }

            showUIItems();
            addUnits();
        }

        file.close();
    }
}

// *************************************** DESCRIPTION SPECIFICS

QDomDocument Table::domFor(const QString &filename)
{
    //    qDebug() << "Loading tokens data: ";

    // 0. Check file existence and format.
    QFileInfo fi (filename);
    if (!fi.exists() || fi.completeSuffix() != "xml")
        return QDomDocument();

    // 1. Read file into xml string.
    QString xml = "";

    QFile file(filename);
    if (file.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&file);
        xml = stream.readAll();

        file.close();
    }

    // 2. Parse XML into DOM model.
    // Generate DOM document and find nodes with the description of the tokens.
    QDomDocument document;

    QString error;
    int error_line;

    bool ok = document.setContent(xml, false, &error, &error_line);    
    if (ok)
        qDebug() << "XML file has been parsed well.";
    else
        qDebug() << error << " Line: " << error_line;        

    return document;
}

void Table::loadDescriptions(const QString& filetype, const QString &filename)
{
    // 1. Load file into document object model.
    QDomDocument document = domFor(filename);
    if (document.isNull())
    {
        qDebug() << "Could not parse the file " << filename;
        return;
    }

    if (filetype == "ownership_tokens")
    {
        // 2. Select all elements with tag "ownership_token".
        QDomNodeList tokens = document.elementsByTagName("ownership_token");
        if (tokens.size() == 0)
            return;

        // 3. Prepare data list. Initialize the pointer or clear old data.
        if (m_OTDescription)
            clearOwnershipTokensData();

        // 4. Fill the list with data gathered from the file.
        // Walk through all the tokens in the list and parse the corresponding node
        // Store the result as OTD class instance and place it in a list for later use
        for (int i = 0; i < tokens.size(); ++i)
        {
            QDomNode token = tokens.at(i);

            QString index = token.toElement().attribute("index");

            QDomNodeList t = token.childNodes();
            QString name          = t.at(0).toElement().text();
            QString description   = t.at(1).toElement().text();
            QString imagePath     = t.at(2).toElement().text();
            QString buyingCost    = t.at(3).toElement().text();
            QString basicIncome   = t.at(4).toElement().text();
            QString upgradeCost   = t.at(5).toElement().text();
            QString upgradeLevel  = t.at(6).toElement().text();
            QString upgradeIncome = t.at(7).toElement().text();

            m_OTDescription->append(new Description(index.toInt(), name, description, imagePath, buyingCost, basicIncome, upgradeCost, upgradeLevel, upgradeIncome));

//            qDebug() << QString("%1. %2: %3. Image: %4. Buy for: %5. Income: %6. Upgrade: cost %7, level %8, income %9.")
//                        .arg(index).arg(name).arg(description).arg(imagePath).arg(buyingCost).arg(basicIncome).arg(upgradeCost).arg(upgradeLevel).arg(upgradeIncome);
        }
    }

    if (filetype == "action_tokens")
    {
        // 2. Select all elements with tag "action_token".
        QDomNodeList tokens = document.elementsByTagName("action_token");
        if (tokens.size() == 0)
            return;

        // 3. Prepare data list. Initialize the pointer or clear old data.
        if (m_ATDescription)
            clearActionTokensData();

        // 4. Fill the list with data gathered from the file.
        // Walk through all the tokens in the list and parse the corresponding node
        // Store the result as OTD class instance and place it in a list for later use
        for (int i = 0; i < tokens.size(); ++i)
        {
            QDomNode token = tokens.at(i);
            QDomNodeList t = token.childNodes();

            QString index = token.toElement().attribute("index");
            QString type = t.at(0).toElement().text();
            QString name = t.at(1).toElement().text();
            QString description = t.at(2).toElement().text();
            QString imagePath = t.at(3).toElement().text();

            m_ATDescription->append(new Description(index.toInt(), Description::ObjectType::ACTION_TOKEN, type, name, description, imagePath));

//            qDebug() << QString("%1. %2 - %3 (%4)").arg(index.toInt()).arg(name).arg(description).arg(imagePath);
        }
    }

    if (filetype == "cards")
    {
        // 2. Select all elements with tag "card".
        QDomNodeList cards = document.elementsByTagName("card");
        if (cards.size() == 0)
            return;

        // 3. Prepare list.
        if (m_CDescription)
            clearCardsData();

        // 4. Fill the list.
        for (int i = 0; i < cards.size(); ++i)
        {
            QDomNode card = cards.at(i);
            QDomNodeList c = card.childNodes();

            QString index = card.toElement().attribute("index");
            QString type = c.at(0).toElement().text();
            QString name = c.at(1).toElement().text();
            QString description = c.at(2).toElement().text();
            QString imagePath = c.at(3).toElement().text();

            m_CDescription->append(new Description(index.toInt(), Description::ObjectType::CARD, type, name, description, imagePath));
        }

    }
}

void Table::makeTokenFromDescription(const QPoint &gridPosition, const TokenType& tokenType, int index)
{
    // 0. Check parameters for validity.
    Node* node = getNodeAt(gridPosition, true);
    if (node == nullptr)
    {
        qDebug() << "There is no node on grid position " << gridPosition << ".";
        return;
    }

    switch (tokenType)
    {
        case TokenType::ACTION:
        {
            // 0. Checks:
            // - existence of the data list with tokens descriptions;
            // - validity of index value.
            if (m_ATDescription == nullptr)
            {
                qDebug() << "Tokens data list wasn't initialized yet.";
                return;
            }

            if (index < 0 || index > m_ATDescription->count())
            {
                qDebug() << "Index should be in range [0; count of elements in tokens data list].";
                return;
            }

            // 1. Get token description data from loaded xml.
            Description* d = m_ATDescription->at(index);

            // 2. Create new token using these pieces of information.
            ActionToken *token = new ActionToken(d);
            token->setRect(node->rect().adjusted(10, 10, -10, -10));

            // 3. Set new ownership token to the node.
            node->setToken(token);
        }
        break;

        case TokenType::OWNERSHIP:
        {
            // 0. Checks:
            // - existence of the data list with tokens descriptions;
            // - validity of index value.
            if (m_OTDescription == nullptr)
            {
                qDebug() << "Tokens data list wasn't initialized yet.";
                return;
            }

            if (index < 0 || index > m_OTDescription->count())
            {
                qDebug() << "Index should be in range [0; count of elements in tokens data list].";
                return;
            }

            // 1. Get token description data from loaded xml.
            Description* d = m_OTDescription->at(index);

            // 2. Create new token using these pieces of information.
            OwnershipToken *token = new OwnershipToken(d);
            token->setRect(node->rect().adjusted(10, 10, -10, -10));

            // 3. Set new ownership token to the node.
            node->setToken(token);

        }
        break;

    }
}

void Table::makeCardFromDescription(const Deck::DeckType &deckType, int index)
{
    if (deckType == Deck::DeckType::POSITIVE)
        m_cardsP->add(new Card(m_CDescription->at(index)));

    if (deckType == Deck::DeckType::NEGATIVE)
        m_cardsN->add(new Card(m_CDescription->at(index)));
}

ActionToken *Table::ATFor(int index)
{
    Q_ASSERT_X(index >= 0 && index < m_ATDescription->count(), "Table::actionTokenFor", "Index should be in range of AT descriptions list.");

    ActionToken* actionToken = new ActionToken(m_ATDescription->at(index));
    qDebug() << "Generated AT using description index " << index << ". Image: " + actionToken->imagePath();

    return actionToken;
}

OwnershipToken *Table::OTFor(int index)
{
    Q_ASSERT_X(index >= 0 && index < m_OTDescription->count(), "Table::ownershipTokenFor", "Index should be in range of OT descriptions list.");

    OwnershipToken* ownershipToken = new OwnershipToken(m_OTDescription->at(index));
    qDebug() << "Generated OT using description index " << index << ". Image: " + ownershipToken->imagePath();

    return ownershipToken;
}

Card *Table::CFor(int index)
{
    Q_ASSERT_X(index >= 0 && index < m_CDescription->count(), "Table::cardFor", "Index should be in range of cards descriptions list.");

    Card* card = new Card(m_CDescription->at(index));
    qDebug() << "Generated card using description index " << index << ". Front foreground image: " << card->imageFrontFG();

    return card;
}

// *********************************************** TESTING STUFF

void Table::fillNodesWithRandomTokens()
{
    // Testing method to check behaviour of tokens

    // Check the existence of nodes and descriptions of tokens.
    if (!m_nodes)
        qDebug() << "List of nodes have not been initialized yet.";

    if (!m_ATDescription)
        qDebug() << "List of action tokens descriptions have not been initialized yet.";

    if (!m_OTDescription)
        qDebug() << "List of ownership tokens descriptions have not been initialized yet.";

    // Fill the nodes with random tokens.
    for (int i = 0; i < m_nodes->count(); ++i)
    {
        QPoint position = m_nodes->at(i)->gridPosition();

        int r = 1; // rand() % 2;
        if (r == 0)
            makeTokenFromDescription(position, TokenType::OWNERSHIP, rand() % m_OTDescription->count());
        else
            makeTokenFromDescription(position, TokenType::ACTION, (rand()%6 == 1) ? 6 : 4 + rand()%2); // rand() % m_ATDescription->count()); // rand() % m_actionTokensData->count()); // 2 + rand() % 2);
    }
}

void Table::fillDecksWithRandomCards()
{
    // Testing method to check behaviour of cards

    // Check existence of the descriptions and decks.
    if (!m_CDescription)
        qDebug() << "Cards descriptions have not been loaded yet";

    if (!m_cardsP)
        qDebug() << "There is no deck of cards with positive bonuses yet.";

    if (!m_cardsN)
        qDebug() << "There is no deck of cards with negative bonuses yet.";

    // Fill the decks with random cards.
    for (int i = 0; i < m_cardsP->maxSize(); ++i)        
        makeCardFromDescription(Deck::DeckType::POSITIVE, 8); // another really random number oO // rand() % 7); // indexes from 0 to 6

    for (int j = 0; j < m_cardsN->maxSize(); ++j)
        makeCardFromDescription(Deck::DeckType::NEGATIVE, 8); // really random number :) // 7 + rand() % 7); // indexes for 7 to 13
}

void Table::fillHandsWithRandomTokens()
{
    // Testing method to fill the hands of active players with some random ownership tokens.

    // 1. Check the existence of players.
    if (!m_units || m_units->count() == 0)
    {
        qDebug() << "There is no list of players yet.";
        return;
    }

    // 2. Check the existence of ownership tokens descriptions.
    if (!m_OTDescription || m_OTDescription->count() == 0)
    {
        qDebug() << "There is no ownership tokens descriptions yet.";
        return;
    }

    // 3. Add {count} random OT to each of the players.
    int count = 5;
    for (int i = 0; i < m_units->count(); ++i)
    {
        Player* p = m_units->at(i);
        if (p->hand())
        {
            int index;
            for (int ot = 0; ot < count; ++ot)
            {
                index = rand() % m_OTDescription->count();
                p->hand()->addToken(new OwnershipToken(m_OTDescription->at(index)));
            }
        }
        else
            qDebug() << QString("Player %1 hasn't any hands yet.").arg(p->name());

    }
}

Node *Table::randomNode()
{
    int idx = rand() % m_nodes->count();

    return m_nodes->at(idx);
}

Token *Table::dummyToken()
{
    Token *token = new Token("abc", "abc", "abc");

    return token;
}

Player *Table::randomOpponent()
{
    QList<Player*> opponents;
    for (int i = 0; i < m_units->count(); ++i)
    {
        Player* p = m_units->at(i);
        if (p != m_currentPlayer)
            opponents.append(p);
    }

    qDebug() << QString("There are %1 opponents").arg(opponents.count());

    Player* opponent = opponents.at(rand() % opponents.count());
    if (opponent == nullptr)
        qDebug() << "There is only one player exists.";

    return opponent;
}

void Table::unusedCodeCemetery()
{

}

// ****************************************************** MOVEMENT

void Table::startMovement (Player* player)
{
    // Update token details item.
    m_details->clearSelection();
    m_details->hide();

    // If current player is in prison, he can not move for some turns.
    qDebug() << "Current player is blocked for " << player->blocked() << " turns.";
    if (player->blocked() > 0)
    {
        m_currentPlayer->decreaseBlock();
        return;
    }

    m_movingPlayer = player;

    // Start movement timer.
    connect(&m_autoMovementTimer, SIGNAL(timeout()), this, SLOT(autoMovement()));
    m_autoMovementTimer.start(150/m_movementSpeed);
}

void Table::autoMovement()
{
    // In cas moving player is not current (when control cards are activated, for example), we'll use another pointer to handle these situations.
    qDebug() << "Current player exists: " << (m_movingPlayer != nullptr) << ". Name: " << m_movingPlayer->name();

    if (m_movingPlayer)
    {        
        stepAuto(m_movingPlayer);

        Token* token = getNodeAt(m_movingPlayer->gridPosition(),true)->token();
        ActionToken* AT = dynamic_cast<ActionToken*>(token);
        OwnershipToken* OT = dynamic_cast<OwnershipToken*>(token);

        // give rewards for passed circle even if it is not the end node for current turn
        if (AT && AT->actionType() == ActionToken::ActionType::START)
            action(AT->actionType());

        if (m_stepsLeft == 0)
        {
            // Stop movement timer.
            m_autoMovementTimer.stop();
            disconnect(&m_autoMovementTimer, SIGNAL(timeout()), this, SLOT(autoMovement()));

            m_currentPlayer = m_movingPlayer;
            m_movingPlayer = nullptr;

            // Default the movement constraint and speed.
            if (m_constraintCurrent != m_constraintDefault)
            {
                m_constraintCurrent = m_constraintDefault;

                Player::Direction nDir = Player::Direction::NO_MOVE;
                Player::Direction cDir = m_currentPlayer->direction();

                if (cDir == Player::Direction::UP) nDir = Player::Direction::DOWN;
                if (cDir == Player::Direction::DOWN) nDir = Player::Direction::UP;
                if (cDir == Player::Direction::LEFT) nDir = Player::Direction::RIGHT;
                if (cDir == Player::Direction::RIGHT) nDir = Player::Direction::LEFT;

                m_currentPlayer->setDirection(nDir);
            }

            m_movementSpeed = 1;

            // Check where the player ended his turn. Here various actions based on this circumstance may be activated.
            qDebug() << "Player " << m_currentPlayer->name() << " ended his turn on token: " << (AT ? AT->name() : OT ? OT->name() : "empty") << ".";
            if (AT) action(AT->actionType());
            if (OT) ;

            // Update details item.
            if (m_currentPlayer)
                qDebug() << "Current player finished his turn, but the pointer to him is still relevant.";

            m_details->show();
            m_details->showButtons();
            m_details->setPlayer(m_currentPlayer);
            m_details->setToken(OT);
            m_details->update();
        }
    }
}

void Table::autoMovementTogether()
{
    qDebug() << "here";

    for (int i = 0; i < m_units->count(); ++i)
    {
        Player* player = m_units->at(i);
        stepAuto(player);

        if (m_stepsLeft == 0)
        {
            // Stop
            m_autoMovementTogetherTimer.stop();
            disconnect(&m_autoMovementTogetherTimer, SIGNAL(timeout()), this, SLOT(autoMovementTogether()));
        }
    }
}

void Table::setMovementConstraint(Constraint constraint)
{
    m_constraintCurrent = constraint;
}

void Table::setMovementSpeed(int value)
{
    if (value >= MIN_MOVEMENT_SPEED and value <= MAX_MOVEMENT_SPEED)
        m_movementSpeed = value;
}

void Table::stepAuto(Player* player)
{
    QPoint pos = player->gridPosition();
    Player::Direction dir = player->direction();

    qDebug() << QString("Player: %1. Position: (%2,%3).").arg(player->name()).arg(pos.x()).arg(pos.y()); // .arg(dir.);
    if (dir == Player::Direction::NO_MOVE)
        qDebug() << "There isn't any movements.";

    // What if...
    // 1. Separate corners based of awailable ways.
    // 2. Separate horizontals and verticals based on what. Class instance to hold the line? Grid position checking?
    // Or some sort of *edges* between nodes? Maybe edges, maybe not, who knows.

    // Lets say, we have a map like this:
    // *****R***
    // *       D
    // **  **L**
    //  U  D
    // **  *R*
    // *     D
    // *****L*

    // There are four types of corners.
    // Player can approach them for two different sides.
    // That means, he has direction in which he moves, and some type of corner. And needs to know, which direction to take next.

    // Meaning of bits in the neighbours bit array: L-R-U-D-LU-LD-RU-RD
    QBitArray neighbours = checkNeighbours(pixelPosition(pos));
    bool l = neighbours.at(0), r = neighbours.at(1), u = neighbours.at(2), d = neighbours.at(3);

    // Lets try to change priority direction on corners
    bool ulC = !u &&  r &&  d && !l;
    bool urC = !u && !r &&  d &&  l;
    bool lrC =  u && !r && !d &&  l;
    bool llC =  u &&  r && !d && !l;

    qDebug() << QString("UL: %1. UR: %2. LR: %3. LL: %4").arg(ulC).arg(urC).arg(lrC).arg(llC);

    // For clockwise movement, units move in sequence UPPER->RIGHT->BOTTOM->LEFT line.
    if (m_constraintCurrent == Constraint::CLOCKWISE)
    {
        // upper left corner // upper right corner // lower left corner // lower right corner
        // ** (left->down)   // ** (right->down)   // *  (left->up)     //  * (right->up)
        // *  (up->right)    //  * (up->left)      // ** (down->right)  // ** (down->left)

        if (ulC) player->setDirection(player->direction() == Player::Direction::LEFT  ? Player::Direction::DOWN : Player::Direction::RIGHT);
        if (urC) player->setDirection(player->direction() == Player::Direction::RIGHT ? Player::Direction::DOWN : Player::Direction::LEFT);
        if (lrC) player->setDirection(player->direction() == Player::Direction::RIGHT ? Player::Direction::UP   : Player::Direction::LEFT);
        if (llC) player->setDirection(player->direction() == Player::Direction::LEFT  ? Player::Direction::UP   : Player::Direction::RIGHT);

        step (player, player->direction());

    }

    // For counter clockwise movement, units move in sequence LEFT->BOTTOM->RIGHT->UPPER line.
    if (m_constraintCurrent == Constraint::COUNTER_CLOCKWISE)
    {
        // upper left corner // upper right corner // lower left corner // lower right corner
        // ** (left->down)   // ** (right->down)   // *  (left->up)     //  * (right->up)
        // *  (up->right)    //  * (up->left)      // ** (down->right)  // ** (down->left)

        // As for counter clockwise movement, the affect of corner positions on next direction is a bit different.
        if (ulC) player->setDirection(player->direction() == Player::Direction::UP ? Player::Direction::RIGHT : Player::Direction::DOWN);
        if (urC) player->setDirection(player->direction() == Player::Direction::RIGHT ? Player::Direction::DOWN : Player::Direction::LEFT);
        if (lrC) player->setDirection(player->direction() == Player::Direction::DOWN ? Player::Direction::LEFT : Player::Direction::UP);
        if (llC) player->setDirection(player->direction() == Player::Direction::LEFT ? Player::Direction::UP : Player::Direction::RIGHT);

        step (player, player->direction());
    }
}

void Table::step(Player *unit, Player::Direction direction)
{
    qDebug() << "Current unit selected: " << (unit != nullptr);

    qDebug() << "in Table::move";
    qDebug() << " unit position: " << unit->gridPosition();
    qDebug() << "check position: " << unit->gridPosition() + directionToVector(direction);

    QPoint prevPosition = unit->gridPosition();
    QPoint currPosition = unit->gridPosition() + directionToVector(direction);

    bool moveIsAvailable = (getNodeAt(currPosition, true) != nullptr);
    if (moveIsAvailable)
    {
        Node* prev = getNodeAt(prevPosition, true);
        Node* curr = getNodeAt(currPosition, true);

        prev->setActive(false);
        curr->setActive(true);

        unit->setGridPosition(curr->gridPosition());
        unit->setRect(curr->rect());

        --m_stepsLeft;

        updateUI();
    }
}

void Table::nextPlayer()
{
    // Choose index on a circular basis.
    if (m_units->indexOf(m_currentPlayer) == m_units->count() - 1)
        m_currentPlayerIndex = 0;
    else
        ++m_currentPlayerIndex;

    // Change current unit and generate new random value as steps.
    m_currentPlayer = m_units->at(m_currentPlayerIndex);
    // if (m_currentPlayer->direction() == Player::Direction::NO_MOVE)
    //     m_currentPlayer->setDirection(m_constraintCurrent == Constraint::CLOCKWISE ? Player::Direction::RIGHT : Player::Direction::LEFT);
}

int  Table::dropDie(int low, int high)
{
    Q_ASSERT_X(low >= 0 && high <= 100, "Table::dropDie", "low should be greater than 0, high should be less than 100");

    return (low + rand() % high);
}

void Table::turn()
{
    // List has not been defined or count of units is insufficient.
    if (!m_units || m_units->count() < 2)
        return;

    nextPlayer();
    m_stepsLeft = dropDie(1,6);
    updateUI();
}

// ****************************************************** SLOTS

void Table::viewMousePositionChanged (const QPoint& mousePosition)
{
    m_viewMP = mousePosition;
    if (m_mode == Mode::PLAY)
        m_viewMP -= QPoint(handwidth, handwidth);


    qDebug() << m_viewMP;

    // View specific events, that depend on moving
    Menu* menu = lookForMenuAt(m_viewMP);
    if (menu)
    {
        qDebug() << "Menu is found at position: " << m_viewMP;

        UIElement* button = menu->buttonAtPosition(m_viewMP);
        if (button)
        {
            qDebug() << button->text();

            menu->setState(Menu::State::BUTTON_HOVER);

            button->setColor(UIElement::Element::FILL, QColor("#fac404"));
            button->setFont(QFont("Truetypewriter PolyglOTT", 13));
            button->setText(QString(">>> %1 <<<").arg(button->defaultText()));
        }
        else
            menu->setState(Menu::State::MENU_HOVER);

        m_scene->update(menu->rect());
    }
}

void Table::onEditMode()
{
    bool inEditState = pb_editMode->isChecked();

    if (inEditState)
        m_mode = Mode::EDIT;
    else
        m_mode = Mode::PLAY;
}

void Table::onSaveMap()
{
    QString filename = QFileDialog::getSaveFileName(nullptr, "Choose map file to save to", QString(), "*.tm");

    saveTo (filename);
}

void Table::onLoadMap()
{
    QString filename = QFileDialog::getOpenFileName(nullptr, "Choose map file to load from", QString(), "*.tm");

    loadFrom(filename);
}

void Table::onDefaults()
{
    clearNodes();
    clearDecks();
    clearUnits();

    loadFrom("not_round.tm");
    if (m_units->isEmpty())
        addUnits();

    fillNodesWithRandomTokens();
    fillDecksWithRandomCards();
    fillHandsWithRandomTokens();
}

void Table::onTurn()
{
    turn();
    startMovement(m_currentPlayer);
}
