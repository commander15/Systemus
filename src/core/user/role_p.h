#ifndef SYSTEMUS_ROLE_P_H
#define SYSTEMUS_ROLE_P_H

#include <SystemusCore/role.h>
#include <SystemusCore/private/privilege_p.h>

namespace Systemus {

class RolePrivate : public PrivilegedDataPrivate<Role>
{
public:
    RolePrivate() = default;
    RolePrivate(const RolePrivate &other) = default;
};

}

#endif // SYSTEMUS_ROLE_P_H
