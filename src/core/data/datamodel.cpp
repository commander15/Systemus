#include "datamodel.h"
#include "datamodel_p.h"

#include <QtSql/qsqlquery.h>

namespace Systemus {

DataModel::DataModel(QObject *parent) :
    QSqlQueryModel(parent),
    d_ptr(new DataModelPrivate(this))
{
}

DataModel::~DataModel()
{
}

Data DataModel::model() const
{
    S_D(const DataModel);
    return d->data;
}

void DataModel::setModel(const Data &data)
{
    S_D(DataModel);
    d->data = data;
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
    setSort(column, order);
    select();
}

void DataModel::setSort(int column, Qt::SortOrder order)
{
    S_D(DataModel);
    d->setSort(column, order);
}

bool DataModel::select()
{
    S_D(DataModel);

    bool ok;
    setQuery(Data::execCachedQuery(selectStatement(), &ok));
    return ok;
}

QString DataModel::selectStatement() const
{
    S_D(const DataModel);

    QString statement = d->data.selectStatement();

    if (d->hasWhereClause())
        statement.append(' ' + d->whereClause());

    if (d->hasOrderByClause())
        statement.append(' ' + d->orderByClause());

    if (d->hasGroupByClause())
        statement.append(' ' + d->groupByClause());

    if (d->hasLimitOffsetClause())
        statement.append(' ' + d->limitOffsetClause());

    return statement;
}

DataModelPrivate::DataModelPrivate(DataModel *qq) :
    q(qq)
{
}

QString DataModelPrivate::filter() const
{
    return (!_filters.isEmpty() ? _filters.join(" AND ") : QString());
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
