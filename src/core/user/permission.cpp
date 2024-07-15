#include "permission.h"

#include <SystemusCore/private/authorizationdata_p.h>

namespace Systemus {

Permission::Permission() :
    AuthorizationData()
{
    init();
}

Permission::Permission(const Permission &other) :
    AuthorizationData(other, false)
{
}

Permission &Permission::operator=(const Permission &other)
{
    AuthorizationData::operator=(other);
    return *this;
}

}
