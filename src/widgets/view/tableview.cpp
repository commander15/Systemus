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

    const QItemSelectionModel *model = selectionModel();
    if (model) {
        const QModelIndexList indexes = model->selectedRows();
        for (const QModelIndex &index : indexes)
            data.append(this->model()->item(index.row()));
    }

    return data;
}

QList<int> TableView::selectedRows() const
{
    QList<int> rows;

    const QItemSelectionModel *model = selectionModel();
    if (model) {
        const QModelIndexList indexes = model->selectedRows();
        for (const QModelIndex &index : indexes)
            rows.append(index.row());
    }

    return rows;
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

void TableView::setModel(QAbstractItemModel *model)
{
    if (model->inherits("Systemus::AbstractDataModel"))
        QTableView::setModel(model);
    else
        QTableView::setModel(nullptr);
}

void TableView::processDoubleClick(const QModelIndex &index)
{
    emit dataDoubleClicked(model()->item(index.row()));
}

}
