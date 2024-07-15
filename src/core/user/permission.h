#ifndef SYSTEMUS_PERMISSION_H
#define SYSTEMUS_PERMISSION_H

#include <SystemusCore/global.h>
#include <SystemusCore/authorizationdata.h>

namespace Systemus {

class SYSTEMUS_CORE_EXPORT Permission : public AuthorizationData
{
    Q_GADGET

public:
    Permission();
    Permission(const Permission &other);

    Permission &operator=(const Permission &other);

private:
    S_DATA(Permission)
};

}

Q_DECLARE_METATYPE(Systemus::Permission)

#endif // SYSTEMUS_PERMISSION_H
