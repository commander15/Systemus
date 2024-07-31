#include "standarduserinterface.h"
#include "standarduserinterface_p.h"
#include "ui_standarduserinterface.h"

#include <SystemusWidgets/dataedit.h>
#include <SystemusWidgets/tableview.h>

#include <SystemusCore/datamodel.h>

#include <SystemusCore/authenticator.h>
#include <SystemusCore/user.h>

#include <QtWidgets/qmessagebox.h>

#include <QtGui/qtextdocument.h>
#include <QtGui/qtextcursor.h>
#include <QtGui/qtexttable.h>
#include <QtGui/qpagelayout.h>
#include <QtGui/qpagesize.h>

#include <QtCore/qtimer.h>

namespace Systemus {

StandardUserInterface::StandardUserInterface(const QByteArray &id, QWidget *parent) :
    UserInterface(new StandardUserInterfacePrivate(id, this), parent),
    ui(new Ui::StandardUserInterface)
{
    S_D(StandardUserInterface);

    ui->setupUi(this);

    d->initContextMenu();

    ui->searchInput->addAction(ui->actionSearchQuery, QLineEdit::LeadingPosition);
    ui->searchInput->addAction(ui->actionSearch, QLineEdit::TrailingPosition);
    ui->searchInput->setCompleter(&d->searchCompleter);

    connect(ui->actionSearch, &QAction::triggered, this, &StandardUserInterface::search);

    connect(ui->refreshButton, &QAbstractButton::clicked, this, &StandardUserInterface::refresh);
    connect(ui->addButton, &QAbstractButton::clicked, this, &StandardUserInterface::addData);
    connect(ui->editButton, &QAbstractButton::clicked, this, &StandardUserInterface::editData);
    connect(ui->deleteButton, &QAbstractButton::clicked, this, &StandardUserInterface::deleteData);
    connect(ui->printButton, &QAbstractButton::clicked, this, &StandardUserInterface::printData);

    connect(ui->tableView, &TableView::dataDoubleClicked, this, [this] {
        if (supportAction(ShowAction))
            showData();
    });

    QString title = id;
    title[0] = title.at(0).toUpper();
    setInterfaceTitle(title + 's');
}

StandardUserInterface::~StandardUserInterface()
{
    delete ui;
}

void StandardUserInterface::search()
{
    S_D(StandardUserInterface);

    d->model->setSearchQuery(ui->searchInput->text());
    d->model->select();
}

bool StandardUserInterface::refresh()
{
    S_D(StandardUserInterface);
    if (d->model->select()) {
        processSelectionChange(QItemSelection(), QItemSelection());
        return true;
    } else {
        return false;
    }
}

void StandardUserInterface::showData()
{
    S_D(StandardUserInterface);
    if (!d->editDialog)
        return;

    d->editDialog->setData(ui->tableView->currentData());
    d->editDialog->setReadOnly(true);
    d->editDialog->exec();
}

void StandardUserInterface::addData()
{
    S_D(StandardUserInterface);
    if (!d->editDialog)
        return;

    d->editDialog->clear();
    d->editDialog->setReadOnly(false);
    if (!d->editDialog->exec())
        return;

    Data data = d->editDialog->data();
    Data::beginTransaction();
    if (data.insert()) {
        Data::commitTransaction();
        refresh();
    } else {
        Data::rollbackTransaction();
        QMessageBox::warning(this, tr("Error"), tr("Error during data add !"));
    }
}

void StandardUserInterface::editData()
{
    S_D(StandardUserInterface);
    if (!d->editDialog)
        return;

    Data data = ui->tableView->currentData();
    d->editDialog->setData(data);
    d->editDialog->setReadOnly(false);
    if (!d->editDialog->exec())
        return;

    data = d->editDialog->data();
    Data::beginTransaction();
    if (data.update()) {
        Data::commitTransaction();
        refresh();
    } else {
        Data::rollbackTransaction();
        QMessageBox::warning(this, tr("Error"), tr("Error during data update !"));
    }
}

void StandardUserInterface::deleteData()
{
    QMessageBox::StandardButton answer;
    answer = QMessageBox::question(this, tr("Deletion"), tr("Do you realy want to delete these data ?"));
    if (answer != QMessageBox::Yes)
        return;

    QList<Data> selectedData = ui->tableView->selectedData();
    Data::beginTransaction();
    for (Data &data : selectedData) {
        if (!data.deleteData()) {
            Data::rollbackTransaction();
            QMessageBox::warning(this, tr("Error"), tr("Error during deletion !"));
            return;
        }
    }
    Data::commitTransaction();

    refresh();
}

void StandardUserInterface::printData()
{
    S_D(StandardUserInterface);
    QTextDocument *doc = new QTextDocument(this);

    const QList<int> rows = ui->tableView->selectedRows();
    for (int row : rows) {
        d->model->selectRow(row);
        fillDocumentForPrinting(doc, d->model->item(row));
    }

    emit printingRequested(doc, documentPageLayout());
}

TableView *StandardUserInterface::tableView() const
{
    return ui->tableView;
}

DataTableModel *StandardUserInterface::dataModel() const
{
    S_D(StandardUserInterface);
    return d->model;
}

void StandardUserInterface::setDataModel(const QByteArray &className)
{
    S_D(StandardUserInterface);

    if (!d->model)
        setDataModel(new DataTableModel(this));

    d->model->setClass(className);
}

void StandardUserInterface::setDataModel(DataTableModel *model)
{
    S_D(StandardUserInterface);

    if (d->model) {
        disconnect(d->model, &QAbstractItemModel::modelReset, this, &StandardUserInterface::configureTableHeaders);
        disconnect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &StandardUserInterface::processSelectionChange);
    }

    ui->tableView->setModel(model);
    d->searchCompleter.setModel(model);
    d->model = model;

    if (d->model) {
        connect(d->model, &QAbstractItemModel::modelReset, this, &StandardUserInterface::configureTableHeaders);
        connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &StandardUserInterface::processSelectionChange);
    }
}

QMenu *StandardUserInterface::contextMenu() const
{
    S_D(StandardUserInterface);
    return d->menu;
}

DataEditDialog *StandardUserInterface::dataEditDialog() const
{
    S_D(const StandardUserInterface);
    return d->editDialog;
}

void StandardUserInterface::setDataEditDialog(DataEditDialog *dialog)
{
    S_D(StandardUserInterface);
    d->editDialog = dialog;
}

QAction *StandardUserInterface::printAction() const
{
    S_D(StandardUserInterface);
    return d->printAction;
}

bool StandardUserInterface::canHandleAction(int action) const
{
    switch (action) {
    case SearchAction:
    case RefreshAction:
    case ShowAction:
    case AddAction:
    case EditAction:
    case DeleteAction:
    case PrintAction:
        return true;

    default:
        return false;
    }
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

bool StandardUserInterface::showDataContextMenu(const QList<Data> &data, QMenu *menu)
{
    S_D(StandardUserInterface);

    const bool single = data.size() == 1;
    d->showAction->setEnabled(supportAction(StandardUserInterface::ShowAction) && single);
    d->editAction->setEnabled(supportAction(StandardUserInterface::EditAction) && single);
    d->deleteAction->setEnabled(supportAction(StandardUserInterface::DeleteAction));
    d->printAction->setEnabled(supportAction(StandardUserInterface::PrintAction));

    Q_UNUSED(menu);
    return true;
}

QPageLayout StandardUserInterface::documentPageLayout() const
{
    return QPageLayout(QPageSize::A4, QPageLayout::Portrait, QMarginsF(), QPageLayout::Millimeter);
}

void StandardUserInterface::fillDocumentForPrinting(QTextDocument *document, const Data &data)
{
    S_D(StandardUserInterface);

    const DataInfo info = data.info();

    QTextCursor cursor(document);

    QTextTable *table;
    {
        QTextTableFormat tableFormat;
        tableFormat.setBorderCollapse(true);
        tableFormat.setHeaderRowCount(1);

        table = cursor.insertTable(info.displayCount() + 1, 2);
        table->mergeCells(0, 0, 1, 2);
    }
    {
        QTextBlockFormat blockFormat;
        blockFormat.setAlignment(Qt::AlignCenter);

        QTextCursor cursor = table->cellAt(0, 0).firstCursorPosition();
        cursor.setBlockFormat(blockFormat);
        cursor.insertText(info.metaObject()->className());
    }

    QTextCharFormat charFormat;

    for (int i(0); i < info.displayCount(); ++i) {
        QTextCursor cursor;

        cursor = table->cellAt(i, 0).firstCursorPosition();
        cursor.insertText(info.fieldName(i), charFormat);

        cursor = table->cellAt(i, 1).firstCursorPosition();
        cursor.insertText(data.property(info.displayPropertyName(i)).toString(), charFormat);
    }
}

void StandardUserInterface::initUi()
{
    S_D(StandardUserInterface);
    if (d->model->rowCount() == 0)
        d->model->select();
    processSelectionChange(QItemSelection(), QItemSelection());
}

void StandardUserInterface::translateUi()
{
    S_D(StandardUserInterface);

    ui->retranslateUi(this);
    d->translateUi();
}

QVariant StandardUserInterface::processAction(int action, const QVariantList &data)
{
    S_D(StandardUserInterface);

    switch (action) {
    case RefreshAction:
        return refresh();

    case SearchAction:
        if (data.size() == 1)
            ui->searchInput->setText(data.constFirst().toString());
        else if (data.size() == 2)
            ui->searchInput->setText(data.constFirst().toString() + " = " + Data::formatValue(data.constLast()));
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

void StandardUserInterface::contextMenuEvent(QContextMenuEvent *event)
{
    const QPoint pos = ui->tableView->viewport()->mapFrom(this, event->pos());

    ui->tableView->setCurrentIndex(ui->tableView->indexAt(pos));
    const QList<Data> data = ui->tableView->selectedData();

    if (!data.isEmpty()) {
        S_D(StandardUserInterface);
        if (showDataContextMenu(data, d->menu)) {
            d->menu->setTitle("Options");
            d->menu->popup(event->globalPos());
            event->accept();
        }
    }
}

void StandardUserInterface::configureTableHeaders()
{
    QHeaderView *view = ui->tableView->horizontalHeader();
    for (int i(0); i < view->count(); ++i)
        view->setSectionResizeMode(i, QHeaderView::Stretch);
}

void StandardUserInterface::processSelectionChange(const QItemSelection &current, const QItemSelection &last)
{
    S_D(StandardUserInterface);

    const int count = current.size();

    ui->addButton->setEnabled(supportAction(AddAction));
    ui->editButton->setEnabled(supportAction(EditAction) && count == 1);
    ui->deleteButton->setEnabled(supportAction(DeleteAction) && count > 0);
    ui->printButton->setEnabled(supportAction(PrintAction) && count > 0);

    d->showAction->setVisible(supportAction(ShowAction) && count == 1);
    d->editAction->setVisible(ui->editButton->isEnabled());
    d->deleteAction->setVisible(ui->deleteButton->isEnabled());
    d->printAction->setVisible(ui->printButton->isEnabled());

    emit actionSupportUpdated(ShowAction, d->showAction->isVisible());
    emit actionSupportUpdated(AddAction, ui->addButton->isEnabled());
    emit actionSupportUpdated(EditAction, d->editAction->isVisible());
    emit actionSupportUpdated(DeleteAction, d->deleteAction->isVisible());
}

StandardUserInterfacePrivate::StandardUserInterfacePrivate(const QByteArray &id, StandardUserInterface *q) :
    UserInterfacePrivate(id, q),
    editDialog(nullptr),
    model(nullptr)
{
}

void StandardUserInterfacePrivate::refreshUi()
{
    S_Q(StandardUserInterface);

    const bool selected = q->ui->tableView->currentIndex().isValid();
    showAction->setVisible(q->supportAction(StandardUserInterface::ShowAction) && selected);
    editAction->setVisible(q->supportAction(StandardUserInterface::EditAction) && selected);
    deleteAction->setVisible(q->supportAction(StandardUserInterface::DeleteAction) && selected);
    printAction->setVisible(q->supportAction(StandardUserInterface::PrintAction) && selected);
}

void StandardUserInterfacePrivate::translateUi()
{
    S_Q(StandardUserInterface);

    showAction->setText(tr("Show"));
    editAction->setText(tr("Edit"));
    deleteAction->setText(tr("Delete"));
    printAction->setText(tr("Print"));
}

void StandardUserInterfacePrivate::initContextMenu()
{
    S_Q(StandardUserInterface);

    menu = new QMenu(q);

    showAction = new QAction(menu);
    showAction->setIcon(QIcon(":/systemus/icons/action_show.png"));
    menu->addAction(showAction);
    QObject::connect(showAction, &QAction::triggered, q, &StandardUserInterface::showData);
    //QObject::connect(showAction, &QAction::visibleChanged, );

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
