#ifndef SYSTEMUS_USER_P_H
#define SYSTEMUS_USER_P_H

#include <SystemusCore/user.h>
#include <SystemusCore/role.h>
#include <SystemusCore/group.h>

#include <SystemusCore/private/privilege_p.h>

namespace Systemus {

class UserProfilePrivate : public DefaultDataPrivate
{
public:
    UserProfilePrivate() = default;
    UserProfilePrivate(const UserProfilePrivate &other) = default;

    bool equals(const DataPrivate *o) const override;

    void clear() override;

    UserProfilePrivate *clone() const override;

    QString name;
    QString firstName;
};

class UserPrivate : public PrivilegedDataPrivate
{
public:
    UserPrivate();
    UserPrivate(const UserPrivate &other) = default;

    static bool checkPassword(const QString &input, const QString &password);
    static QString encryptPassword(const QString &password);

    bool hasPrivilege(const QString &name) const override;

    bool hasPermission(const QString &name) const override;

    bool equals(const DataPrivate *o) const override;

    void clear() override;

    UserPrivate *clone() const override;

    int dataType() const override
    { return UserDataType; }

    QString password;
    bool active = false;

    QDate lastLogDate;
    QTime lastLogTime;

    OneToOneRelation<UserProfile> profile;
    ManyToOneRelation<Role> role;
    ManyToManyRelation<Group> groups;
};

}

#endif // SYSTEMUS_USER_P_H
