#ifndef SYSTEMUS_AUTHORIZATIONDATA_P_H
#define SYSTEMUS_AUTHORIZATIONDATA_P_H

#include <SystemusCore/authorizationdata.h>
#include <SystemusCore/private/data_p.h>

namespace Systemus {

class AuthorizationDataPrivate : public DefaultDataPrivate
{
public:
    AuthorizationDataPrivate() = default;
    AuthorizationDataPrivate(const AuthorizationDataPrivate &other) = default;

    virtual bool equals(const DataPrivate *o) const override
    {
        const AuthorizationDataPrivate *other = static_cast<const AuthorizationDataPrivate *>(o);

        return name == other->name
               && description == other->description
               && creationDate == other->creationDate
               && creationTime == other->creationTime
               && DefaultDataPrivate::equals(o);
    }

    virtual void clear() override
    {
        name.clear();
        description.clear();
        creationDate = QDate();
        creationTime = QTime();
        DefaultDataPrivate::clear();
    }

    inline AuthorizationDataPrivate *clone() const override
    { return new AuthorizationDataPrivate(*this); }

    inline int dataType() const override
    { return AuthorizationDataType; }

    QString name;
    QString description;

    QDate creationDate;
    QTime creationTime;
};

}

#endif // SYSTEMUS_AUTHORIZATIONDATA_P_H
