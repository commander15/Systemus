#include "standarduserinterface.h"
#include "standarduserinterface_p.h"
#include "ui_standarduserinterface.h"

namespace Systemus {

StandardUserInterface::StandardUserInterface(const QByteArray &id, QWidget *parent) :
    UserInterface(new StandardUserInterfacePrivate(id, this), parent),
    ui(new Ui::StandardUserInterface)
{
    S_D(StandardUserInterface);

    ui->setupUi(this);

    ui->tableView->setModel(&d->model);
    connect(ui->tableView, &QWidget::customContextMenuRequested, this, &StandardUserInterface::showContextMenu);
}

StandardUserInterface::~StandardUserInterface()
{
    delete ui;
}

TableView *StandardUserInterface::tableView() const
{
    return ui->tableView;
}

QMenu *StandardUserInterface::contextMenu() const
{
    S_D(StandardUserInterface);
    return &d->menu;
}

QSqlQueryModel *StandardUserInterface::dataModel() const
{
    S_D(StandardUserInterface);
    return &d->model;
}

void StandardUserInterface::showContextMenu(const QPoint &pos)
{
    S_D(StandardUserInterface);
    QSqlRecord record = ui->tableView->recordAt(pos);

    if (!record.isEmpty()) {
        QPoint cursorPos = ui->tableView->viewport()->mapTo(this, pos);
        d->menu.popup(cursorPos);
    }
}

void StandardUserInterface::refresh()
{
    S_D(StandardUserInterface);
    d->model.setQuery(d->data.selectStatement());
}

StandardUserInterfacePrivate::StandardUserInterfacePrivate(const QByteArray &id, StandardUserInterface *qq) :
    UserInterfacePrivate(id, qq)
{
}

}
