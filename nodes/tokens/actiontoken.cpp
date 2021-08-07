#include "actiontoken.h"

#include <QDebug>

ActionToken::ActionToken(ActionToken::ActionType type, const QString &name, const QString &description, const QString &imagePath)
    : Token (name, description, "d:/monopoly/at/" + imagePath.trimmed())
{
    setActionType(type);
}

ActionToken::ActionToken(Description *atd)
{
    if (atd->objectType() != Description::ObjectType::ACTION_TOKEN)
        return;

    m_index = atd->index();

    setName(atd->name());
    setDescription(atd->description());
    setImage("d:/monopoly/at/" + atd->imagePath().trimmed());    
    setActionType(stringToType(atd->type()));
}

ActionToken::~ActionToken()
{

}

void ActionToken::setActionType(const ActionType &type)
{
    m_type = type;
}

QString ActionToken::typeToString() const
{
    // Fill the lists with relevant action types and their string representations.
    QList<ActionType> typesAT;
    typesAT << ActionType::START << ActionType::PORTAL << ActionType::PRISON << ActionType::EXCHANGE << ActionType::MOVE_FORWARD << ActionType::MOVE_BACKWARD << ActionType::CARD_POSITIVE << ActionType::CARD_NEGATIVE;

    QStringList typesSL;
    typesSL << "start" << "portal" << "prison" << "exchange" << "move_forward" << "move_backward" << "card_positive" << "card_negative";

    // Find index of parameter type in the list of available action types and return it.
    int index = -1;
    for (int i = 0; i < typesAT.count(); ++i)
    {
        if (m_type == typesAT.at(i))
        {
            index = i;
            break;
        }
    }

    return typesSL.at(index);
}

ActionToken::ActionType ActionToken::stringToType(const QString &name) const
{
    // 1. Fill types with string representations of enum values.
    QStringList typesSL;
    typesSL << "start" << "portal" << "prison" << "exchange" << "move_forward" << "move_backward" << "card_positive" << "card_negative";

    QList<ActionType> typesAT;
    typesAT << ActionType::START << ActionType::PORTAL << ActionType::PRISON << ActionType::EXCHANGE << ActionType::MOVE_FORWARD << ActionType::MOVE_BACKWARD << ActionType::CARD_POSITIVE << ActionType::CARD_NEGATIVE;

    // 2. Seek the list for the item with the same string representation as parameter. Return its index in the list.
    int index = -1;
    for (int i = 0; i < typesSL.count(); ++i)
    {
        if (name.trimmed() == typesSL.at(i))
        {
            index = i;
            break;
        }
    }

    qDebug() << "TypesSL: " << typesSL.count() << ". TypesAT: " << typesAT.count() << ". Index: " << index;

    // 3. Return the actual ActionType from the other list.
    Q_ASSERT_X(index != -1, "ActionToken::stringToType", "The parameter \"name\" is not one of the available action types");
    return typesAT.at(index);
}

const ActionToken::ActionType& ActionToken::actionType() const
{
    return m_type;
}

QDataStream& operator<<(QDataStream &out, const ActionToken &t)
{
    out << t.name() << t.description() << t.imagePath() << t.typeToString();

    return out;
}

QDataStream& operator>>(QDataStream &in, ActionToken &t)
{
    QString name;
    QString description;
    QString imagePath;
    QString actionTypeString;

    in >> name >> description >> imagePath >> actionTypeString;

    t.setName(name);
    t.setDescription(description);
    t.setImage(imagePath);
    t.setActionType(t.stringToType(actionTypeString));

    return in;
}
