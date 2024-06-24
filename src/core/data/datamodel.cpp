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

QString DataModel::tableName() const
{
    S_D(const DataModel);
    return d->tableName;
}

void DataModel::setTableName(const QString &name)
{
    S_D(DataModel);
    d->tableName = name;
}

int DataModel::addField(const QString &name, QMetaType::Type type, const QString &table)
{
    S_D(DataModel);
    d->tableRecord.append(QSqlField(name, QMetaType(type), table));
    return d->tableRecord.count() - 1;
}

void DataModel::insertField(int pos, const QString &name, QMetaType::Type type, const QString &table)
{
    S_D(DataModel);
    d->tableRecord.insert(pos, QSqlField(name, QMetaType(type), table));
}

void DataModel::replaceField(int pos, const QString &name, QMetaType::Type type, const QString &table)
{
    S_D(DataModel);
    d->tableRecord.replace(pos, QSqlField(name, QMetaType(type), table));
}

int DataModel::addRelation(const QString &table)
{
    return addRelation(table, "id", table + "_id");
}

int DataModel::addRelation(const QString &table, const QString &foreignIndex)
{
    return addRelation(table, "id", foreignIndex);
}

int DataModel::addRelation(const QString &table, const QString &index, const QString &foreignIndex)
{
    S_D(DataModel);
    d->tableRelations.insert(foreignIndex, QSqlRelation(table, index, QString()));
    return d->tableRelations.size() - 1;
}

void DataModel::setTable(const QString &table)
{
    clear();

    S_D(DataModel);
    d->tableName = table;
    d->tableRecord = QSqlDatabase::database().record(table);
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

void DataModel::clear()
{
    S_D(DataModel);
    d->tableName.clear();
    d->tableRecord.clear();
    d->tableRelations.clear();
    QSqlQueryModel::clear();
}

QString DataModel::selectStatement() const
{
    S_D(const DataModel);

    QString statement;

    {
        QStringList fields;

        for (int i(0); i < d->tableRecord.count(); ++i) {
            const QSqlField field = d->tableRecord.field(i);

            QString name;
            if (!field.tableName().isEmpty())
                name.append(field.tableName());
            else
                name.append(d->tableName);
            name.append('.' + field.name());

            fields.append(name);
        }

        fields.append(d->tableRelations.keys());

        statement.append("SELECT " + fields.join(", ") + " FROM " + d->tableName);
    }

    const QStringList fields = d->tableRelations.keys();
    for (const QString &field : fields) {
        const QSqlRelation relation = d->tableRelations.value(field);
        statement.append(QStringLiteral(" LEFT JOIN %1 ON (%1.%2 = %3)").arg(relation.tableName(), relation.indexColumn(), field));
    }

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

DataModelPrivate::~DataModelPrivate()
{
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
