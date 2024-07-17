#include "global.h"

#include <SystemusCore/user.h>
#include <SystemusCore/role.h>
#include <SystemusCore/group.h>
#include <SystemusCore/privilege.h>
#include <SystemusCore/permission.h>


void Systemus::init(int argc, char *argv[])
{
    _s_register_internal_types();
}

void _s_register_internal_types()
{
    static bool registered(false);
    if (!registered)
        registered = true;
    else
        return;

    sRegisterType<Systemus::User>();
    sRegisterType<Systemus::UserProfile>();
    sRegisterType<Systemus::Role>();
    sRegisterType<Systemus::Group>();
    sRegisterType<Systemus::Privilege>();
    sRegisterType<Systemus::Permission>();
}
