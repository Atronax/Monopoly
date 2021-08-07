#ifndef ACTIONTOKEN_H
#define ACTIONTOKEN_H

#include "token.h"
#include "helper/description.h"

class ActionToken : public Token
{
public:
    enum class ActionType {START, PORTAL, PRISON, EXCHANGE, MOVE_FORWARD, MOVE_BACKWARD, CARD_POSITIVE, CARD_NEGATIVE};

    ActionToken(ActionType actionType, const QString& name, const QString& description, const QString& imagePath);
    ActionToken(Description* atd);
    virtual ~ActionToken();

    void setActionType(const ActionType& actionType);    
    const ActionType& actionType() const;

    QString    typeToString () const;
    ActionType stringToType (const QString& name) const;

    // friend QDataStream& operator<<(QDataStream &out, const ActionToken &t);
    // friend QDataStream& operator>>(QDataStream &in,        ActionToken &t);

private:
    // Remark. Methods should be const to call them from friendly functions.
    int m_index;

    ActionType m_type;
};

#endif // ACTIONTOKEN_H
