#include "datainterface.h"
#include "datainterface_p.h"
#include "ui_datainterface.h"

#include <SystemusWidgets/filterwidget.h>
#include <SystemusWidgets/dataedit.h>

#include <SystemusCore/data.h>
#include <SystemusCore/datatablemodel.h>
#include <SystemusCore/metamapper.h>

#include <QtWidgets/qmessagebox.h>
#include <QtWidgets/qmenu.h>

#include <QtGui/qevent.h>

#include <QtSql/qsqlerror.h>

namespace Systemus {

DataInterface::DataInterface(QWidget *parent) :
    UserInterface(new DataInterfacePrivate(this), parent),
    ui(new Ui::DataInterface)
{
    S_D(DataInterface);

    ui->setupUi(this);

    ui->filterFrame->setVisible(false);

    d->createContextMenu();

    connect(ui->searchInput, &QLineEdit::textEdited, this, [=](const QString &query) {
        ui->searchInput->setProperty("query", query);
    });
    connect(ui->searchInput, &QLineEdit::returnPressed, this, QOverload<>::of(&DataInterface::search));

    ui->toggleFiltersButtons->setVisible(false);
    connect(ui->filterButton, &QAbstractButton::clicked, this, &DataInterface::refresh);
    connect(ui->refreshButton, &QAbstractButton::clicked, this, &DataInterface::refresh);
    connect(ui->addButton, &QAbstractButton::clicked, this, &DataInterface::addNewData);
    connect(ui->editButton, &QAbstractButton::clicked, this, &DataInterface::editCurrentData);
    connect(ui->deleteButton, &QAbstractButton::clicked, this, &DataInterface::deleteSelectedData);

    connect(ui->tableView, &QAbstractItemView::doubleClicked, this, [this, d](const QModelIndex &index) {
        if (supportAction(ShowAction))
            showData(d->model->item(index.row()));
    });

    connect(ui->previousPageButton, &QAbstractButton::clicked, this, [d, this] {
        if (d->model)
            d->model->setPage(d->model->page() - 1);
        refresh();
    });

    connect(ui->nextPageButton, &QAbstractButton::clicked, this, [d, this] {
        if (d->model)
            d->model->setPage(d->model->page() + 1);
        refresh();
    });
}

DataInterface::~DataInterface()
{
    delete ui;
}

bool DataInterface::search()
{
    const QString query = ui->searchInput->text();
    return searchWithLabel(query, query);
}

bool DataInterface::search(const QString &query)
{
    return searchWithLabel(query, query);
}

bool DataInterface::searchWithLabel(const QString &query, const QString &label)
{
    ui->searchInput->setProperty("query", query);
    ui->searchInput->setText(label);
    return refresh();
}

bool DataInterface::refresh()
{
    S_D(DataInterface);

    if (d->model) {
        QStringList filters;
        {
            const QString query = ui->searchInput->property("query").toString();
            if (!query.isEmpty())
                filters.append(searchFilter(query));
        }

        {
            const QString user = userFilter();
            if (!user.isEmpty())
                filters.append(user);
        }

        {
            const QString extra = extraFilter();
            if (!extra.isEmpty())
                filters.append(extra);
        }
        std::transform(filters.begin(), filters.end(), filters.begin(), [](const QString &filter) {
            return '(' + filter + ')';
        });

        d->model->setFilter(filters.join(" AND "));
        if (d->model->select()) {
            ui->tableView->scrollToTop();

            ui->pageInput->setMinimum(d->model->pageCount() == 0 ? 0 : 1);
            ui->pageInput->setMaximum(d->model->pageCount());
            ui->pageInput->setSpecialValueText(d->model->pageCount() == 0 ? tr("N / A") : QString());
            ui->pageInput->setValue(d->model->page());

            ui->previousPageButton->setEnabled(d->model->page() > 1);
            ui->nextPageButton->setEnabled(ui->pageInput->value() < d->model->pageCount());
            return true;
        }
    }

    return false;
}

void DataInterface::showCurrentData()
{
    S_D(DataInterface);
    if (supportAction(ShowAction) && d->model)
        showData(d->model->item(ui->tableView->currentIndex().row()));
}

void DataInterface::showData(const Data &data)
{
    S_D(DataInterface);
    if (d->editDialog) {
        d->editDialog->setData(data);
        d->editDialog->setReadOnly(true);
        d->editDialog->exec();
    }
}

bool DataInterface::editCurrentData()
{
    S_D(DataInterface);
    if (supportAction(EditAction) && d->model)
        return editData(d->model->item(ui->tableView->currentIndex().row()));
    else
        return false;
}

bool DataInterface::editData(const Data &data)
{
    S_D(DataInterface);

    Data editedData;
    if (d->editDialog) {
        d->editDialog->setData(data);
        d->editDialog->setReadOnly(false);
        if (d->editDialog->exec())
            editedData = d->editDialog->data();
        else
            return false;
    }

    Systemus::beginTransaction();
    if (editedData.update()) {
        bool ok = Systemus::commitTransaction();
        refresh();
        return ok;
    } else {
        Systemus::rollbackTransaction();
        showErrorMessage(tr("Error"), tr("Error during data update !"), editedData.lastError());
        return false;
    }
}

QVariant DataInterface::addNewData()
{
    S_D(DataInterface);
    if (supportAction(AddAction))
        return addData(d->model ? d->model->item() : Data());
    else
        return QVariant();
}

QVariant DataInterface::addData(const Data &data)
{
    S_D(DataInterface);

    Data editedData;
    if (d->editDialog) {
        d->editDialog->setData(data);
        d->editDialog->setReadOnly(false);
        if (d->editDialog->exec())
            editedData = d->editDialog->data();
        else
            return QVariant();
    }

    Systemus::beginTransaction();
    if (editedData.insert()) {
        Systemus::commitTransaction();
        refresh();
        return editedData.primaryValue();
    } else {
        Systemus::rollbackTransaction();
        showErrorMessage(tr("Error"), tr("Error during data add !"), editedData.lastError());
        return QVariant();
    }
}

int DataInterface::deleteSelectedData()
{
    S_D(DataInterface);

    if (!supportAction(DeleteAction))
        return 0;

    QList<Data> data;
    const QModelIndexList indexes = ui->tableView->selectionModel()->selectedRows();

    for (const QModelIndex &index : indexes)
        data.append(d->model->item(index.row()));

    return deleteData(data);
}

int DataInterface::deleteData(const QList<Data> &data)
{
    QMessageBox::StandardButton answer;
    answer = QMessageBox::question(this, tr("Deletion"), tr("Do you realy want to delete these data ?"));
    if (answer != QMessageBox::Yes)
        return 0;

    Systemus::beginTransaction();

    for (Data d : data) {
        if (!d.deleteData()) {
            Systemus::rollbackTransaction();
            showErrorMessage(tr("Error"), tr("Error during deletion !"), d.lastError());
            return false;
        }
    }

    Systemus::commitTransaction();

    refresh();
    return data.size();
}

QTableView *DataInterface::tableView() const
{
    return ui->tableView;
}

QMenu *DataInterface::contextMenu() const
{
    S_D(const DataInterface);
    return d->contextMenu;
}

FilterWidget *DataInterface::filterWidget() const
{
    S_D(const DataInterface);
    return d->filterWidget;
}

void DataInterface::setFilterWidget(FilterWidget *widget)
{
    S_D(DataInterface);
    d->filterWidget = widget;
    ui->filterContainer->layout()->addWidget(widget);
    ui->toggleFiltersButtons->setVisible(widget);
}

DataEditDialog *DataInterface::editDialog() const
{
    S_D(const DataInterface);
    return d->editDialog;
}

void DataInterface::setEditDialog(DataEditDialog *dialog)
{
    S_D(DataInterface);
    d->editDialog = dialog;
}

Orm::DataTableModel *DataInterface::model() const
{
    S_D(const DataInterface);
    return d->model;
}

void DataInterface::setModel(Orm::DataTableModel *model)
{
    S_D(DataInterface);
    ui->tableView->setModel(model);
    d->model = model;
}

bool DataInterface::supportAction(int action) const
{
    S_D(const DataInterface);

    /*const User user = Authenticator::instance()->loggedUser();
    const QByteArray id = interfaceId();

    switch (action) {
    case RefreshAction:
    case SearchAction:
        return user.hasPermission(id + ".list");

    case ShowAction:
        return user.hasPermission(id + ".read");

    case AddAction:
        return user.hasPermission(id + ".create");

    case EditAction:
        return user.hasPermission(id + ".update");

    case DeleteAction:
        return user.hasPermission(id + ".delete");

    default:
        return false;
    }*/

    switch (action) {
    case RefreshAction:
    case SearchAction:
    case DeleteAction:
        return true;

    default:
        return d->editDialog;
    }
}

bool DataInterface::canHandleAction(int action) const
{
    S_D(const DataInterface);

    switch (action) {
    case SearchAction:
    case RefreshAction:
    case DeleteAction:
        return d->model;

    case ShowAction:
        return d->editDialog;

    case EditAction:
    case AddAction:
        return d->editDialog && d->model;

    default:
        return false;
    }
}

void DataInterface::initUi()
{
    S_D(DataInterface);

    if (!d->uiInitialized) {
        translateUi(false);
        d->uiInitialized = true;
    }

    if (!d->selectionInitialized && ui->tableView->selectionModel()) {
        processSelectionChange(QItemSelection(), QItemSelection());
        connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &DataInterface::processSelectionChange);

        d->selectionInitialized = true;
    }

    if (d->model && d->model->rowCount() == 0) {
        refresh();
    }
}

void DataInterface::cleanupUi()
{
}

void DataInterface::translateUi(bool full)
{
    S_D(DataInterface);

    d->showAction->setText(tr("Show"));
    d->editAction->setText(tr("Edit"));
    d->deleteAction->setText(tr("Delete"));

    if (full)
        ui->retranslateUi(this);

}

void DataInterface::showErrorMessage(const QString &title, const QString &text, const QSqlError &error)
{
    QMessageBox box;
    box.setWindowTitle(title);
    box.setText(text);
    box.setDetailedText("Error code: " + error.nativeErrorCode());
    box.setIcon(QMessageBox::Warning);
    box.exec();
}

bool DataInterface::showDataContextMenu(const QList<Data> &data, QMenu *menu)
{
    S_D(DataInterface);

    const int count = data.size();
    d->showAction->setVisible(supportAction(ShowAction) && count == 1);
    d->editAction->setVisible(supportAction(EditAction) && count == 1);
    d->deleteAction->setVisible(supportAction(DeleteAction) && count > 0);

    Q_UNUSED(menu);
    return !data.isEmpty();
}

void DataInterface::contextMenuEvent(QContextMenuEvent *event)
{
    S_D(DataInterface);

    QList<Data> data;
    const QModelIndexList indexes = ui->tableView->selectionModel()->selectedRows();
    for (const QModelIndex &index : indexes)
        data.append(d->model->item(index.row()));

    if (data.size() == 1) {
        const QPoint pos = ui->tableView->viewport()->mapFrom(this, event->pos());
        ui->tableView->setCurrentIndex(ui->tableView->indexAt(pos));
    }

    if (showDataContextMenu(data, d->contextMenu)) {
        d->contextMenu->popup(event->globalPos());
        event->accept();
    } else {
        UserInterface::contextMenuEvent(event);
    }
}

QVariant DataInterface::processAction(int action, const QVariantList &data)
{
    switch (action) {
    case SearchAction:
        switch (data.size()) {
        case 0:
            return search();

        case 1:
            return search(data.constFirst().toString());

        case 2:
        default:
            return searchWithLabel(data.constFirst().toString(), data.constLast().toString());
        }

    case RefreshAction:
        return refresh();

    case ShowAction:
        if (data.isEmpty())
            showCurrentData();
        else
            showData(data.constFirst().value<Data>());
        return QVariant();

    case EditAction:
        if (data.isEmpty())
            return editCurrentData();
        else
            return editData(data.constFirst().value<Data>());

    case AddAction:
        return addData(!data.isEmpty() ? data.constFirst().value<Data>() : Data());

    case DeleteAction:
        if (data.isEmpty())
            return deleteSelectedData();
        else {
            QList<Data> deletionData;
            deletionData.resize(data.size());
            std::transform(data.begin(), data.end(), deletionData.begin(), [](const QVariant &data) {
                return data.value<Data>();
            });
            return deleteData(deletionData);
        }

    default:
        return QVariant();
    }
}

QAbstractButton *DataInterface::addButton(const QIcon &icon, const QString &text)
{
    QToolButton *button = new QToolButton(this);
    button->setIcon(icon);
    button->setText(text);
    addButton(button);
    return button;
}

void DataInterface::addButton(QAbstractButton *button)
{
    ui->topBar->addWidget(button);
}

QAction *DataInterface::showAction() const
{
    S_D(const DataInterface);
    return d->showAction;
}

QAction *DataInterface::editAction() const
{
    S_D(const DataInterface);
    return d->editAction;
}

QAction *DataInterface::deleteAction() const
{
    S_D(const DataInterface);
    return d->deleteAction;
}

QString DataInterface::searchFilter(const QString &query) const
{
    S_D(const DataInterface);
    const Orm::MetaTable table = d->model->classTable();

    QList<QMetaProperty> properties;
    for (int i(0); i < table.searchCount(); ++i)
        properties.append(table.searchProperty(i));
    if (properties.isEmpty())
        properties.append(table.userProperty());

    QStringList expressions;
    expressions.resize(properties.size());

    const int options = Orm::MetaMapper::IncludeTableName|Orm::MetaMapper::EscapeIdentifiers;

    std::transform(properties.begin(), properties.end(), expressions.begin(), [table, options, query](const Orm::SecretProperty &property) {
        const QString &prop = Orm::MetaMapper::fieldName(property.name(), table, options);
        if (property.metaType().id() == QMetaType::QString) {
            return prop + " LIKE " + formatValue(query + '%');
        } else {
            QVariant value = query;
            value.convert(property.metaType());
            return prop + " = " + formatValue(value);
        }
    });

    return expressions.join(" AND ");
}

QString DataInterface::userFilter() const
{
    S_D(const DataInterface);
    return (d->filterWidget ? d->filterWidget->filter() : QString());
}

QString DataInterface::extraFilter() const
{
    return QString();
}

void DataInterface::configureTableHeaders()
{
    QHeaderView *view = ui->tableView->horizontalHeader();
    view->hideSection(0);
    for (int i(1); i < view->count(); ++i)
        view->setSectionResizeMode(i, QHeaderView::Stretch);
}

void DataInterface::processSelectionChange(const QItemSelection &current, const QItemSelection &last)
{
    S_D(DataInterface);

    const int count = current.size();

    ui->editButton->setEnabled(supportAction(EditAction) && count == 1);
    ui->addButton->setEnabled(supportAction(AddAction));
    ui->deleteButton->setEnabled(supportAction(DeleteAction) && count > 0);

    d->showAction->setVisible(supportAction(ShowAction) && count == 1);
    d->editAction->setVisible(ui->editButton->isEnabled() && count == 1);
    d->deleteAction->setVisible(ui->deleteButton->isEnabled());

    emit actionSupportUpdated(ShowAction, d->showAction->isVisible());
    emit actionSupportUpdated(EditAction, d->editAction->isVisible());
    emit actionSupportUpdated(DeleteAction, d->deleteAction->isVisible());
}

DataInterfacePrivate::DataInterfacePrivate(DataInterface *q) :
    UserInterfacePrivate(q),
    uiInitialized(false),
    selectionInitialized(false),
    contextMenu(nullptr),
    showAction(nullptr),
    editAction(nullptr),
    deleteAction(nullptr),
    filterWidget(nullptr),
    editDialog(nullptr),
    model(nullptr)
{
}

void DataInterfacePrivate::createContextMenu()
{
    S_Q(DataInterface);

    contextMenu = new QMenu(q);
    showAction = contextMenu->addAction(QIcon(":/systemus/icons/action_show.png"), QString(), q, &DataInterface::showCurrentData);
    editAction = contextMenu->addAction(QIcon(":/systemus/icons/action_edit.png"), QString(), q, &DataInterface::editCurrentData);
    deleteAction = contextMenu->addAction(QIcon(":/systemus/icons/action_delete.png"), QString(), q, &DataInterface::deleteSelectedData);
}

}
