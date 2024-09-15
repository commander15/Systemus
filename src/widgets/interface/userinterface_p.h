#ifndef SYSTEMUS_USERINTERFACE_P_H
#define SYSTEMUS_USERINTERFACE_P_H

#include <SystemusWidgets/userinterface.h>

namespace Systemus {

class UserInterfacePrivate
{
public:
    UserInterfacePrivate(UserInterface *q);
    ~UserInterfacePrivate();

    virtual void init();

    QAction *action() const;

    UserInterface *q_ptr;

    QByteArray id;

private:
    mutable QAction *m_action;
};

}

#endif // SYSTEMUS_USERINTERFACE_P_H
