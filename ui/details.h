#ifndef DETAILS_H
#define DETAILS_H

#include <QGraphicsRectItem>

#include "nodes/tokens/actiontoken.h"
#include "nodes/tokens/ownershiptoken.h"
#include "cards/card.h"

#include "ui/uielement.h"

class Player;

class Details : public QGraphicsRectItem
{
public:
     Details();
    ~Details();

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) override;

    void setPlayer(Player* player);
    void setToken (Token*  token);
    void setCard  (Card*   card);

    ActionToken*    AT()   const;
    OwnershipToken* OT()   const;
    Card*           card() const;

    UIElement* buttonAtPosition(const QPoint& pixelPosition);    
    UIElement* buyButton();
    UIElement* upgradeButton();
    UIElement* useButton();
    UIElement* closeButton();

    void clearSelection();
    void createButtons();
    void showButtons();
    void hideButtons();    

private:    
    void deleteButtons();

    // Drawing methods.
    void drawBackground (QPainter* painter);
    void drawAT      (QPainter* painter);
    void drawOT      (QPainter* painter);
    void drawCard    (QPainter* painter);
    void drawButtons (QPainter* painter);

    // Helper methods.    
    UIElement* createButton(const QString& text, const QSize& size, const QPoint& position);
    void         hideButton(UIElement* button);
    void         showButton(UIElement* button);
    QString upgradeMessage();

    // Object pointers. Instance of this class may have different behaviour based on currently used object.
    QGraphicsScene* m_scene          = nullptr;
    Player*         m_currentPlayer  = nullptr;
    ActionToken*    m_actionToken    = nullptr;
    OwnershipToken* m_ownershipToken = nullptr;
    Card*           m_card           = nullptr;

    bool m_buttonsCreated = false;
    UIElement* m_buyButton     = nullptr;
    UIElement* m_upgradeButton = nullptr;
    UIElement* m_useButton     = nullptr;
    UIElement* m_closeButton   = nullptr;
};

#endif // DETAILS_H
