#include "privilege.h"
#include "privilege_p.h"

#include <SystemusCore/system.h>

#include <QtCore/qjsonarray.h>

namespace Systemus {

Privilege::Privilege() :
    AuthorizationData(new PrivilegePrivate)
{
    init();
}

Privilege::Privilege(const Privilege &other) :
    AuthorizationData(other, false)
{
}

Privilege::~Privilege()
{
}

Privilege &Privilege::operator=(const Privilege &other)
{
    AuthorizationData::operator=(other);
    return *this;
}

bool Privilege::hasPermission(const QString &name) const
{
    S_D(const Privilege);
    return d->permissionIndex(name) >= 0;
}

QList<Permission> Privilege::permissions() const
{
    S_D(const Privilege);
    return d->permissions;
}

bool Privilege::getExtras(ExtraType type)
{
    S_D(Privilege);
    if (type == PostExtra)
        return Data::getExtras(type) && d->permissions.get(this);
    else
        return Data::getExtras(type);
}

bool Privilege::insertExtras(ExtraType type)
{
    S_D(Privilege);
    if (type == PostExtra) {
        return AuthorizationData::insertExtras(type) && d->permissions.insert(this);
    }

    return AuthorizationData::insertExtras(type);
}

PrivilegedData::PrivilegedData() :
    AuthorizationData(new PrivilegedDataPrivate(QString()))
{
}

PrivilegedData::PrivilegedData(const PrivilegedData &other) :
    AuthorizationData(other, false)
{
}

PrivilegedData::PrivilegedData(PrivilegedDataPrivate *data) :
    AuthorizationData(data)
{
}

PrivilegedData::PrivilegedData(const PrivilegedData &other, bool adapt) :
    AuthorizationData(other, adapt)
{
}

PrivilegedData::~PrivilegedData()
{
}

PrivilegedData &PrivilegedData::operator=(const PrivilegedData &other)
{
    AuthorizationData::operator=(other);
    return *this;
}

bool PrivilegedData::hasPrivilege(const QString &name) const
{
    S_D(const PrivilegedData);
    return d->hasPrivilege(name);
}

QList<Privilege> PrivilegedData::privileges() const
{
    S_D(const PrivilegedData);
    return d->privileges;
}

bool PrivilegedData::hasPermission(const QString &name) const
{
    S_D(const PrivilegedData);
    return d->hasPermission(name);
}

QList<Permission> PrivilegedData::permissions() const
{
    S_D(const PrivilegedData);
    return d->permissions;
}

bool PrivilegedData::getExtras(ExtraType type)
{
    S_D(PrivilegedData);
    if (type == PostExtra)
        return d->privileges.get(this) && d->permissions.get(this);
    else
        return AuthorizationData::getExtras(type);
}

bool PrivilegedData::insertExtras(ExtraType type)
{
    S_D(PrivilegedData);
    if (type == PostExtra) {
        return AuthorizationData::insertExtras(type) && d->privileges.insert(this) && d->permissions.insert(this);
    }

    return AuthorizationData::insertExtras(type);
}

bool PrivilegedData::updateExtras(ExtraType type)
{
    S_D(PrivilegedData);
    if (type == PostExtra)
        return AuthorizationData::updateExtras(type) && d->privileges.update(this) && d->permissions.update(this);

    return AuthorizationData::updateExtras(type);
}

PrivilegePrivate::PrivilegePrivate() :
    permissions("PrivilegePermission")
{
}

int PrivilegePrivate::permissionIndex(const QString &name) const
{
    for (int i(0); i < permissions.size(); ++i)
        if (permissions.at(i).name() == name)
            return i;
    return -1;
}

bool PrivilegePrivate::equals(const DataPrivate *o) const
{
    const PrivilegePrivate *other = static_cast<const PrivilegePrivate *>(o);
    return permissions == other->permissions
           && AuthorizationDataPrivate::equals(o);
}

void PrivilegePrivate::clear()
{
    permissions.clear();
    permissions.clear();
    AuthorizationDataPrivate::clear();
}

PrivilegedDataPrivate::PrivilegedDataPrivate(const QString &context) :
    privileges(context + "Privilege"),
    permissions(context + "Permission")
{
}

bool PrivilegedDataPrivate::hasPrivilege(const QString &name) const
{
    return privilegeIndex(name) >= 0;
}

int PrivilegedDataPrivate::privilegeIndex(const QString &name) const
{
    for (int i(0); i < privileges.size(); ++i)
        if (privileges.at(i).name() == name)
            return i;
    return -1;
}

bool PrivilegedDataPrivate::hasPermission(const QString &name) const
{
    for (const Privilege &privilege : privileges)
        if (privilege.hasPermission(name))
            return true;

    for (const Permission &permission : permissions)
        if (permission.name() == name)
            return true;

    return false;
}

int PrivilegedDataPrivate::permissionIndex(const QString &name) const
{
    for (int i(0); i < permissions.size(); ++i)
        if (permissions.at(i).name() == name)
            return i;
    return -1;
}

bool PrivilegedDataPrivate::equals(const DataPrivate *o) const
{
    const PrivilegedDataPrivate *other = static_cast<const PrivilegedDataPrivate *>(o);
    return privileges == other->privileges
           && permissions == other->permissions
           && AuthorizationDataPrivate::equals(o);
}

void PrivilegedDataPrivate::clear()
{
    privileges.clear();
    permissions.clear();
    AuthorizationDataPrivate::clear();
}

}
