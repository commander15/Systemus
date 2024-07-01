#include "global.h"

#include <SystemusCore/user.h>
#include <SystemusCore/role.h>
#include <SystemusCore/privilege.h>
#include <SystemusCore/permission.h>

void _s_register_internal_types()
{
    sRegisterType<Systemus::User>();
    sRegisterType<Systemus::Role>();
    sRegisterType<Systemus::Privilege>();
    sRegisterType<Systemus::Permission>();
}
