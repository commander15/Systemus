#include "tableview.h"

#include <QtWidgets/qheaderview.h>

namespace Systemus {

TableView::TableView(QWidget *parent) :
    QTableView(parent)
{
    setSelectionMode(MultiSelection);
    setSelectionBehavior(SelectRows);
    setEditTriggers(NoEditTriggers);
}

TableView::~TableView()
{
}

QSqlRecord TableView::currentRecord() const
{
    if (model())
        return model()->record(currentIndex().row());
    else
        return QSqlRecord();
}

QList<QSqlRecord> TableView::selectedRecords() const
{
    QList<QSqlRecord> records;

    const QModelIndexList indexes = selectedIndexes();
    for (const QModelIndex &index : indexes)
        records.append(model()->record(index.row()));

    return records;
}

QSqlRecord TableView::recordAt(const QPoint &pos) const
{
    if (model())
        return model()->record(indexAt(pos).row());
    else
        return QSqlRecord();
}

QSqlQueryModel *TableView::model() const
{
    QAbstractItemModel *model = QTableView::model();
    return (model ? static_cast<QSqlTableModel *>(model) : nullptr);
}

void TableView::setModel(QSqlQueryModel *model)
{
    QSqlQueryModel *oldModel = this->model();
    if (oldModel)
        disconnect(oldModel, &QAbstractItemModel::modelReset, this, &TableView::prepare);

    QTableView::setModel(model);
    if (model) {
        prepare();
        connect(model, &QAbstractItemModel::modelReset, this, &TableView::prepare);
    }
}

void TableView::prepare()
{
    hideColumn(0);

    QHeaderView *view = horizontalHeader();
    for (int i(1); i < view->count(); ++i)
        view->setSectionResizeMode(i, QHeaderView::Stretch);
}

}
