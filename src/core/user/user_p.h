#ifndef SYSTEMUS_USER_P_H
#define SYSTEMUS_USER_P_H

#include <SystemusCore/user.h>
#include <SystemusCore/role.h>
#include <SystemusCore/group.h>

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
    QString firstName;
};

class UserPrivate : public PrivilegedDataPrivate
{
public:
    UserPrivate();
    UserPrivate(const UserPrivate &other) = default;

    static QString encryptPassword(const QString &password);

    bool hasPrivilege(const QString &name) const override;

    bool hasPermission(const QString &name) const override;

    QVariant property(const QString &name) const;
    void setProperty(const QString &name, const QVariant &value);

    bool equalsTo(const DataPrivate *o) const override;

    void clear() override;

    int dataType() const override
    { return UserDataType; }

    QString password;
    bool active = false;

    OneToOneRelation<UserProfile> profile;
    OneToOneRelation<Role> role;
    ManyToManyRelation<Group> groups;
};

}

#endif // SYSTEMUS_USER_P_H
