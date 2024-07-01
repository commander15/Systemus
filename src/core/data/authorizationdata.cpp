#include "authorizationdata.h"
#include "authorizationdata_p.h"

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

AuthorizationData::~AuthorizationData()
{
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

bool AuthorizationData::isValid() const
{
    S_D(const AuthorizationData);
    return !d->name.isEmpty() && Data::isValid();
}

}
