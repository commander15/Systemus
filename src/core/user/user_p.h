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

    bool equalsTo(const DataPrivate *o) const override;

    void clear() override;

    QString name;
};

class UserPrivate : public PrivilegedDataPrivate
{
public:
    UserPrivate();
    UserPrivate(const UserPrivate &other) = default;

    static QString encryptPassword(const QString &password);

    bool isPrivilegeActive(const QString &name) const override;


    bool isPermissionActive(const QString &name) const override;

    bool equalsTo(const DataPrivate *o) const override;

    void clear() override;

    QString password;
    bool active = false;

    OneToOneRelation<UserProfile> profile;
    OneToOneRelation<Role> role;
};

}

#endif // SYSTEMUS_USER_P_H
