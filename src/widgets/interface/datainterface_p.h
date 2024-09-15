#ifndef SYSTEMUS_DATAINTERFACE_P_H
#define SYSTEMUS_DATAINTERFACE_P_H

#include <SystemusWidgets/datainterface.h>
#include <SystemusWidgets/private/userinterface_p.h>

namespace Systemus {

class DataInterfacePrivate : public UserInterfacePrivate
{
public:
    DataInterfacePrivate(DataInterface *q);

    void createContextMenu();

    bool uiInitialized;
    bool selectionInitialized;

    QMenu *contextMenu;
    QAction *showAction;
    QAction *editAction;
    QAction *deleteAction;

    DataEditDialog *editDialog;

    Orm::DataTableModel *model;
};

}

#endif // SYSTEMUS_DATAINTERFACE_P_H
