#ifndef SYSTEMUS_PERMISSION_H
#define SYSTEMUS_PERMISSION_H

#include <SystemusCore/global.h>
#include <SystemusCore/authorizationdata.h>

namespace Systemus {

class SYSTEMUS_CORE_EXPORT Permission : public AuthorizationData
{
    Q_GADGET

public:
    enum PermissionStatus {
        ActivePermission,
        InactivePermission,
        UndefinedPermission
    };

private:
    S_DATA(Permission)
};

}

#endif // SYSTEMUS_PERMISSION_H
