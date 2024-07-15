#ifndef SYSTEMUS_USER_H
#define SYSTEMUS_USER_H

#include <SystemusCore/global.h>
#include <SystemusCore/privilege.h>

namespace Systemus {

class UserProfile;
class Role;
class Group;

class SYSTEMUS_CORE_EXPORT User : public PrivilegedData
{
    Q_GADGET
    Q_PROPERTY(QString password READ password WRITE setPassword)
    Q_PROPERTY(bool active READ isActive WRITE setActive)
    Q_PROPERTY(UserProfile profile READ profile STORED false)
    Q_CLASSINFO("fields", "profile_id(int), role_id(int)")

public:
    User();
    User(const User &other);

    User &operator=(const User &other);

    QString password() const;
    void setPassword(const QString &password, bool encrypt = false);

    bool isActive() const;
    void setActive(bool active = true);

    UserProfile profile() const;

    bool hasRole(const QString &name) const;
    Role role() const;

    bool inGroup(const QString &name) const;
    QList<Group> groups() const;

    bool getExtras() override;

private:
    S_DATA(User)
};

class SYSTEMUS_CORE_EXPORT UserProfile : public Data
{
    Q_GADGET
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString firstName READ firstName WRITE setFirstName)
    Q_PROPERTY(QString fullName READ fullName USER true STORED false)
    Q_CLASSINFO("table", "Profiles")

public:
    UserProfile();
    UserProfile(const UserProfile &other);

    UserProfile &operator=(const UserProfile &other);

    QString name() const;
    void setName(const QString &name);

    QString firstName() const;
    void setFirstName(const QString &name);

    QString fullName() const;

private:
    S_DATA(UserProfile)
};

}

Q_DECLARE_METATYPE(Systemus::User)
Q_DECLARE_METATYPE(Systemus::UserProfile)

#endif // SYSTEMUS_USER_H
