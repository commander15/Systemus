#ifndef SYSTEMUS_PRIVILEGE_P_H
#define SYSTEMUS_PRIVILEGE_P_H

#include <SystemusCore/private/data_p.h>

#include <SystemusCore/privilege.h>

namespace Systemus {

class PrivilegePrivate : public AuthorizationDataPrivate
{
public:
    PrivilegePrivate() = default;
    PrivilegePrivate(const PrivilegePrivate &other) = default;

    bool equalsTo(const DataPrivate *o) const override
    {
        const PrivilegePrivate *other = static_cast<const PrivilegePrivate *>(o);
        return permissions == other->permissions
               && AuthorizationDataPrivate::equalsTo(o);
    }

    void clear() override
    {
        permissions.clear();
        AuthorizationDataPrivate::clear();
    }

    ManyToManyRelationship<Privilege, Permission> permissions = ManyToManyRelationship<Privilege, Permission>({ "active", "issue_date", "issue_time" });
};

class PrivilegedDataInterfacePrivate : public AuthorizationDataPrivate
{
public:
    virtual bool hasPrivilege(const QString &name) const = 0;
    virtual bool isPrivilegeActive(const QString &name) const = 0;
    //virtual QStringList privilegeNames() const = 0;
    //virtual Privilege privilege(const QString &name) const = 0;
    virtual QList<Privilege> privileges() const = 0;

    virtual bool hasPermission(const QString &name) const = 0;
    virtual bool isPermissionActive(const QString &name) const = 0;
    //virtual QStringList permissionNames() const = 0;
    //virtual Permission permission(const QString &name) const = 0;
    virtual QList<Permission> permissions() const = 0;

    virtual bool get(const Data &primaryData) = 0;
};

template<typename T>
class PrivilegedDataPrivate : public PrivilegedDataInterfacePrivate
{
public:
    PrivilegedDataPrivate() = default;
    PrivilegedDataPrivate(const PrivilegedDataPrivate &o) = default;
    ~PrivilegedDataPrivate() = default;

    virtual bool hasPrivilege(const QString &name) const override
    { return isPrivilegeActive(name); }

    virtual bool isPrivilegeActive(const QString &name) const override
    {
        for (int i(0); i < _privileges.size(); ++i)
            if (_privileges.at(i).name() == name)
                return _privileges.junctionData(i).value("active").toBool();
        return false;
    }

    virtual QList<Privilege> privileges() const override
    { return _privileges; }

    virtual bool hasPermission(const QString &name) const override
    { return isPermissionActive(name); }

    virtual bool isPermissionActive(const QString &name) const override
    {
        for (int i(0); i < _permissions.size(); ++i)
            if (_permissions.at(i).name() == name)
                return _permissions.junctionData(i).value("active").toBool();

        for (int i(0); i < _privileges.size(); ++i) {
            const Privilege privilege = _privileges.at(i);
            if (privilege.hasPermission(name))
                return privilege.isPermissionActive(name);
        }

        return false;
    }

    virtual QList<Permission> permissions() const override
    { return _permissions; }

    bool get(const Data &primaryData) override
    {
        T data = static_cast<const T &>(primaryData);
        return _privileges.get(data)
            && _permissions.get(data);
    }

    virtual bool equalsTo(const DataPrivate *o) const override
    {
        const PrivilegedDataPrivate<T> *other = static_cast<const PrivilegedDataPrivate<T> *>(o);
        return _privileges == other->_privileges
            && _permissions == other->_permissions
            && AuthorizationDataPrivate::equalsTo(o);
    }

    virtual void clear() override
    {
        _privileges.clear();
        _permissions.clear();
        AuthorizationDataPrivate::clear();
    }

    ManyToManyRelationship<T, Privilege> _privileges = ManyToManyRelationship<T, Privilege>({ "active", "issue_date", "issue_time" });
    ManyToManyRelationship<T, Permission> _permissions = ManyToManyRelationship<T, Permission>({ "active", "issue_date", "issue_time" });
};

}

#endif // SYSTEMUS_PRIVILEGE_P_H
