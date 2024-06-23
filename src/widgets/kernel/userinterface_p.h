#ifndef SYSTEMUS_USERINTERFACE_P_H
#define SYSTEMUS_USERINTERFACE_P_H

#include <SystemusWidgets/userinterface.h>

namespace Systemus {

class UserInterfacePrivate
{
public:
    UserInterfacePrivate(const QByteArray &id, UserInterface *qq);

    virtual void init();

    QAction *action() const;

    UserInterface *q;

    const QByteArray id;

private:
    mutable QAction *_action;
};

}

#endif // SYSTEMUS_USERINTERFACE_P_H
