#include "datamodel.h"
#include "datamodel_p.h"

#include <QtSql/qsqlquery.h>

namespace Systemus {

DataModel::DataModel(QObject *parent) :
    QAbstractTableModel(parent),
    d_ptr(new DataModelPrivate(this))
{
}

DataModel::~DataModel()
{
}

Data DataModel::data(int row) const
{
    S_D(const DataModel);
    return d->data.at(row);
}

DataInfo DataModel::dataInfo() const
{
    S_D(const DataModel);
    return d->info;
}

void DataModel::setDataInfo(const DataInfo &info)
{
    S_D(DataModel);
    d->info = info;
}

bool DataModel::search(const QString &query)
{
    setSearchQuery(query);
    return select();
}

void DataModel::setSearchQuery(const QString &query)
{
    qDebug() << Q_FUNC_INFO << ": unimplemented !";
}

QString DataModel::filter() const
{
    S_D(const DataModel);
    return d->filter();
}

void DataModel::setFilter(const QString &filter)
{
    S_D(DataModel);
    d->setFilter(filter);
}

void DataModel::sort(int column, Qt::SortOrder order)
{
    S_D(DataModel);
    d->setSort(column, order);
    select();
}

void DataModel::setSort(int column, Qt::SortOrder order)
{
    S_D(DataModel);
    d->setSort(column, order);
}

QSqlError DataModel::lastError() const
{
    S_D(const DataModel);
    return d->lastError;
}

bool DataModel::select()
{
    beginResetModel();

    S_D(DataModel);
    d->data = d->info.find(*d, &d->lastError);

    endResetModel();
    return !d->lastError.isValid();
}

bool DataModel::selectRow(int row)
{
    S_D(DataModel);

    const Data data = d->data.at(row);

    DataSearch query = *d;
    query.filter(QStringLiteral("%1 = %2").arg(d->info.idFieldName()).arg(data.id()));

    QList<Data> selectedData = d->info.find(query, &d->lastError);
    if (!data.isEmpty()) {
        d->data.replace(row, selectedData.constFirst());
        emit dataChanged(index(row, 0), index(row, d->info.displayCount()));
    }

    return !d->lastError.isValid();
}

void DataModel::clear()
{
    beginResetModel();

    S_D(DataModel);
    d->info = DataInfo();
    d->data.clear();
    d->clear();

    endResetModel();
}

QVariant DataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    S_D(const DataModel);

    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
        return (orientation == Qt::Horizontal ? d->columnName(section) : QString::number(d->itemNumber(section)));

    default:
        return QAbstractItemModel::headerData(section, orientation, role);
    }
}

bool DataModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    S_D(DataModel);

    if (orientation == Qt::Horizontal && (role == Qt::DisplayRole || role == Qt::EditRole)) {
        d->headerData.insert(section, value.toString());
        emit headerDataChanged(orientation, section, section);
        return true;
    } else
        return QAbstractTableModel::setHeaderData(section, orientation, role);
}

QVariant DataModel::data(const QModelIndex &index, int role) const
{
    S_D(const DataModel);
    const Data data = d->data.at(index.row());
    const QString property = d->info.displayPropertyName(index.column());

    switch(role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
        return data.property(property).toString();

    default:
        return QVariant();
    }
}

bool DataModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    S_D(DataModel);

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        const int row = index.row();
        d->data[row].setProperty(d->info.displayPropertyName(row), value);
        return true;
    } else
        return QAbstractTableModel::setData(index, value, role);
}

int DataModel::rowCount(const QModelIndex &parent) const
{
    S_D(const DataModel);
    return (true ? d->data.count() : 0);
}

int DataModel::columnCount(const QModelIndex &parent) const
{
    S_D(const DataModel);
    return (true ? d->info.displayCount() : 0);
}

DataModelPrivate::DataModelPrivate(DataModel *qq) :
    q(qq)
{
}

DataModelPrivate::~DataModelPrivate()
{
}

QString DataModelPrivate::columnName(int index) const
{
    if (headerData.contains(index))
        return headerData.value(index);
    else
        return info.displayPropertyName(index);
}

int DataModelPrivate::itemNumber(int index) const
{
    return (_page > 1 ? ((_page - 1) * _itemsPerPage) : 1) + index;
}

QString DataModelPrivate::filter() const
{
    return (!_filters.isEmpty() ? _filters.constFirst() : QString());
}

void DataModelPrivate::setFilter(const QString &filter)
{
    if (!_filters.isEmpty())
        _filters[0] = filter;
    else
        _filters.append(filter);
}

void DataModelPrivate::setSort(int column, Qt::SortOrder order)
{
    if (!_sortFields.isEmpty()) {
        _sortFields[0] = QString::number(column);
        _sortOrders[0] = order;
    } else {
        _sortFields.append(QString::number(column));
        _sortOrders.append(order);
    }
}

}
