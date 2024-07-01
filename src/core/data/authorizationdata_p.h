#ifndef SYSTEMUS_AUTHORIZATIONDATA_P_H
#define SYSTEMUS_AUTHORIZATIONDATA_P_H

#include <SystemusCore/authorizationdata.h>
#include <SystemusCore/private/data_p.h>

namespace Systemus {

class AuthorizationDataPrivate : public DataPrivate
{
public:
    AuthorizationDataPrivate() = default;
    AuthorizationDataPrivate(const AuthorizationDataPrivate &other) = default;

    virtual bool equalsTo(const DataPrivate *o) const override
    {
        const AuthorizationDataPrivate *other = static_cast<const AuthorizationDataPrivate *>(o);

        return name == other->name
               && description == other->description
               && creationDate == other->creationDate
               && creationTime == other->creationTime
               && DataPrivate::equalsTo(o);
    }

    virtual void clear() override
    {
        name.clear();
        description.clear();
        creationDate = QDate();
        creationTime = QTime();
        DataPrivate::clear();
    }

    inline DataType dataType() const override
    { return AccessControlDataType; }

    QString name;
    QString description;

    QDate creationDate;
    QTime creationTime;
};

}

#endif // SYSTEMUS_AUTHORIZATIONDATA_P_H
