#ifndef SYSTEMUS_STANDARDUSERINTERFACE_H
#define SYSTEMUS_STANDARDUSERINTERFACE_H

#include <SystemusWidgets/global.h>
#include <SystemusWidgets/userinterface.h>

class QMenu;

class QSqlQueryModel;

namespace Systemus {

namespace Ui {
class StandardUserInterface;
}

class TableView;

class StandardUserInterfacePrivate;
class SYSTEMUS_WIDGETS_EXPORT StandardUserInterface : public UserInterface
{
    Q_OBJECT

public:
    explicit StandardUserInterface(const QByteArray &id, QWidget *parent = nullptr);
    virtual ~StandardUserInterface();

    TableView *tableView() const;
    QMenu *contextMenu() const;
    QSqlQueryModel *dataModel() const;

    template<typename T>
    void setInterfaceModel()
    { }

    InterfaceType interfaceType() const override
    { return StandardInterface; }

private:
    Q_SLOT void showContextMenu(const QPoint &pos);

    Q_SLOT void refresh();

    Ui::StandardUserInterface *ui;
};

}

#endif // SYSTEMUS_STANDARDUSERINTERFACE_H
