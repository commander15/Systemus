#include "tableview.h"

#include <SystemusCore/abstractdatamodel.h>

#include <SystemusCore/private/data_p.h>

#include <QtWidgets/qheaderview.h>
#include <QtWidgets/qmenu.h>

#include <QtGui/qevent.h>

namespace Systemus {

TableView::TableView(QWidget *parent) :
    QTableView(parent)
{
    //setSelectionMode(MultiSelection);
    setSelectionBehavior(SelectRows);
    setEditTriggers(NoEditTriggers);

    connect(this, &QAbstractItemView::doubleClicked, this, &TableView::processDoubleClick);
}

TableView::~TableView()
{
}

Data TableView::currentData() const
{
    AbstractDataModel *model = this->model();
    if (!model)
        return Data();

    return model->item(currentIndex().row());
}

QList<Data> TableView::selectedData() const
{
    QList<Data> data;

    const QModelIndexList indexes = selectionModel()->selectedRows();
    for (const QModelIndex &index : indexes)
        data.append(model()->item(index.row()));

    return data;
}

Data TableView::dataAt(const QPoint &pos) const
{
    AbstractDataModel *model = this->model();
    if (!model)
        return Data();

    return model->item(indexAt(pos).row());
}

AbstractDataModel *TableView::model() const
{
    return static_cast<AbstractDataModel *>(QTableView::model());
}

void TableView::setModel(AbstractDataModel *model)
{
    if (this->model()) {
        disconnect(this->model(), &QAbstractItemModel::modelReset, this, &TableView::configureHeaders);
    }

    QTableView::setModel(model);

    if (model) {
        //configureHeaders();
        connect(this->model(), &QAbstractItemModel::modelReset, this, &TableView::configureHeaders);
    }
}

void TableView::configureHeaders()
{
    QHeaderView *header = horizontalHeader();
    for (int i(0); i < header->count(); ++i)
        header->setSectionResizeMode(i, QHeaderView::Stretch);
}

void TableView::processDoubleClick(const QModelIndex &index)
{
    emit dataDoubleClicked(model()->item(index.row()));
}

}
