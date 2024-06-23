#ifndef SYSTEMUS_USER_H
#define SYSTEMUS_USER_H

#include <SystemusCore/global.h>
#include <SystemusCore/privilege.h>

namespace Systemus {

class Role;

class SYSTEMUS_CORE_EXPORT UserProfile : public Data
{
    Q_GADGET
    Q_PROPERTY(QString name READ name WRITE setName USER true)
    Q_CLASSINFO("table", "Profiles")

public:
    UserProfile();
    UserProfile(const UserProfile &other);

    QString name() const;
    void setName(const QString &name);
};

class SYSTEMUS_CORE_EXPORT User : public PrivilegedData
{
    Q_GADGET
    Q_PROPERTY(QString password READ password WRITE setPassword)
    Q_PROPERTY(bool active READ isActive WRITE setActive)
    Q_CLASSINFO("fields", "profile_id, role_id")

public:
    User();
    User(const User &other);

    QString password() const;
    void setPassword(const QString &password, bool encrypt = false);

    bool isActive() const;
    void setActive(bool active = true);

    QString profileName() const;
    UserProfile profile() const;

    bool hasRole(const QString &name) const;
    Role role() const;

    bool insert() override;
};

}

#endif // SYSTEMUS_USER_H
