#include "standarduserinterface.h"
#include "standarduserinterface_p.h"
#include "ui_standarduserinterface.h"

#include <SystemusCore/authenticator.h>
#include <SystemusCore/user.h>

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

    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
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

void StandardUserInterface::showData()
{
    //
}

void StandardUserInterface::addData()
{
    //
}

void StandardUserInterface::editData()
{
    //
}

void StandardUserInterface::deleteData()
{
    //
}

void StandardUserInterface::printData()
{
    //
}

TableView *StandardUserInterface::tableView() const
{
    return ui->tableView;
}

QMenu *StandardUserInterface::contextMenu() const
{
    S_D(StandardUserInterface);
    return d->menu;
}

DataModel *StandardUserInterface::dataModel() const
{
    S_D(StandardUserInterface);
    return &d->model;
}

bool StandardUserInterface::supportAction(int action) const
{
    S_D(const StandardUserInterface);

    const User user = Authenticator::instance()->loggedUser();

    switch (action) {
    case RefreshAction:
    case SearchAction:
        return user.hasPermission(d->id + ".list");

    case ShowAction:
        return user.hasPermission(d->id + ".read");

    case AddAction:
        return user.hasPermission(d->id + ".create");

    case EditAction:
        return user.hasPermission(d->id + ".update");

    case DeleteAction:
        return user.hasPermission(d->id + ".delete");

    case PrintAction:
        return user.hasPermission(d->id + ".print");

    default:
        return false;
    }
}

void StandardUserInterface::initUi()
{
    S_D(StandardUserInterface);

    if (d->model.rowCount() == 0)
        d->model.select();

    for (int i(d->model.fieldCount()); i < d->model.columnCount(); ++i)
        ui->tableView->hideColumn(i);

    d->refreshUi();
}

void StandardUserInterface::translateUi()
{
    S_D(StandardUserInterface);

    ui->retranslateUi(this);
    d->translateUi();
}

QVariant StandardUserInterface::processAction(int action, const QVariant &data)
{
    switch (action) {
    case RefreshAction:
        return refresh();

    case SearchAction:
        if (!data.toString().isEmpty())
            ui->searchInput->setText(data.toString());
        search();
        break;

    case ShowAction:
        showData();
        break;

    case AddAction:
        addData();
        break;

    case EditAction:
        editData();
        break;

    case DeleteAction:
        deleteData();
        break;

    case PrintAction:
        printData();
        break;
    }

    return QVariant();
}

void StandardUserInterface::showContextMenu(const QPoint &pos)
{
    S_D(StandardUserInterface);

    d->refreshUi();

    QSqlRecord record = ui->tableView->recordAt(pos);

    if (!record.isEmpty()) {
        QPoint cursorPos = ui->tableView->viewport()->mapTo(ui->tableView, pos);
        d->menu->popup(ui->tableView->mapToGlobal(cursorPos));
    }
}

StandardUserInterfacePrivate::StandardUserInterfacePrivate(const QByteArray &id, StandardUserInterface *qq) :
    UserInterfacePrivate(id, qq)
{
}

void StandardUserInterfacePrivate::refreshUi()
{
    S_Q(StandardUserInterface);

    showAction->setEnabled(q->supportAction(UserInterface::ShowAction));
    editAction->setEnabled(q->supportAction(UserInterface::EditAction));
    deleteAction->setEnabled(q->supportAction(UserInterface::DeleteAction));
    printAction->setEnabled(q->supportAction(UserInterface::PrintAction));
}

void StandardUserInterfacePrivate::translateUi()
{
    S_Q(StandardUserInterface);

    showAction->setText(tr("Show"));
    editAction->setText(tr("Edit"));
    deleteAction->setText(tr("Delete"));
    printAction->setText(tr("Print"));
}

void StandardUserInterfacePrivate::init()
{
    S_Q(StandardUserInterface);

    menu = new QMenu(q);

    showAction = new QAction(menu);
    showAction->setIcon(QIcon(":/systemus/icons/action_show.png"));
    menu->addAction(showAction);
    QObject::connect(showAction, &QAction::triggered, q, &StandardUserInterface::showData);

    editAction = new QAction(menu);
    editAction->setIcon(QIcon(":/systemus/icons/action_edit.png"));
    menu->addAction(editAction);
    QObject::connect(editAction, &QAction::triggered, q, &StandardUserInterface::editData);

    deleteAction = new QAction(menu);
    deleteAction->setIcon(QIcon(":/systemus/icons/action_delete.png"));
    menu->addAction(deleteAction);
    QObject::connect(deleteAction, &QAction::triggered, q, &StandardUserInterface::deleteData);

    printAction = new QAction(menu);
    printAction->setIcon(QIcon(":/systemus/icons/action_print.png"));
    menu->addAction(printAction);
    QObject::connect(printAction, &QAction::triggered, q, &StandardUserInterface::printData);

    translateUi();
    UserInterfacePrivate::init();
}

}
