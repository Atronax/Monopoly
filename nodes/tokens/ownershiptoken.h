#ifndef OWNERSHIPTOKEN_H
#define OWNERSHIPTOKEN_H

#include "token.h"
#include "helper/description.h"

class Player;

// Besides NAME, DESCRIPTION and IMAGE, each ownership token should have:
// 1. BUYINGCOST: price, at which the company may be bought by player.
// 2. UPGRADECOST: price, at which the company may be upgraded.
// 3. BASICINCOME: the basic returns player will get at each passed circle.
// 4. UPGRADEINCOME: additional bonus player will get at each passed circle.
// 5. UPGRADELEVEL: current progress of the company, influences the returns and stars on the image.

// OwnershipTokenDescription class will be used to store the data received from XML to use it later
// to create instances using only indexes of elements stores in corresponding list.

class OwnershipToken : public Token
{
public:
    OwnershipToken(const QString& name, const QString& description, const QString& imagePath);
    OwnershipToken(Description* otd);
    virtual ~OwnershipToken();

    void setBuyingCost    (int buyingCost);
    void setBasicIncome   (int basicIncome);
    void setUpgradeLevel  (int level);
    void setUpgradeCost   (const QVector<int>& upgradeCost);
    void setUpgradeIncome (const QVector<int>& upgradeIncome);    

    int  buyingCost   () const;
    int  basicIncome  () const;
    int  upgradeLevel () const;
    int  cUpgradeCost () const;
    const QVector<int>& upgradeCost  () const;
    const QVector<int>& upgradeIncome() const;

    bool hasOwner () const;
    void setOwner (Player* player);
    void setThumbnailRegion (const QRectF& thumbnailRegion);

    Player* owner () const;
    const QRectF& thumbnailRegion() const;

    void upgrade(bool bonus);
    int  income();

    void activate() override;

    friend QDataStream& operator<<(QDataStream &out, const OwnershipToken &t);
    friend QDataStream& operator>>(QDataStream &in,  OwnershipToken &t);

private:
    constexpr static int MAX_UPGRADE = 3;

    bool    m_hasOwner = false;
    Player* m_owner = nullptr;
    QRectF  m_thumbnailRegion;

    int m_buyingCost;
    int m_basicIncome;
    int m_upgradeLevel = 0;
    QVector<int> m_upgradeCost;
    QVector<int> m_upgradeIncome;
};

#endif // OWNERSHIPTOKEN_H
