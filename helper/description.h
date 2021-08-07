#ifndef DESCRIPTION_H
#define DESCRIPTION_H

#include <QString>
#include <QVector>

// Description is the helper class to store information of tokens and cards, loaded from the XML file.
// It has constructors to build instances holding everything needed to create any existing token or card.
// Usually constructor uses just some variables leaving the rest empty.

class Description
{
public:
    enum class ObjectType  {ACTION_TOKEN, OWNERSHIP_TOKEN, CARD, EMPTY};

    // ACTION_TOKEN or CARD depending of ObjectType
    Description(int index,
                const ObjectType& object, const QString& type,
                const QString& name,   const QString& description, const QString& imagePath);

    // OWNERSHIP_TOKEN
    Description(int index,
                const QString& name,        const QString& description, const QString& imagePath,
                const QString& buyingCost,  const QString& basicIncome,
                const QString& upgradeCost, const QString& upgradeLevel, const QString& upgradeIncome);

    ObjectType objectType() { return m_objectType; };

    int index() {return m_index; };
    const QString& name() {return m_name; };
    const QString& type() {return m_type; };
    const QString& description() {return m_description; };
    const QString& imagePath() {return m_imagePath; };
    const QString& buyingCost() {return m_buyingCost; };
    const QString& basicIncome() {return m_basicIncome; };
    const QString& upgradeCost() {return m_upgradeCost; };
    const QString& upgradeLevel() {return m_upgradeLevel; };
    const QString& upgradeIncome() {return m_upgradeIncome; };

    QVector<int> arrayStringToIntegerVector(const QString& string);    

private:
    ObjectType m_objectType = ObjectType::EMPTY;

    int m_index;
    QString m_type;
    QString m_name;
    QString m_description;
    QString m_imagePath;
    QString m_buyingCost;
    QString m_basicIncome;
    QString m_upgradeCost;
    QString m_upgradeLevel;
    QString m_upgradeIncome;
};

#endif // DESCRIPTION_H
