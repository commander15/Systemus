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
    Q_CLASSINFO("table", "SUsers")

public:
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
    void setPassword(const QString &password, bool encrypt = false);

    int status() const;
    void setStatus(int status);

    UserProfile profile() const;
    void setProfile(const UserProfile &profile);

    bool hasRole(const QString &name) const;
    UserRole role() const;
    void setRole(const UserRole &role);

    bool postGet() override;

    static User fromLogin(const QString &login);

private:
    S_DATA(User)
};

class SYSTEMUS_CORE_EXPORT UserProfile : public InternalData
{
    Q_GADGET
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString firstName READ firstName WRITE setFirstName)
    Q_PROPERTY(QString fullName READ fullName USER true STORED false)
    Q_CLASSINFO("properties", "userId(int, required)")
    Q_CLASSINFO("table", "SUserProfiles")

public:
    UserProfile();
    UserProfile(const UserProfile &other) = default;

    QString name() const;
    void setName(const QString &name);

    QString firstName() const;
    void setFirstName(const QString &name);

    QString fullName() const;

private:
    S_DATA(UserProfile)
};

class DescriptiveUserDataPrivate;
class SYSTEMUS_CORE_EXPORT DescriptiveUserData : public InternalData
{
    Q_GADGET
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString description READ description WRITE setDescription)

public:
    DescriptiveUserData();
    DescriptiveUserData(const DescriptiveUserData &other) = default;

    QString name() const;
    void setName(const QString &name);

    QString description() const;
    void setDescription(const QString &desc);
};

class SYSTEMUS_CORE_EXPORT UserRole : public DescriptiveUserData
{
    Q_GADGET
    Q_CLASSINFO("table", "SUserRoles")

private:
    S_DATA(UserRole)
};

}

Q_DECLARE_METATYPE(Systemus::User)
Q_DECLARE_METATYPE(Systemus::UserProfile)
Q_DECLARE_METATYPE(Systemus::UserRole)

#endif // SYSTEMUS_USER_H
