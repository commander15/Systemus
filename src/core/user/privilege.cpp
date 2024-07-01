#include "privilege.h"
#include "privilege_p.h"

#include <QtCore/qjsonarray.h>

namespace Systemus {

Privilege::Privilege() :
    AuthorizationData(new PrivilegePrivate)
{
}

Privilege::Privilege(const Privilege &other) :
    AuthorizationData(other)
{
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
    int index = d->permissionIndex(name);
    return (index >= 0 ? d->permissions.junctionData("active", index).toBool() : false);
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

QJsonObject Privilege::toJsonObject() const
{
    S_D(const Privilege);

    QJsonObject object = AuthorizationData::toJsonObject();

    QJsonArray permissions;
    for (const Permission &permission : d->permissions) {
        QJsonObject permissionObject;
        permissionObject.insert("permission", permission.toJsonObject());
        permissionObject.insert("active", d->permissions.junctionData(0).value("active").toBool());
        permissions.append(permissionObject);
    }
    object.insert("permissions", permissions);

    return object;
}

PrivilegedData::PrivilegedData(PrivilegedDataPrivate *data) :
    AuthorizationData(data)
{
}

PrivilegedData::PrivilegedData() :
    AuthorizationData(new PrivilegedDataPrivate(QString(), false))
{
}

PrivilegedData::~PrivilegedData()
{
}

bool PrivilegedData::hasPrivilege(const QString &name) const
{
    return isPrivilegeActive(name);
}

bool PrivilegedData::isPrivilegeActive(const QString &name) const
{
    S_D(const PrivilegedData);
    return d->isPrivilegeActive(name);
}

QList<Privilege> PrivilegedData::privileges() const
{
    S_D(const PrivilegedData);
    return d->privileges;
}

bool PrivilegedData::hasPermission(const QString &name) const
{
    return isPermissionActive(name);
}

bool PrivilegedData::isPermissionActive(const QString &name) const
{
    S_D(const PrivilegedData);
    return d->isPermissionActive(name);
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

QJsonObject PrivilegedData::toJsonObject() const
{
    S_D(const PrivilegedData);

    QJsonObject object = AuthorizationData::toJsonObject();

    QJsonArray privileges;
    for (int i(0); i < d->privileges.size(); ++i) {
        QJsonObject privilegeObject;
        privilegeObject.insert("privilege", d->privileges.at(i).toJsonObject());
        privilegeObject.insert("active", d->privileges.junctionData("active", i).toBool());
        privileges.append(privilegeObject);
    }
    object.insert("privileges", privileges);

    QJsonArray permissions;
    for (const Permission &permission : d->permissions) {
        QJsonObject permissionObject;
        permissionObject.insert("permission", permission.toJsonObject());
        permissionObject.insert("active", d->permissions.junctionData(0).value("active").toBool());
        permissions.append(permissionObject);
    }
    object.insert("permissions", permissions);

    return object;
}

PrivilegePrivate::PrivilegePrivate() :
    permissions("PrivilegePermissions", { "active", "issue_date", "issue_time" })
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
    privileges(context + "Privileges", (withJunctionData ? QStringList() << "active" << "issue_date" << "issue_time" : QStringList())),
    permissions(context + "Permissions", (withJunctionData ? QStringList() << "active" << "issue_date" << "issue_time" : QStringList()))
{
}

bool PrivilegedDataPrivate::isPrivilegeActive(const QString &name) const
{
    int index = privilegeIndex(name);
    return (index >= 0 ? privileges.junctionData("active", index).toBool() : false);
}

int PrivilegedDataPrivate::privilegeIndex(const QString &name) const
{
    for (int i(0); i < privileges.size(); ++i)
        if (privileges.at(i).name() == name)
            return i;
    return -1;
}

bool PrivilegedDataPrivate::isPermissionActive(const QString &name) const
{
    int index = permissionIndex(name);
    return (index >= 0 ? permissions.junctionData("active", index).toBool() : false);
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
