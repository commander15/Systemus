#include "permission.h"

namespace Systemus {

Permission::Permission()
{
    init<Permission>();
}

Permission::Permission(const Permission &other) :
    AuthorizationData(other)
{
    init<Permission>();
}

Permission::~Permission()
{
}

}
