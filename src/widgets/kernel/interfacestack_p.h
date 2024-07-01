#ifndef SYSTEMUS_INTERFACESTACK_P_H
#define SYSTEMUS_INTERFACESTACK_P_H

#include <SystemusWidgets/interfacestack.h>

#include <QtWidgets/qstackedwidget.h>

namespace Systemus {

class InterfaceStackPrivate
{
public:
    InterfaceStackPrivate(InterfaceStack *q);

    InterfaceStack *q_ptr;
};

}

#endif // SYSTEMUS_INTERFACESTACK_P_H
