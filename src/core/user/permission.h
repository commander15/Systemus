#ifndef SYSTEMUS_PERMISSION_H
#define SYSTEMUS_PERMISSION_H

#include <SystemusCore/global.h>
#include <SystemusCore/data.h>

namespace Systemus {

class SYSTEMUS_CORE_EXPORT Permission : public AuthorizationData
{
    Q_GADGET

public:
    Permission();
    Permission(const Permission &other);
    ~Permission();

private:
    SYSTEMUS_DATA_METHODS(Permission)
};

class SYSTEMUS_CORE_EXPORT IssuedPermission : public IssuedAuthorizationData
{
    Q_GADGET

public:
    //IssuedPermission();
    //IssuedPermission(const IssuedPermission &other);
    //~IssuedPermission();
};

}

#endif // SYSTEMUS_PERMISSION_H
