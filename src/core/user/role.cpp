#include "role.h"
#include "role_p.h"

namespace Systemus {

Role::Role() :
    PrivilegedData(new RolePrivate)
{
}

Role::Role(const Role &other) :
    PrivilegedData(other)
{
}

RolePrivate::RolePrivate() :
    PrivilegedDataPrivate("Role", true)
{
}

}
