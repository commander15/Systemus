#include "privilege.h"
#include "privilege_p.h"

#include <QtCore/qjsonarray.h>

namespace Systemus {

Privilege::Privilege() :
    AuthorizationData(new PrivilegePrivate)
{
    init<Privilege>();
}

Privilege::Privilege(const Privilege &other) :
    AuthorizationData(other)
{
    init<Privilege>();
}

Privilege::~Privilege()
{
}

bool Privilege::hasPermission(const QString &name) const
{
    return isPermissionActive(name);
}

bool Privilege::isPermissionActive(const QString &name) const
{
    S_D(const Privilege);

    for (int i(0); i < d->permissions.size(); ++i) {
        const Permission permission = d->permissions.at(i);
        if (permission.name() == name)
            return d->permissions.junctionData(i).value("active").toBool();
    }
    return false;
}

QList<Permission> Privilege::permissions() const
{
    S_D(const Privilege);
    return d->permissions;
}

bool Privilege::getExtras()
{
    S_D(Privilege);
    return d->permissions.get(*this);
}

QJsonObject Privilege::toJsonObject() const
{
    S_D(const Privilege);

    QJsonObject object = AuthorizationData::toJsonObject();

    QJsonArray permissions;
    for (const Permission &permission : d->permissions) {
        QJsonObject permissionObject = permission.toJsonObject();
        permissionObject.insert("active", d->permissions.junctionData(0).value("active").toBool());
        permissions.append(permissionObject);
    }
    object.insert("permissions", permissions);

    return object;
}

PrivilegedData::PrivilegedData(AuthorizationDataPrivate *data) :
    AuthorizationData(data)
{
}

PrivilegedData::~PrivilegedData()
{
}

bool PrivilegedData::hasPrivilege(const QString &name) const
{
    S_D(const PrivilegedDataInterface);
    return d->hasPrivilege(name);
}

bool PrivilegedData::isPrivilegeActive(const QString &name) const
{
    S_D(const PrivilegedDataInterface);
    return d->isPrivilegeActive(name);
}

QList<Privilege> PrivilegedData::privileges() const
{
    S_D(const PrivilegedDataInterface);
    return d->privileges();
}

bool PrivilegedData::hasPermission(const QString &name) const
{
    S_D(const PrivilegedDataInterface);
    return d->hasPermission(name);
}

bool PrivilegedData::isPermissionActive(const QString &name) const
{
    S_D(const PrivilegedDataInterface);
    return d->isPermissionActive(name);
}

QList<Permission> PrivilegedData::permissions() const
{
    S_D(const PrivilegedDataInterface);
    return d->permissions();
}

bool PrivilegedData::getExtras()
{
    S_D(PrivilegedDataInterface);
    return d->get(*this);
}

}
