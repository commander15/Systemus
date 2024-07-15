#include "role.h"
#include "role_p.h"

namespace Systemus {

Role::Role() :
    PrivilegedData(new RolePrivate)
{
    init();
}

Role::Role(const Role &other) :
    PrivilegedData(other, false)
{
}

Role &Role::operator=(const Role &other)
{
    PrivilegedData::operator=(other);
    return *this;
}

RolePrivate::RolePrivate() :
    PrivilegedDataPrivate("Role")
{
}

}
