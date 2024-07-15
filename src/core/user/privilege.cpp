#include "privilege.h"
#include "privilege_p.h"

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

bool Privilege::getExtras()
{
    S_D(Privilege);
    return d->permissions.get(this);
}

PrivilegedData::PrivilegedData() :
    AuthorizationData(new PrivilegedDataPrivate(QString(), false))
{
}

PrivilegedData::PrivilegedData(const PrivilegedData &other) :
    AuthorizationData(other)
{
}

PrivilegedData::PrivilegedData(PrivilegedDataPrivate *data) :
    AuthorizationData(data)
{
}

PrivilegedData::PrivilegedData(const PrivilegedData &other, bool transferProperties) :
    AuthorizationData(other, transferProperties)
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

bool PrivilegedData::getExtras()
{
    S_D(PrivilegedData);
    return d->privileges.get(this) && d->permissions.get(this);
}

PrivilegePrivate::PrivilegePrivate() :
    permissions("PrivilegePermissions", { "issue_date", "issue_time" })
{
}

int PrivilegePrivate::permissionIndex(const QString &name) const
{
    for (int i(0); i < permissions.size(); ++i)
        if (permissions.at(i).name() == name)
            return i;
    return -1;
}

bool PrivilegePrivate::equalsTo(const DataPrivate *o) const
{
    const PrivilegePrivate *other = static_cast<const PrivilegePrivate *>(o);
    return permissions == other->permissions
           && AuthorizationDataPrivate::equalsTo(o);
}

void PrivilegePrivate::clear()
{
    permissions.clear();
    permissions.clear();
    AuthorizationDataPrivate::clear();
}

PrivilegedDataPrivate::PrivilegedDataPrivate(const QString &context, bool withJunctionData) :
    privileges(context + "Privileges", (withJunctionData ? QStringList() << "issue_date" << "issue_time" : QStringList())),
    permissions(context + "Permissions", (withJunctionData ? QStringList() << "issue_date" << "issue_time" : QStringList()))
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
    return permissionIndex(name) >= 0;
}

int PrivilegedDataPrivate::permissionIndex(const QString &name) const
{
    for (int i(0); i < permissions.size(); ++i)
        if (permissions.at(i).name() == name)
            return i;
    return -1;
}

bool PrivilegedDataPrivate::equalsTo(const DataPrivate *o) const
{
    const PrivilegedDataPrivate *other = static_cast<const PrivilegedDataPrivate *>(o);
    return privileges == other->privileges
           && permissions == other->permissions
           && AuthorizationDataPrivate::equalsTo(o);
}

void PrivilegedDataPrivate::clear()
{
    privileges.clear();
    permissions.clear();
    AuthorizationDataPrivate::clear();
}

}
