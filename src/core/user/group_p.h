#ifndef SYSTEMUS_GROUP_P_H
#define SYSTEMUS_GROUP_P_H

#include <SystemusCore/group.h>
#include <SystemusCore/private/privilege_p.h>

namespace Systemus {

class GroupPrivate : public PrivilegedDataPrivate
{
public:
    GroupPrivate();
    GroupPrivate(const GroupPrivate &other) = default;

    int dataType() const override
    { return GroupDataType; }
};

}

#endif // SYSTEMUS_GROUP_P_H
