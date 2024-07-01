#ifndef SYSTEMUS_STANDARDUSERINTERFACE_P_H
#define SYSTEMUS_STANDARDUSERINTERFACE_P_H

#include <SystemusWidgets/standarduserinterface.h>
#include <SystemusWidgets/private/userinterface_p.h>
#include <SystemusWidgets/tableview.h>

#include <SystemusCore/datamodel.h>

#include <QtWidgets/qmenu.h>

#include <QtCore/qcoreapplication.h>

namespace Systemus {

class StandardUserInterfacePrivate : public UserInterfacePrivate
{
    Q_DECLARE_TR_FUNCTIONS(StandardUserInterface)

public:
    StandardUserInterfacePrivate(const QByteArray &id, StandardUserInterface *q);

    void refreshUi();
    void translateUi();

    void init() override;

    QList<UserInterface::InterfaceAction> supportedActions;

    QMenu *menu;
    QAction *showAction;
    QAction *editAction;
    QAction *deleteAction;
    QAction *printAction;

    DataModel model;
};

}

#endif // SYSTEMUS_STANDARDUSERINTERFACE_P_H
