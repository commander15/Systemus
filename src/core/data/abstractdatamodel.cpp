#include "abstractdatamodel.h"
#include "abstractdatamodel_p.h"

#include <QtSql/qsqldriver.h>
#include <QtSql/qsqlfield.h>

namespace Systemus {

AbstractDataModel::AbstractDataModel(QObject *parent) :
    AbstractDataModel(new AbstractDataModelPrivate(this), parent)
{
}

AbstractDataModel::AbstractDataModel(AbstractDataModelPrivate *d, QObject *parent) :
    QAbstractItemModel(parent),
    d_ptr(d)
{
}

AbstractDataModel::~AbstractDataModel()
{
}

Data AbstractDataModel::item() const
{
    S_D(const AbstractDataModel);
    return d->info.newData();
}

Data AbstractDataModel::item(int row) const
{
    S_D(const AbstractDataModel);
    Data item = d->items.at(row);
    item.getExtras();
    return item;
}

QByteArray AbstractDataModel::className() const
{
    S_D(const AbstractDataModel);
    return d->info.metaObject()->className();
}

void AbstractDataModel::setClass(const QByteArray &className)
{
    S_D(AbstractDataModel);
    beginResetModel();
    d->info = DataInfo::fromName(className);
    d->items.clear();
    endResetModel();
}

void AbstractDataModel::setClass(const Data &data)
{
    S_D(AbstractDataModel);
    beginResetModel();
    d->info = data.dataInfo();
    d->items.clear();
    endResetModel();
}

void AbstractDataModel::setClass(const DataInfo &info)
{
    S_D(AbstractDataModel);
    beginResetModel();
    d->info = info;
    d->items.clear();
    endResetModel();
}

DataInfo AbstractDataModel::dataInfo() const
{
    S_D(const AbstractDataModel);
    return d->info;
}

QList<Data> AbstractDataModel::items() const
{
    S_D(const AbstractDataModel);
    return d->items;
}

void AbstractDataModel::setItems(const QList<Data> &items)
{
    S_D(AbstractDataModel);
    beginResetModel();
    d->items = items;
    endResetModel();
}

QString AbstractDataModel::searchQuery()
{
    S_D(const AbstractDataModel);
    return d->searchQuery();
}

void AbstractDataModel::setSearchQuery(const QString &query)
{
    S_D(AbstractDataModel);
    d->setSearchQuery(query);
}

bool AbstractDataModel::isMultiSearchEnabled() const
{
    S_D(const AbstractDataModel);
    return d->multiSearch;
}

void AbstractDataModel::setMultiSearchEnabled(bool enabled)
{
    S_D(AbstractDataModel);
    d->multiSearch = enabled;
}

QString AbstractDataModel::filter() const
{
    S_D(const AbstractDataModel);
    return d->filter();
}

void AbstractDataModel::setFilter(const QString &filter)
{
    S_D(AbstractDataModel);
    d->setFilter(filter);
}

void AbstractDataModel::sort(int column, Qt::SortOrder order)
{
    S_D(AbstractDataModel);
    d->setSort(column, order);
    select();
}

void AbstractDataModel::setSort(int column, Qt::SortOrder order)
{
    S_D(AbstractDataModel);
    d->setSort(column, order);
}

bool AbstractDataModel::select()
{
    S_D(AbstractDataModel);
    QSqlError error;

    beginResetModel();
    d->items = d->info.find(*d, false, &error);
    endResetModel();

    if (!error.isValid())
        return true;
    else {
        d->lastError = error;
        return false;
    }
}

bool AbstractDataModel::selectRow(int row)
{
    S_D(AbstractDataModel);

    Data *data = &d->items[row];
    if (data->get()) {
        emit dataChanged(index(row, 1), index(row, columnCount()));
        return true;
    } else {
        d->lastError = data->lastError();
        return false;
    }
}

QSqlError AbstractDataModel::lastError() const
{
    S_D(const AbstractDataModel);
    return d->lastError;
}

void AbstractDataModel::clear()
{
    beginResetModel();

    S_D(AbstractDataModel);
    d->items.clear();
    d->clear();

    endResetModel();
}

QModelIndex AbstractDataModel::index(int row, int column, const QModelIndex &parent) const
{
    S_D(const AbstractDataModel);
    if (row >= 0 && column >= 0)
        return createIndex(row, column, d->items[row].d_ptr.get());
    else
        return QModelIndex();
}

QModelIndex AbstractDataModel::parent(const QModelIndex &index) const
{
    return QModelIndex();
}

int AbstractDataModel::rowCount(const QModelIndex &parent) const
{
    S_D(const AbstractDataModel);
    return (!parent.isValid() ? d->items.count() : 0);
}

bool AbstractDataModel::isCustomized() const
{
    return false;
}

AbstractDataModelPrivate::AbstractDataModelPrivate(AbstractDataModel *qq) :
    q(qq),
    multiSearch(false),
    customFilterSet(false)
{
}

AbstractDataModelPrivate::~AbstractDataModelPrivate()
{
}

QString AbstractDataModelPrivate::searchQuery() const
{
    return _searchQuery;
}

void AbstractDataModelPrivate::setSearchQuery(const QString &query)
{
    if (!_searchQuery.isEmpty()) {
        if (!customFilterSet) {
            _filters.clear();
        } else {
            _filters.removeLast();
        }
    }

    _searchQuery = query;

    // We don't add search filters if the query is empty !
    if (query.isEmpty())
        return;

    QString formatedQuery;
    {
        QSqlField field(QString(), QMetaType::fromName("QString"));
        field.setValue(query + '%'),
        formatedQuery = Data::driver()->formatValue(field);
    }

    if (multiSearch) {
        // Searching using all search fields

        QStringList searchFilters;
        for (const QString &field : info.searchFieldNames()) {
            searchFilters.append(QStringLiteral("%1 LIKE %2").arg(field, formatedQuery));
        }

        _filters.append('(' + searchFilters.join(" OR ") + ')');
    } else {
        // Searching using the user field only

        _filters.append(QStringLiteral("%1 LIKE %2").arg(info.userFieldName(), formatedQuery));
    }
}

QString AbstractDataModelPrivate::filter() const
{
    return (!_filters.isEmpty() ? _filters.constFirst() : QString());
}

void AbstractDataModelPrivate::setFilter(const QString &filter)
{
    if (!customFilterSet) {
        _filters.append(filter);
        customFilterSet = true;
    } else if (filter.isEmpty()) {
        _filters.removeFirst();
        customFilterSet = false;
    } else {
        _filters.replace(0, filter);
    }
}

void AbstractDataModelPrivate::setSort(int column, Qt::SortOrder order)
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
