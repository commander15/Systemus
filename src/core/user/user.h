#ifndef SYSTEMUS_USER_H
#define SYSTEMUS_USER_H

#include <SystemusCore/global.h>
#include <SystemusCore/internaldata.h>

namespace Systemus {

class UserProfile;
class UserRole;
class UserGroup;

class SYSTEMUS_CORE_EXPORT User : public InternalData
{
    Q_GADGET
    Q_PROPERTY(QString login READ login WRITE setLogin)
    Q_PROPERTY(QString password READ password WRITE setPassword)
    Q_PROPERTY(int status READ status WRITE setStatus)
    Q_PROPERTY(UserProfile profile READ profile STORED false)
    Q_CLASSINFO("properties", "roleId(int, required)")
    S_DATA(User)

public:
    enum PasswordType {
        Auto,
        Clear,
        Hashed
    };

    enum Status {
        UnknownStatus,
        ActiveStatus,
        InactiveStatus
    };

    User();
    User(const User &other) = default;

    QString login() const;
    void setLogin(const QString &login);

    QString password() const;
    void setPassword(const QString &password, PasswordType type = Auto);

    int status() const;
    void setStatus(int status);

    UserProfile profile() const;
    void setProfile(const UserProfile &profile);

    bool hasRole(const QString &name) const;
    UserRole role() const;
    void setRole(const UserRole &role);

    //bool inGroup(const QString &name) const;
    //QList<UserGroup> groups() const;
    //void setGroups(const QList<UserGroup> &groups);

    bool postGet() override;
    bool postUpdate() override;

    static User fromLogin(const QString &login);
};

class SYSTEMUS_CORE_EXPORT UserProfile : public InternalData
{
    Q_GADGET
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString firstName READ firstName WRITE setFirstName)
    Q_PROPERTY(QString fullName READ fullName USER true STORED false)
    Q_CLASSINFO("properties", "userId(int, required)")
    S_DATA(UserProfile)

public:
    UserProfile();
    UserProfile(const UserProfile &other) = default;

    QString name() const;
    void setName(const QString &name);

    QString firstName() const;
    void setFirstName(const QString &name);

    QString fullName() const;
};

class SYSTEMUS_CORE_EXPORT UserRole : public DescriptiveData
{
    Q_GADGET
    S_DATA(UserRole)
};

class SYSTEMUS_CORE_EXPORT UserGroup : public DescriptiveData
{
    Q_GADGET
    S_DATA(UserGroup)
};

}

Q_DECLARE_METATYPE(Systemus::User)
Q_DECLARE_METATYPE(Systemus::UserProfile)
Q_DECLARE_METATYPE(Systemus::UserRole)
Q_DECLARE_METATYPE(Systemus::UserGroup)

#endif // SYSTEMUS_USER_H
