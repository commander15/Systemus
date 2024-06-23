#include "role.h"
#include "role_p.h"

namespace Systemus {

Role::Role() :
    PrivilegedData(new RolePrivate)
{
    init<Role>();
}

Role::Role(const Role &other) :
    PrivilegedData(other)
{
    init<Role>();
}

}
