#include "datatablemodel.h"
#include "datatablemodel_p.h"

#include <SystemusCore/namespace.h>

#include <QtCore/qmetaobject.h>

namespace Systemus {

DataTableModel::DataTableModel(QObject *parent) :
    QSqlQueryModel(parent),
    d_ptr(new DataTableModelPrivate(this))
{
}

DataTableModel::~DataTableModel()
{
}

Data DataTableModel::item() const
{
    return d_ptr->classInfo.newData();
}

Data DataTableModel::item(int row) const
{
    return *d_ptr->item(row);
}

QByteArray DataTableModel::className() const
{
    return d_ptr->classInfo.metaObject()->className();
}

const QMetaObject *DataTableModel::classMetaObject() const
{
    return d_ptr->classInfo.metaObject();
}

QSqlRecord DataTableModel::classRecord() const
{
    return d_ptr->classInfo.record();
}

DataInfo DataTableModel::classDataInfo() const
{
    return d_ptr->classInfo;
}

void DataTableModel::setClass(const Data &data)
{
    d_ptr->classInfo = data.info();
}

void DataTableModel::setClass(const QByteArray &className)
{
    d_ptr->classInfo = DataInfo::fromName(className);
}

void DataTableModel::setClass(const DataInfo &info)
{
    d_ptr->classInfo = info;
}

QStringList DataTableModel::properties() const
{
    return d_ptr->properties;
}

void DataTableModel::setProperties(const QStringList &properties)
{
    d_ptr->properties = properties;
}

QStringList DataTableModel::searchProperties() const
{
    return d_ptr->searchProperties;
}

void DataTableModel::setSearchProperties(const QStringList &properties)
{
    d_ptr->searchProperties = properties;
}

QString DataTableModel::filter() const
{
    return d_ptr->filter();
}

void DataTableModel::setFilter(const QString &filter)
{
    d_ptr->setFilter(filter);
}

void DataTableModel::search(const QString &query)
{
    d_ptr->setSearchQuery(query);
    select();
}

QString DataTableModel::searchQuery()
{
    return d_ptr->searchQuery();
}

void DataTableModel::setSearchQuery(const QString &query)
{
    d_ptr->setSearchQuery(query);
}

void DataTableModel::sort(int column, Qt::SortOrder order)
{
    d_ptr->setSort(column, order);
    select();
}

int DataTableModel::sortColumn() const
{
    return d_ptr->sortColumn();
}

Qt::SortOrder DataTableModel::sortOrder() const
{
    return d_ptr->sortOrder();
}

void DataTableModel::setSort(int column, Qt::SortOrder order)
{
    d_ptr->setSort(column, order);
}

bool DataTableModel::select()
{
    bool ok;
    QSqlQueryModel::setQuery(Data::execCachedQuery(selectStement(), &ok));
    return ok;
}

QByteArrayList DataTableModel::linkedClassNames() const
{
    QByteArrayList names;
    for (const DataTableModelLink &link : d_ptr->links)
        names.append(link.className);
    return names;
}

void DataTableModel::linkTo(const QByteArray &className)
{
    DataTableModelLink link;
    link.className = className;
    d_ptr->links.append(link);
}

void DataTableModel::linkTo(const QByteArray &className, const QString &foreignProperty)
{
    DataTableModelLink link;
    link.className = className;
    link.foreignProperty = foreignProperty;
    d_ptr->links.append(link);
}

void DataTableModel::linkTo(const QByteArray &className, const QString &foreignProperty, const QString &indexProperty)
{
    DataTableModelLink link;
    link.className = className;
    link.foreignProperty = foreignProperty;
    link.indexProperty = indexProperty;
    d_ptr->links.append(link);
}

Data *DataTableModel::itemPointer(int row) const
{
    return d_ptr->item(row);
}

QString DataTableModel::selectStement() const
{
    QString statement = "SELECT";

    const int propertyCount = d_ptr->properties.size();
    for (int i(0); i < propertyCount; ++i) {
        statement.append(' ' + (d_ptr->properties.at(i)));
        if (i < propertyCount - 1)
            statement.append(',');
    }

    const QString join = QStringLiteral("INNER JOIN %1 ON (%1.%2 = %3.%4)");
    for (const DataTableModelLink &link : d_ptr->links) {
        const DataInfo foreignInfo = DataInfo::fromName(link.className);

        QString foreignField;
        if (!link.foreignProperty.isEmpty())
            foreignField = fieldNameFromPropertyName(link.foreignProperty);
        else
            foreignField = foreignInfo.foreignFieldName();

        QString idField;
        if (!link.indexProperty.isEmpty())
            idField = fieldNameFromPropertyName(link.indexProperty);
        else
            idField = foreignInfo.idFieldName();

        statement.append(join.arg(foreignInfo.tableName()).arg(foreignField).arg(d_ptr->classInfo.tableName()).arg(idField));
    }

    if (d_ptr->hasWhereClause())
        statement.append(' ' + d_ptr->whereClause());

    if (d_ptr->hasGroupByClause())
        statement.append(' ' + d_ptr->groupByClause());

    if (d_ptr->hasOrderByClause())
        statement.append(' ' + d_ptr->orderByClause());

    if (d_ptr->hasLimitOffsetClause())
        statement.append(' ' + d_ptr->limitOffsetClause());

    return statement;
}

DataTableModelPrivate::DataTableModelPrivate(DataTableModel *q) :
    q_ptr(q)
{
}

Data *DataTableModelPrivate::item(int index) const
{
    if (!m_items.contains(index)) {
        const QMetaType type = QMetaType::fromName(classInfo.metaObject()->className());
        if (!type.isValid())
            return nullptr;

        Data *data = static_cast<Data *>(type.create());
        data->fill(q_ptr->record(index), true);
        return (m_items.insert(index, data) ? data : nullptr);
    } else {
        return m_items.object(index);
    }
}

}
