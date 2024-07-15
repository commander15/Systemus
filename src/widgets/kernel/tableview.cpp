#include "tableview.h"

#include <SystemusCore/datamodel.h>

namespace Systemus {

TableView::TableView(QWidget *parent) :
    QTableView(parent)
{
    setSelectionMode(MultiSelection);
    setSelectionBehavior(SelectRows);
    setEditTriggers(NoEditTriggers);

    connect(this, &QAbstractItemView::doubleClicked, this, &TableView::processDoubleClick);
}

TableView::~TableView()
{
}

Data TableView::currentData() const
{
    DataModel *model = this->model();
    if (!model)
        return Data();

    return model->data(currentIndex().row());
}

QList<Data> TableView::selectedData() const
{
    QList<Data> data;

    const QModelIndexList indexes = selectedIndexes();
    for (const QModelIndex &index : indexes)
        data.append(model()->data(index.row()));

    return data;
}

Data TableView::dataAt(const QPoint &pos) const
{
    DataModel *model = this->model();
    if (!model)
        return Data();

    return model->data(indexAt(pos).row());
}

DataModel *TableView::model() const
{
    return static_cast<DataModel *>(QTableView::model());
}

void TableView::setModel(DataModel *model)
{
    QTableView::setModel(model);
}

void TableView::processDoubleClick(const QModelIndex &index)
{
    emit dataDoubleClicked(model()->data(index.row()));
}

}
