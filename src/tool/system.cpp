#include "system.h"

Installation::Installation()
{
    init<Installation>();
}

System::System() :
    installations(Systemus::OneToManyRelationship<System, Installation>())
{
    init<System>();
}
