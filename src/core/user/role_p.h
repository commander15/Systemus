#ifndef SYSTEMUS_ROLE_P_H
#define SYSTEMUS_ROLE_P_H

#include <SystemusCore/role.h>
#include <SystemusCore/private/privilege_p.h>

namespace Systemus {

class RolePrivate : public PrivilegedDataPrivate
{
public:
    RolePrivate();
    RolePrivate(const RolePrivate &other) = default;

    RolePrivate *clone() const override
    { return new RolePrivate(*this); }

    int dataType() const override
    { return RoleDataType; }
};

}

#endif // SYSTEMUS_ROLE_P_H
