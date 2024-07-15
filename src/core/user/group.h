#ifndef SYSTEMUS_GROUP_H
#define SYSTEMUS_GROUP_H

#include <SystemusCore/global.h>
#include <SystemusCore/privilege.h>

namespace Systemus {

class SYSTEMUS_CORE_EXPORT Group : public PrivilegedData
{
    Q_GADGET

public:
    Group();
    Group(const Group &other);

    Group &operator=(const Group &other);

private:
    S_DATA(Group);
};

}

Q_DECLARE_METATYPE(Systemus::Group)

#endif // SYSTEMUS_GROUP_H
