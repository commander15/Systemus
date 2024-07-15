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

    Privilege &operator=(const Privilege &other);

    bool hasPermission(const QString &name) const;
    QList<Permission> permissions() const;

    bool getExtras() override;

private:
    S_DATA(Privilege)
};

class PrivilegedDataPrivate;
class SYSTEMUS_CORE_EXPORT PrivilegedData : public AuthorizationData
{
    Q_GADGET

public:
    PrivilegedData();
    PrivilegedData(const PrivilegedData &other);
    PrivilegedData(const PrivilegedData &other, bool transferProperties);
    virtual ~PrivilegedData();

    PrivilegedData &operator=(const PrivilegedData &other);

    bool hasPrivilege(const QString &name) const;
    QList<Privilege> privileges() const;

    bool hasPermission(const QString &name) const;
    QList<Permission> permissions() const;

    bool getExtras() override;

protected:
    PrivilegedData(PrivilegedDataPrivate *data);
};

}

Q_DECLARE_METATYPE(Systemus::Privilege)

#endif // SYSTEMUS_PRIVILEGE_H
