#ifndef SYSTEMUS_PRIVILEGE_P_H
#define SYSTEMUS_PRIVILEGE_P_H

#include <SystemusCore/privilege.h>
#include <SystemusCore/private/authorizationdata_p.h>
#include <SystemusCore/datarelation.h>

namespace Systemus {

class PrivilegePrivate : public AuthorizationDataPrivate
{
public:
    PrivilegePrivate();
    PrivilegePrivate(const PrivilegePrivate &other) = default;

    int permissionIndex(const QString &name) const;

    bool equalsTo(const DataPrivate *o) const override;

    void clear() override;

    int dataType() const override
    { return PrivilegeDataType; }

    ManyToManyRelation<Permission> permissions;
};

class PrivilegedDataPrivate : public AuthorizationDataPrivate
{
public:
    PrivilegedDataPrivate(const QString &context, bool withJunctionData = true);
    PrivilegedDataPrivate(const PrivilegedDataPrivate &other) = default;

    virtual bool hasPrivilege(const QString &name) const;
    int privilegeIndex(const QString &name) const;

    virtual bool hasPermission(const QString &name) const;
    int permissionIndex(const QString &name) const;

    bool equalsTo(const DataPrivate *o) const override;

    void clear() override;

    ManyToManyRelation<Privilege> privileges;
    ManyToManyRelation<Permission> permissions;
};

}

#endif // SYSTEMUS_PRIVILEGE_P_H
