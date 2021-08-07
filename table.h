#ifndef TABLE_H
#define TABLE_H

#include <QGraphicsScene>
// #include <QGraphicsView>
#include "ui/view.h"

#include <QLabel>
#include <QPushButton>
#include <QGridLayout>

#include <QBitArray>
#include <QTimer>
#include <QList>

#include <QtXml/QDomDocument>
#include <QtXml/QDomNodeList>
#include <QtXml/QDomElement>

#include "helper/description.h"
#include "nodes/node.h"
#include "nodes/tokens/actiontoken.h"
#include "nodes/tokens/ownershiptoken.h"
#include "cards/deck.h"
#include "player/player.h"

#include "ui/historylabel.h"
#include "ui/uielement.h"
#include "ui/details.h"
#include "ui/menu.h"

class Table : public QWidget
{
    Q_OBJECT

public:
    enum class Constraint {CLOCKWISE, COUNTER_CLOCKWISE, UNCONSTRAINED};
    enum class TokenType  {ACTION, OWNERSHIP};
    enum class Mode       {MENU, PLAY, EDIT};

    Table(QWidget *parent = nullptr);
    ~Table();    

    // Event handlers for interaction with keyboard and mouse.
    // These are overrides of relevant virtual QWidget methods.
    // - keyPressEvent is used for moving purposes;
    // - mousePressEvent is used for interaction with cards, nodes and tokens in play or edit mode;
    // - mouseDoubleClickEvent is used to activate the node editor in edit mode.
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;    

private:
    // Editing or playing
    void setMode (const Mode& m_mode);
    Mode m_mode = Mode::PLAY;

    void hideMenu();
    void showMenu();
    void newGame();
    void quit();

    // Initialization.
    // These are the methods to:
    // - setup scene and view;
    // - draw the grid lines for table and hands regions;
    // - add objects to the scene;
    // - compose user interface and update it.
    void activateHistory();
    void prepareScene();
    void addGrid();
    void addNodes();
    void addHands();
    void addUnits();
    void addDecks();    

    // Cleaning.
    // Just as name of the category says, these methods are used for cleaning all the unused pointers,
    // objects and collections of objects. Remarkably, used in destructor for cleaning before instance deleting.
    void clearScene();
    void clearNodes();
    void clearDescriptions();
    void clearOwnershipTokensData();
    void clearActionTokensData();
    void clearCardsData();
    void clearHands();
    void clearUnits();
    void clearDecks();
    void freePointers();
    void nullifyPointers();

    // Serialization
    // * saveTo and loadFrom methods are used to save and load the generated map;
    // * loadTokensData and domFor allow fetching the tokens data from outer XML file.
    void saveTo (const QString& filename);
    void loadFrom (const QString& filename);    
    void loadDescriptions (const QString& filetype, const QString& filename);
    QDomDocument domFor  (const QString& filename);

    // Checkers
    // * removalIsValid method returns true, if the node may be removed from the scene;
    // * addingIsValid method returns true, if the node may be added to the scene at specific position;
    // * lookFor... methods try to find an object on specific position(pixel or grid) and return it if found, nullptr otherwise;
    // * getNodeAt returns node at specific position or nullptr;
    // * checkNeighbours check all 8 neighbouring nodes for pixelPosition and returns bit array of specific format, that may be used by other methods;
    // * hasNeighbourNode returns true, if there is a node at specific direction from some pixelPosition.
    bool  removalIsValid (Node* node);
    bool  addingIsValid  (const QPoint& pixelPosition);
    Deck*  lookForDeckAt (const QPoint& pixelPosition);
    Token* lookForTokenAt(const QPoint& pixelPosition);
    Menu*  lookForMenuAt (const QPoint& pixelPosition);
    UIElement* lookForButtonAt (const QPoint& pixelPosition);

    Node* getNodeAt (const QPoint& position, bool onGrid);
    Node* lookForNodeAt (const QPoint& gridPosition);
    Node* findNodeByName(const QString& name);

    QBitArray checkNeighbours (const QPoint& pixelPosition);
    bool hasNeighbourNode (const QPoint& pixelPosition, Player::Direction direction);

    // Conversions:
    // - gridPosition and pixelPosition allow conversions between pixel and scene locations;
    // - directionToVector converts direction enum value to actual vector.
    QPoint gridPosition (const QPoint& pixelPosition);
    QPoint pixelPosition (const QPoint& gridPosition);
    QPoint directionToVector (Player::Direction direction);

    // Scene and view:
    // - tableWidth and tableHeight represent size of the actual table region without hands regions;
    // - handsSpace used as both dimensions for hands regions, that are located to each side from the table,
    //   it should also include some sort of margin to separate hands region from actual table region;
    // - HANDS_SPACE constant is the value of the width and height of any hands region;
    // - SCENE_MARGIN constant is the value of the margin, that separated hands and table regions;
    // - m_scene and m_view are pointers to the model of the scene and connected visual representation;
    //   since they are nullptr currently, there should be some method to initialize them.
    int basewidth, baseheight, handwidth;
    const int HANDS_SPACE = 70;
    const int SCENE_MARGIN = 5;
    QGraphicsScene *m_scene = nullptr;
    View           *m_view = nullptr;
    QPoint          m_viewMP;

    // Nodes:
    // * createNode is the factory method to create node at specific grid position and place it on the scene;
    // * addNode is the helper method to add generated node to the scene, if it is not there yet;
    // * removeNode is the helper method to remove existing node from the scene, if it is there;
    // * editNode is the method to make the interaction with editor dialogue possible;
    // - NODE_WIDTH and NODE_HEIGHT are basic values of each nodes' sizes;
    // - NODES_PER_ROW and NODES_PER_COLUMN used to set maximum count of nodes that can be placed on table;
    // - m_nodes is the storage for all placed nodes, used for interaction with them.
    Node* createNode  (const QPoint& gridPosition);
    void  addNode (Node* n);
    void  removeNode (Node* n);
    void  editNode (Node* n);

    const int NODE_WIDTH = 88;
    const int NODE_HEIGHT = 88;
    const int NODES_PER_ROW = 8;
    const int NODES_PER_COLUMN = 8;
    QList<Node*> *m_nodes = nullptr;

    // Tokens:
    // These store the information of all tokens, that are loaded from XML files.
    // May be used by factory methods to create actual tokens and placing them to nodes.
    // * ATFor method creates new action token using description data from the m_ATDescription list item with specific index;
    // * OTFor method creates new ownership token using description data from the m_OTDescription list item with specific index;
    // * CFor  method creates new card using description data from m_CDescription list with specific index;
    // * addToken creates token of specific tokenType and index, and places it for node at gridPosition;
    // * addCard  creates card  of specific  deckType and index, and places it into corresponding deck;
    // - m_ownershipTokensData is the storage for all description objects, created after loading of ot.xml file;
    // - m_actionTokensData    is the storage for all description objects, created after loading of at.xml file.    
    ActionToken*    ATFor (int index);
    OwnershipToken* OTFor (int index);
    Card*           CFor  (int index);

    void makeTokenFromDescription (const QPoint& gridPosition, const TokenType& tokenType, int index);
    void makeCardFromDescription  (const Deck::DeckType& deckType, int index);

    QList<Description*> *m_OTDescription = nullptr;
    QList<Description*> *m_ATDescription = nullptr;
    QList<Description*> *m_CDescription  = nullptr;

    // Units:
    // * createUnit is the factory method to create unit at specific grid position and description;
    // * addUnit is the helper method to add generated unit to the scene, if it is not there yet;
    // * removeUnit is the helper method to remove existing unit from the scene, if it is there;
    // - UNIT_WIDTH and UNIT_HEIGHT responsible for size of the players' unit;
    // - m_units is the storage for all players, that are currently in game;
    // - m_currentPlayer represents the player, that should make his turn next;
    // - m_currentPlayerIndex is the helper variable, used to select the current player from the list of all available units.
    Player* createUnit  (const QPoint& gridPosition, const QString& name, const QColor& color, const QString& imagePath);
    void addUnit (Player* u);
    void removeUnit (Player* u);

    const int UNIT_WIDTH = 45;
    const int UNIT_HEIGHT = 45;
    QList<Player*> *m_units = nullptr;    
    Player* m_currentPlayer = nullptr;
    Player* m_movingPlayer = nullptr;
    int m_currentPlayerIndex = -1;

    // Cards:
    // * createCard is the factory method to create card with specific description and type;
    // * addCard is the helper method to add generated card to the scene, if it is not there yet;
    // * removeCard is the helper method to remove existing card from the scene, if it is there;
    // - CARD_WIDTH and CARD_HEIGHT are the size of the card in the deck;
    // - CARD_ANGLE used to rotate the deck on the table and place it using angle, different from default one;
    // - m_cardsP represents pointer to the deck of cards, that give the positive bonuses to the player;
    // - m_cardsN represents pointer to the deck of cards, that give the negative bonuses to the player.
    // Make sure to initialize these in some methods before use.
    Card* createCard  (const QString& name, const QString& description, const QString& imagePath, bool isPositive);
    void addCard (Card* c, bool isPositive);
    void removeCard (Card* c, bool isPositive);

    const int CARD_WIDTH = 90;
    const int CARD_HEIGHT = 135;
    const float CARD_ANGLE = 21.f;
    Deck* m_cardsP = nullptr;
    Deck* m_cardsN = nullptr;
    Card* m_currentCard = nullptr;

    // Movement:
    // Since this is the editor of nodes, which could be extended to use some sort of solid or passable blocks,
    // cost and nodes movement based of the graph search algorithms (A*, Dijkstra, Kraskal-Prim etc.), as well
    // as for monopoly-like board games, the movement type can be controlled through Constraint enumeration.
    // * setMovementConstraint method allows to select one of the affordable constraint types;
    // * startAutoMovement method initializes timer and connects it to the relevant slot, that performs auto movement;
    // * stepAuto method makes one step of the current player in an automatic regime;
    // * action method is called, when player ends his turn on one of the nodes with action tokens;
    // * step method just makes the movement of unit in a specific direction, if it is allowed;
    // * turn method passes the turn to next player in a list in a circular way.
    // - m_constraint used to allow user to change the constrain type on the fly;
    // - m_movingTimer used to make the movement of units step by step instead of quick change of the position,
    //   through connection the slot that makes the steps and checks the count of steps left for current player,
    //   when there are no steps left, the timer detaches from slot and waits for activation by next player.
    // - m_stepsLeft represents count of steps for current player;
    //   when user clicks turn, the random value from 1 to 6 is generated and set and its value,
    //   random seed ensures it would be different of each running of the application.    
    void setMovementConstraint  (Constraint constraint);
    void setMovementSpeed  (int value);
    void startMovement (Player* player);
    void stepAuto          (Player* player);
    void step (Player* unit, Player::Direction direction);
    void turn ();

    void action   (ActionToken::ActionType actionType);
    void activate (Card* card);

    Constraint m_constraintCurrent = Constraint::UNCONSTRAINED;
    Constraint m_constraintDefault = Constraint::COUNTER_CLOCKWISE;
    QTimer m_autoMovementTimer, m_autoMovementTogetherTimer;
    int m_stepsLeft = 0;

    const int MIN_MOVEMENT_SPEED = 1;
    const int MAX_MOVEMENT_SPEED = 5;
    int m_movementSpeed = 1;

    // UI:
    // * makeUI makes all necessary widgets and items representing UI and composes the view of the app;
    // * updateUI makes user interface interactable: updates data on different widgets and items;
    // Application uses these widgets:
    // - pb_editMode is toggle button to turn to editor mode and vise versa;
    // - pb_saveMap and pb_loadMap are buttons to save the created map into some file and load from it correspondingly;
    // - pb_defaults used for loading of the default map and filling test tokens there, useful for testing purposes;
    // - pb_turn is the button to choose the current player and move his unit on the board;
    // - l_turn and l_steps are status label showing the current player index and name, as well as left count of steps;
    // - m_layout used for composing the user interface and setting it for this widget.
    // Besides widgets, there are also graphics items that are used as UI objects:
    // - m_currentPlayerStatus represents some sort of shape with text inside.
    // - m_tokenDetails is used to show the detailed description of the token player stands on currently.
    void addMenu();
    void addUIItems();
    void addUIWidgets();
    void hideUIItems();
    void showUIItems();
    void updateUI();

    HistoryLabel* l_history  = nullptr;
    QPushButton *pb_editMode = nullptr;
    QPushButton *pb_saveMap  = nullptr;
    QPushButton *pb_loadMap  = nullptr;
    QPushButton *pb_defaults = nullptr;
    QPushButton *pb_turn     = nullptr;
    QLabel      *l_turn      = nullptr;
    QLabel      *l_steps     = nullptr;
    QGridLayout *m_layout    = nullptr;

    Menu      *m_menu    = nullptr;
    Details   *m_details = nullptr;
    UIElement *m_status  = nullptr;

    // Hand UI:
    void addPainterPath(Hand::Side side, const QVector<QPointF>& polygon);

    QPair<Hand::Side, QPainterPath>* handAt(const Hand::Side& side);
    QList<QPair<Hand::Side, QPainterPath>*> *m_hands = nullptr;

    // Testing:
    // - dummyToken returns empty token to fill the node at least with something;
    // - randomNode returns randomly chosen node from the list of available;
    // - fillRandomTokens fill all the existing nodes with randomly chosen tokens.
    Token* dummyToken();
    Node*  randomNode();
    void fillNodesWithRandomTokens();
    void fillHandsWithRandomTokens();
    void fillDecksWithRandomCards();
    void unusedCodeCemetery();

    Player* randomOpponent();
    void nextPlayer();
    int  dropDie(int low, int high);

public slots:
    void autoMovement();
    void autoMovementTogether();

    void viewMousePositionChanged(const QPoint& mousePosition);
    void onEditMode();
    void onSaveMap();
    void onLoadMap();
    void onDefaults();
    void onTurn();
};

#endif // TABLE_H
