#include "group.h"
#include "group_p.h"

namespace Systemus {

Group::Group() :
    PrivilegedData(new GroupPrivate)
{
    init();
}

Group::Group(const Group &other) :
    PrivilegedData(other, false)
{
}

Group &Group::operator=(const Group &other)
{
    PrivilegedData::operator=(other);
    return *this;
}

GroupPrivate::GroupPrivate() :
    PrivilegedDataPrivate("Group")
{
}

}
