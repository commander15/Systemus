#ifndef SYSTEMUS_STANDARDUSERINTERFACE_P_H
#define SYSTEMUS_STANDARDUSERINTERFACE_P_H

#include <SystemusWidgets/standarduserinterface.h>
#include <SystemusWidgets/private/userinterface_p.h>
#include <SystemusWidgets/tableview.h>

#include <QtWidgets/qmenu.h>

#include <QtSql/qsqlquerymodel.h>

namespace Systemus {

class StandardUserInterfacePrivate : public UserInterfacePrivate
{
public:
    StandardUserInterfacePrivate(const QByteArray &id, StandardUserInterface *qq);

    QList<UserInterface::InterfaceAction> supportedActions;

    QMenu menu;
    QSqlQueryModel model;
    Data data;
};

}

#endif // SYSTEMUS_STANDARDUSERINTERFACE_P_H
