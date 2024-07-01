#include "standarduserinterface.h"
#include "standarduserinterface_p.h"
#include "ui_standarduserinterface.h"

#include <SystemusCore/authenticator.h>
#include <SystemusCore/user.h>

#include <QtWidgets/qmessagebox.h>

#include <QtGui/qtextcursor.h>
#include <QtGui/qtexttable.h>

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
    connect(ui->addButton, &QAbstractButton::clicked, this, &StandardUserInterface::addData);
    connect(ui->editButton, &QAbstractButton::clicked, this, &StandardUserInterface::editData);
    connect(ui->deleteButton, &QAbstractButton::clicked, this, &StandardUserInterface::deleteData);
    connect(ui->printButton, &QAbstractButton::clicked, this, &StandardUserInterface::printData);

    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tableView->setModel(&d->model);
    connect(ui->tableView, &TableView::recordDoubleClicked, this, &StandardUserInterface::showRecord);
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
    showRecord(ui->tableView->currentRecord());
}

void StandardUserInterface::addData()
{
    if (!addRecord(ui->tableView->currentRecord()))
        QMessageBox::warning(this, tr("Error"), tr("Error during data add !"));
}

void StandardUserInterface::editData()
{
    if (!editRecord(ui->tableView->currentRecord()))
        QMessageBox::warning(this, tr("Error"), tr("Error during data update !"));
}

void StandardUserInterface::deleteData()
{
    QMessageBox::StandardButton answer;
    answer = QMessageBox::question(this, tr("Deletion"), tr("Do you realy want to delete these records ?"));
    if (answer == QMessageBox::Yes && !deleteRecords(ui->tableView->selectedRecords()))
        QMessageBox::warning(this, tr("Error"), tr("Error during data deletion !"));
}

void StandardUserInterface::printData()
{
    QTextDocument *doc = new QTextDocument(this);

    for (const QSqlRecord &record : ui->tableView->selectedRecords())
        fillDocumentForPrinting(doc, record);

    emit printingRequested(doc);
    doc->deleteLater();
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

QAction *StandardUserInterface::printAction() const
{
    S_D(StandardUserInterface);
    return d->printAction;
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

void StandardUserInterface::showRecord(const QSqlRecord &record)
{
    //
}

bool StandardUserInterface::addRecord(const QSqlRecord &record)
{
    return false;
}

bool StandardUserInterface::editRecord(const QSqlRecord &record)
{
    return false;
}

bool StandardUserInterface::deleteRecords(const QList<QSqlRecord> &records)
{
    return false;
}

void StandardUserInterface::fillDocumentForPrinting(QTextDocument *document, const QSqlRecord &record)
{
    QTextCursor cursor(document);

    QTextTable *table;
    {
        QTextTableFormat tableFormat;
        tableFormat.setBorderCollapse(true);
        tableFormat.setHeaderRowCount(1);

        table = cursor.insertTable(2, record.count());
    }

    QTextBlockFormat blockFormat;
    blockFormat.setAlignment(Qt::AlignCenter);

    QTextCharFormat charFormat;

    for (int i(0); i < record.count(); ++i) {
        QTextCursor cursor;

        cursor = table->cellAt(0, i).firstCursorPosition();
        cursor.setBlockFormat(blockFormat);
        cursor.insertText(record.fieldName(i), charFormat);

        cursor = table->cellAt(1, i).firstCursorPosition();
        cursor.setBlockFormat(blockFormat);
        cursor.insertText(record.value(i).toString(), charFormat);
    }
}

void StandardUserInterface::showRecordsContextMenu(const QList<QSqlRecord> &records, const QPoint &pos)
{
    S_D(StandardUserInterface);

    if (records.isEmpty())
        return;

    bool single = records.size() == 1;
    d->showAction->setEnabled(supportAction(UserInterface::ShowAction) && single);
    d->editAction->setEnabled(supportAction(UserInterface::EditAction) && single);
    d->deleteAction->setEnabled(supportAction(UserInterface::DeleteAction));
    d->printAction->setEnabled(supportAction(UserInterface::PrintAction));

    d->menu->popup(pos);
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
    QPoint cursorPos = ui->tableView->viewport()->mapTo(ui->tableView, pos);
    ui->tableView->setCurrentIndex(ui->tableView->indexAt(pos));
    showRecordsContextMenu(ui->tableView->selectedRecords(), cursorPos);
}

StandardUserInterfacePrivate::StandardUserInterfacePrivate(const QByteArray &id, StandardUserInterface *q) :
    UserInterfacePrivate(id, q)
{
}

void StandardUserInterfacePrivate::refreshUi()
{
    S_Q(StandardUserInterface);

    bool selected = q->ui->tableView->currentIndex().isValid();

    showAction->setEnabled(q->supportAction(UserInterface::ShowAction) && selected);
    editAction->setEnabled(q->supportAction(UserInterface::EditAction) && selected);
    deleteAction->setEnabled(q->supportAction(UserInterface::DeleteAction) && selected);
    printAction->setEnabled(q->supportAction(UserInterface::PrintAction) && selected);
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
