#include "description.h"

Description::Description(int index, const QString &name, const QString &description, const QString &imagePath, const QString &buyingCost, const QString &basicIncome, const QString &upgradeCost, const QString &upgradeLevel, const QString &upgradeIncome)
{
    m_objectType = ObjectType::OWNERSHIP_TOKEN;

    m_index = index;
    m_name = name;
    m_description = description;
    m_imagePath = imagePath;
    m_buyingCost = buyingCost;
    m_basicIncome = basicIncome;
    m_upgradeCost = upgradeCost;
    m_upgradeIncome = upgradeIncome;
    m_upgradeLevel = upgradeLevel;
}

Description::Description(int index, const ObjectType& object, const QString &type, const QString &name, const QString &description, const QString &imagePath)
{   
    Q_ASSERT_X((object == ObjectType::ACTION_TOKEN || object == ObjectType::CARD), "Description::Description", "This constructor is for action tokens and cards.");

    m_objectType = object;

    m_index = index;
    m_type = type;
    m_name = name;
    m_description = description;
    m_imagePath = imagePath;
}

QVector<int> Description::arrayStringToIntegerVector(const QString &string)
{
    // The method is used to convert strings like "%d,%d,%d" into vector of corresponding integers.
    // NOTE: Return values can be zeroes, if .toInt() method could not parse the string.

    QStringList sl = string.split(',');

    QVector<int> iv;
    for (int i = 0; i < sl.count(); ++i)
        iv.append(sl.at(i).toInt());

    return iv;
}
