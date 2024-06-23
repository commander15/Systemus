#ifndef SYSTEMUS_USER_P_H
#define SYSTEMUS_USER_P_H

#include <SystemusCore/user.h>
#include <SystemusCore/role.h>

#include <SystemusCore/private/privilege_p.h>

namespace Systemus {

class UserProfilePrivate : public DataPrivate
{
public:
    UserProfilePrivate() = default;
    UserProfilePrivate(const UserProfilePrivate &other) = default;

    bool equalsTo(const DataPrivate *o) const override
    {
        const UserProfilePrivate *other = static_cast<const UserProfilePrivate *>(o);
        return name == other->name
               && DataPrivate::equalsTo(o);
    }

    void clear() override
    {
        name.clear();
        DataPrivate::clear();
    }

    QString name;
};

class UserPrivate : public PrivilegedDataPrivate<User>
{
public:
    UserPrivate() = default;
    UserPrivate(const UserPrivate &other) = default;

    static QString encryptPassword(const QString &password);

    bool hasPrivilege(const QString &name) const override
    {
        return role.object().hasPrivilege(name)
               || PrivilegedDataPrivate<User>::hasPrivilege(name);
    }

    bool isPrivilegeActive(const QString &name) const override
    {
        return role.object().isPrivilegeActive(name)
               || PrivilegedDataPrivate<User>::isPrivilegeActive(name);
    }

    QList<Privilege> privileges() const override
    { return role.object().privileges() + PrivilegedDataPrivate<User>::privileges(); }

    bool hasPermission(const QString &name) const override
    {
        return role.object().hasPermission(name)
               || PrivilegedDataPrivate<User>::hasPermission(name);
    }

    bool isPermissionActive(const QString &name) const override
    {
        return role.object().isPermissionActive(name)
               || PrivilegedDataPrivate<User>::isPermissionActive(name);
    }

    QList<Permission> permissions() const override
    { return role.object().permissions() + PrivilegedDataPrivate<User>::permissions(); }

    bool get(const Data &primaryData) override
    {
        const User &user = static_cast<const User &>(primaryData);
        return profile.get(user)
            && role.get(user)
            && PrivilegedDataPrivate<User>::get(primaryData);
    }

    bool equalsTo(const DataPrivate *o) const override
    {
        const UserPrivate *other = static_cast<const UserPrivate *>(o);
        return password == other->password
               && active == other->active
               && profile == other->profile
               && role == other->role
               && PrivilegedDataPrivate<User>::equalsTo(o);
    }

    void clear() override
    {
        password.clear();
        active = false;
        role.clear();
        PrivilegedDataPrivate<User>::clear();
    }

    QString password;
    bool active = false;

    OneToOneRelationship<User, UserProfile> profile;
    OneToOneRelationship<User, Role> role;
};

}

#endif // SYSTEMUS_USER_P_H
