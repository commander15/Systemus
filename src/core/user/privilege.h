#ifndef SYSTEMUS_PRIVILEGE_H
#define SYSTEMUS_PRIVILEGE_H

#include <SystemusCore/global.h>
#include <SystemusCore/authorizationdata.h>
#include <SystemusCore/permission.h>


namespace Systemus {

class PrivilegePrivate;
class SYSTEMUS_CORE_EXPORT Privilege : public AuthorizationData
{
    Q_GADGET

public:
    Privilege();
    Privilege(const Privilege &other);
    ~Privilege();

    bool hasPermission(const QString &name) const;
    bool isPermissionActive(const QString &name) const;
    QList<Permission> permissions() const;

    bool getExtras() override;

    QJsonObject toJsonObject() const override;

private:
    S_DATA(Privilege)
};

class PrivilegedDataPrivate;
class SYSTEMUS_CORE_EXPORT PrivilegedData : public AuthorizationData
{
    Q_GADGET

public:
    PrivilegedData();
    //PrivilegedData(const PrivilegedData &other);
    virtual ~PrivilegedData();

    bool hasPrivilege(const QString &name) const;
    bool isPrivilegeActive(const QString &name) const;
    QList<Privilege> privileges() const;

    bool hasPermission(const QString &name) const;
    bool isPermissionActive(const QString &name) const;
    QList<Permission> permissions() const;

    bool getExtras() override;

    QJsonObject toJsonObject() const override;

protected:
    PrivilegedData(PrivilegedDataPrivate *data);
};

}

#endif // SYSTEMUS_PRIVILEGE_H
