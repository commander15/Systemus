#include "authorizationdata.h"
#include "authorizationdata_p.h"

#include <SystemusCore/system.h>

namespace Systemus {

AuthorizationData::AuthorizationData() :
    Data(new AuthorizationDataPrivate)
{
}

AuthorizationData::AuthorizationData(AuthorizationDataPrivate *data) :
    Data(data)
{
}

AuthorizationData::AuthorizationData(const AuthorizationData &other) :
    Data(other)
{
}

AuthorizationData::AuthorizationData(const AuthorizationData &other, bool adapt) :
    Data(other, adapt)
{
}

AuthorizationData::~AuthorizationData()
{
}

AuthorizationData &AuthorizationData::operator=(const AuthorizationData &other)
{
    Data::operator=(other);
    return *this;
}

QString AuthorizationData::name() const
{
    S_D(const AuthorizationData);
    return d->name;
}

void AuthorizationData::setName(const QString &name)
{
    S_D(AuthorizationData);
    d->name = name;
}

QString AuthorizationData::description() const
{
    S_D(const AuthorizationData);
    return d->description;
}

void AuthorizationData::setDescription(const QString &description)
{
    S_D(AuthorizationData);
    d->description = description;
}

QDate AuthorizationData::creationDate() const
{
    S_D(const AuthorizationData);
    return d->creationDate;
}

QTime AuthorizationData::creationTime() const
{
    S_D(const AuthorizationData);
    return d->creationTime;
}

bool AuthorizationData::saveReadOnlyProperty(const QString &name, const QVariant &value)
{
    S_D(AuthorizationData);

    if (name == "creationDate")
        d->creationDate = value.toDate();
    else if (name == "creationTime")
        d->creationTime = value.toTime();
    else
        return Data::saveReadOnlyProperty(name, value);

    return true;
}

bool AuthorizationData::isValid() const
{
    S_D(const AuthorizationData);
    return !d->name.isEmpty() && Data::isValid();
}

bool AuthorizationData::insertExtras(ExtraType type)
{
    if (type == PreExtra) {
        S_D(AuthorizationData);
        const QDateTime now = System::instance()->now();
        d->creationDate = now.date();
        d->creationTime = now.time();
    }

    return Data::insertExtras(type);
}

}
