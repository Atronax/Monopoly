#include "ownershiptoken.h"

#include <QDebug>
#include "player/player.h"

OwnershipToken::OwnershipToken(const QString &name, const QString &description, const QString &imagePath)
    : Token(name, description, "d:/monopoly/ot/" + imagePath.trimmed())
{
}

OwnershipToken::OwnershipToken(Description* otd)
{
    if (otd->objectType() != Description::ObjectType::OWNERSHIP_TOKEN)
    {
        qDebug() << "Token description hasn't any connections with ownership tokens!";
        return;
    }

    setName(otd->name());
    setDescription(otd->description());
    setImage("d:/monopoly/ot/" + otd->imagePath().trimmed());
    setBuyingCost(otd->buyingCost().toInt());
    setBasicIncome(otd->basicIncome().toInt());
    setUpgradeLevel(otd->upgradeLevel().toInt());
    setUpgradeCost(otd->arrayStringToIntegerVector(otd->upgradeCost()));
    setUpgradeIncome(otd->arrayStringToIntegerVector(otd->upgradeIncome()));

    qDebug() << "image is null: " << image().isNull();
    qDebug() << "OT created using otd";
}

OwnershipToken::~OwnershipToken()
{

}

void OwnershipToken::setBuyingCost(int buyingCost)
{
    if (buyingCost > 0)
        m_buyingCost = buyingCost;
}

void OwnershipToken::setBasicIncome(int basicIncome)
{
    if (basicIncome > 0)
        m_basicIncome = basicIncome;
}

void OwnershipToken::setUpgradeLevel(int level)
{
    if (level >= 1 && level <= MAX_UPGRADE)
        m_upgradeLevel = level;
}

void OwnershipToken::setUpgradeCost(const QVector<int>& upgradeCost)
{
    m_upgradeCost = upgradeCost;
}

void OwnershipToken::setUpgradeIncome(const QVector<int>& upgradeIncome)
{
    m_upgradeIncome = upgradeIncome;
}

bool OwnershipToken::hasOwner() const
{
    return m_hasOwner;
}

Player *OwnershipToken::owner() const
{
    return m_owner;
}

const QRectF &OwnershipToken::thumbnailRegion() const
{
    return m_thumbnailRegion;
}

int OwnershipToken::buyingCost() const
{
    return m_buyingCost;
}

int OwnershipToken::basicIncome() const
{
    return m_basicIncome;
}

int OwnershipToken::upgradeLevel() const
{
    return m_upgradeLevel;
}

int OwnershipToken::cUpgradeCost() const
{
    return (m_upgradeLevel == 0) ? m_upgradeCost.at(0) : m_upgradeCost.at(m_upgradeLevel - 1);
}

const QVector<int>& OwnershipToken::upgradeCost() const
{
    return m_upgradeCost;
}

const QVector<int>& OwnershipToken::upgradeIncome() const
{
    return m_upgradeIncome;
}

void OwnershipToken::activate()
{

}

void OwnershipToken::setOwner(Player *player)
{
    m_hasOwner = true;
    m_owner = player;
}

void OwnershipToken::setThumbnailRegion(const QRectF &ownerLocation)
{
    m_thumbnailRegion = ownerLocation;
}

void OwnershipToken::upgrade(bool bonus)
{
    qDebug() << "in upgrade";
    if (m_upgradeLevel == MAX_UPGRADE)
    {
        qDebug() << "This company has been upgraded to maximum level already";
        return;
    }

    if (!bonus)
    {
        if (m_owner->hand()->gold() >= m_upgradeCost[m_upgradeLevel])
        {
            qDebug() << m_owner->name() << " is able to make this deal.";
            m_owner->hand()->pay(m_upgradeCost[m_upgradeLevel]);
        }
        else
        {
            qDebug() << m_owner->name() << " has not enough money to make this deal.";
            return;
        }
    }

    qDebug() << "Setting level " << m_upgradeLevel + 1;
    setUpgradeLevel(++m_upgradeLevel);

    qDebug() << "This OT has been upgraded.";
}

int OwnershipToken::income()
{
    int upgradeBonus = 0;
    for (int i = 0; i < m_upgradeLevel; ++i)
        upgradeBonus += m_upgradeIncome[i];

    return m_basicIncome + upgradeBonus;
}


QDataStream& operator<<(QDataStream &out, const OwnershipToken &t)
{
    out << t.name() << t.description() << t.imagePath() << t.buyingCost() << t.basicIncome() << t.upgradeLevel() << t.upgradeCost() << t.upgradeIncome();

    return out;
}

QDataStream& operator>>(QDataStream &in, OwnershipToken &t)
{
    QString name;
    QString description;
    QString imagePath;
    int buyingCost;
    int basicIncome;
    int upgradeLevel;
    QVector<int> upgradeCost;
    QVector<int> upgradeIncome;

    in >> name >> description >> imagePath >> buyingCost >> basicIncome >> upgradeLevel >> upgradeCost >> upgradeIncome;

    t.setName(name);
    t.setDescription(description);
    t.setImage(imagePath);
    t.setBuyingCost(buyingCost);
    t.setBasicIncome(basicIncome);
    t.setUpgradeLevel(upgradeLevel);
    t.setUpgradeCost(upgradeCost);
    t.setUpgradeIncome(upgradeIncome);

    return in;
}
