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

    ui->searchInput->addAction(ui->actionSearchQuery, QLineEdit::LeadingPosition);
    ui->searchInput->addAction(ui->actionSearch, QLineEdit::TrailingPosition);

    connect(ui->actionSearch, &QAction::triggered, this, &StandardUserInterface::search);

    connect(ui->refreshButton, &QAbstractButton::clicked, this, &StandardUserInterface::refresh);

    ui->tableView->setModel(&d->model);
    connect(ui->tableView, &QWidget::customContextMenuRequested, this, &StandardUserInterface::showContextMenu);
}

StandardUserInterface::~StandardUserInterface()
{
    delete ui;
}

void StandardUserInterface::search()
{
    S_D(StandardUserInterface);

    QString field = d->model.tableName();
    d->model.setFilter(field + " = '" + ui->searchInput->text() + '\'');
    d->model.select();
}

bool StandardUserInterface::refresh()
{
    S_D(StandardUserInterface);
    return d->model.select();
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

DataModel *StandardUserInterface::dataModel() const
{
    S_D(StandardUserInterface);
    return &d->model;
}

void StandardUserInterface::initUi()
{
    S_D(StandardUserInterface);

    if (d->model.rowCount() == 0)
        d->model.select();
}

void StandardUserInterface::translateUi()
{
    ui->retranslateUi(this);
}

QVariant StandardUserInterface::processAction(InterfaceAction action, const QVariant &data)
{
    switch (action) {
    case RefreshAction:
        return refresh();

    default:
        return QVariant();
    }
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

StandardUserInterfacePrivate::StandardUserInterfacePrivate(const QByteArray &id, StandardUserInterface *qq) :
    UserInterfacePrivate(id, qq)
{
}

}
