#ifndef SYSTEMUS_ROLE_H
#define SYSTEMUS_ROLE_H

#include <SystemusCore/global.h>
#include <SystemusCore/privilege.h>

namespace Systemus {

class SYSTEMUS_CORE_EXPORT Role : public PrivilegedData
{
    Q_GADGET

public:
    Role();
    Role(const Role &other);

private:
    S_DATA(Role)
};

}

#endif // SYSTEMUS_ROLE_H
