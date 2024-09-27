#include "datatablemodel.h"
#include "datatablemodel_p.h"

#include <SystemusCore/namespace.h>
#include <SystemusCore/ormbackend.h>
#include <SystemusCore/metamapper.h>

#include <QtCore/qmetaobject.h>

namespace Systemus {
namespace Orm {

DataTableModel::DataTableModel(QObject *parent) :
    QSqlQueryModel(parent),
    d_ptr(new DataTableModelPrivate(this))
{
}

DataTableModel::~DataTableModel()
{
}

QVariant DataTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical && role == Qt::DisplayRole) {
        return ((d_ptr->page() - 1) * d_ptr->itemsPerPage()) + (section + 1);
    } else {
        return QSqlQueryModel::headerData(section, orientation, role);
    }
}

Systemus::Data DataTableModel::item() const
{
    void *rawItem = d_ptr->table.newData();
    return (rawItem ? static_cast<Orm::Data *>(rawItem) : Systemus::Data());
}

Systemus::Data DataTableModel::item(int row) const
{
    Systemus::Data data = *d_ptr->item(row);
    data.setModelRecord(record(row));
    return data;
}

QByteArray DataTableModel::className() const
{
    return d_ptr->table.metaObject()->className();
}

const QMetaObject *DataTableModel::classMetaObject() const
{
    return d_ptr->table.metaObject();
}

QMetaType DataTableModel::classMetaType() const
{
    return d_ptr->table.metaType();
}

QString DataTableModel::classTableName() const
{
    return d_ptr->table.tableName();
}

QSqlRecord DataTableModel::classRecord() const
{
    return d_ptr->table.record();
}

MetaTable DataTableModel::classTable() const
{
    return d_ptr->table;
}

void DataTableModel::setClass(const Data &data)
{
    d_ptr->table = data.metaTable();
}

void DataTableModel::setClass(const QByteArray &className)
{
    d_ptr->table = MetaTable::fromClassName(className);
}

void DataTableModel::setClass(const MetaTable &table)
{
    d_ptr->table = table;
}

QStringList DataTableModel::properties() const
{
    return d_ptr->properties;
}

void DataTableModel::setProperties(const QStringList &properties)
{
    d_ptr->properties = properties;
}

QString DataTableModel::filter() const
{
    return d_ptr->filter();
}

void DataTableModel::setFilter(const QString &filter)
{
    d_ptr->setFilter(filter);
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

int DataTableModel::page() const
{
    return d_ptr->page();
}

void DataTableModel::setPage(int page)
{
    d_ptr->setPage(page);
}

int DataTableModel::pageCount() const
{
    return d_ptr->pageCount();
}

int DataTableModel::itemsPerPage() const
{
    return d_ptr->itemsPerPage();
}

void DataTableModel::setItemsPerPage(int items)
{
    d_ptr->setItemsPerPage(items);
}

bool DataTableModel::select()
{
    bool ok;

    QSqlQuery query = Systemus::exec(selectStatement(CountStatement), &ok);
    if (ok && query.next())
        d_ptr->setItemCount(query.value(0).toInt());

    QSqlQueryModel::setQuery(Systemus::execCached(selectStatement(DataStatement), &ok));

    return ok;
}

void DataTableModel::clear()
{
    QSqlQueryModel::clear();
    d_ptr->clearItems();
}

QStringList DataTableModel::linkedClassNames() const
{
    QStringList names;
    for (const DataTableModelLink &link : d_ptr->links)
        names.append(link.className);
    return names;
}

void DataTableModel::linkTo(const QString &className, LinkMode mode)
{
    DataTableModelLink link;
    link.className = className;
    link.linkMode = mode;
    d_ptr->links.append(link);
}

void DataTableModel::linkTo(const QString &className, const QString &foreignProperty, LinkMode mode)
{
    DataTableModelLink link;
    link.className = className;
    link.foreignProperty = foreignProperty;
    link.linkMode = mode;
    d_ptr->links.append(link);
}

void DataTableModel::linkTo(const QString &className, const QString &foreignProperty, const QString &indexProperty, LinkMode mode)
{
    DataTableModelLink link;
    link.className = className;
    link.foreignProperty = foreignProperty;
    link.indexProperty = indexProperty;
    link.linkMode = mode;
    d_ptr->links.append(link);
}

QString DataTableModel::selectStatement(StatementType type) const
{
    QString statement;

    statement.append("SELECT");
    if (type == DataStatement) {
        if (!d_ptr->properties.isEmpty()) {
            QStringList fieldNames;
            fieldNames.resize(d_ptr->properties.size());
            std::transform(d_ptr->properties.begin(), d_ptr->properties.end(), fieldNames.begin(), [this](const QString &propertyName) {
                const MetaMapper::MapOptions options = (!d_ptr->links.isEmpty() ? MetaMapper::IncludeTableName : MetaMapper::NoFieldMapOption);
                return MetaMapper::fieldName(propertyName, d_ptr->table, options | MetaMapper::EscapeIdentifiers);
            });
            statement.append(' ' + fieldNames.join(", "));
        } else {
            statement.append(" *");
        }
    } else {
        statement.append(" COUNT(0)");
    }
    statement.append(" FROM " + escapeTableName(d_ptr->table.tableName()));

    const QString join = QStringLiteral(" %1 JOIN %2 ON (%3 = %4)");
    const MetaMapper::MapOptions mapOptions(MetaMapper::EscapeIdentifiers|MetaMapper::IncludeTableName);
    for (const DataTableModelLink &link : d_ptr->links) {
        const MetaTable indexTable = MetaTable::fromClassName(link.className);

        QString indexProperty;
        if (!link.indexProperty.isEmpty())
            indexProperty = link.indexProperty;
        else
            indexProperty = indexTable.primaryPropertyName();

        QString foreignProperty;
        if (!link.foreignProperty.isEmpty())
            foreignProperty = link.foreignProperty;
        else
            foreignProperty = d_ptr->table.foreignPropertyName();

        QString currentJoin;
        switch (link.linkMode) {
        case InnerLink:
            currentJoin = join.arg("INNER");
            break;

        case LeftLink:
            currentJoin = join.arg("LEFT");
            break;

        case RightLink:
            currentJoin = join.arg("RIGHT");
            break;
        }

        currentJoin = currentJoin.arg(MetaMapper::tableName(indexTable, mapOptions));
        currentJoin = currentJoin.arg(MetaMapper::fieldName(indexProperty, indexTable, mapOptions));
        currentJoin = currentJoin.arg(MetaMapper::fieldName(foreignProperty, d_ptr->table, mapOptions));

        statement.append(currentJoin);
    }

    d_ptr->context(d_ptr->table.className());

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

void DataTableModel::queryChange()
{
    d_ptr->clearItems();
}

DataTableModelPrivate::DataTableModelPrivate(DataTableModel *q) :
    q_ptr(q),
    m_totalCount(0)
{
}

Data *DataTableModelPrivate::item(int index) const
{
    Data *data = nullptr;

    if (!m_items.contains(index)) {
        const QMetaType type = table.metaType();
        if (type.isValid()) {
            data = static_cast<Data *>(type.create());
            data->fill(q_ptr->record(index));
            m_items.insert(index, data);
        }
    } else {
        data = m_items.value(index);

        if (data->toSqlRecord() != q_ptr->record(index)) {
            data->fill(q_ptr->record(index));
            data->postGet();
        }
    }

    return data;
}

void DataTableModelPrivate::clearItems()
{
    const QList<int> indexes = m_items.keys();
    for (int index : indexes)
        delete m_items.take(index);
}

QString DataTableModelPrivate::filter() const
{
    return (!m_filters.isEmpty() ? m_filters.constFirst() : QString());
}

void DataTableModelPrivate::setFilter(const QString &filter)
{
    if (!m_filters.isEmpty()) {
        if (!filter.isEmpty())
            m_filters.replace(0, filter);
        else
            m_filters.remove(0);
    } else if (!filter.isEmpty())
        m_filters.append(filter);
}

int DataTableModelPrivate::sortColumn() const
{
    return -1;
}

Qt::SortOrder DataTableModelPrivate::sortOrder() const
{
    return (!m_sortOrders.isEmpty() ? m_sortOrders.first() : Qt::DescendingOrder);
}

void DataTableModelPrivate::setSort(int column, Qt::SortOrder order)
{
    if (m_sortProperties.isEmpty()) {
        m_sortProperties.append(propertyName(column));
        m_sortOrders.append(order);
    } else {
        m_sortProperties.replace(0, propertyName(column));
        m_sortOrders.replace(0, order);
    }
}

int DataTableModelPrivate::page() const
{
    return m_page;
}

void DataTableModelPrivate::setPage(int page)
{
    m_page = (page > 0 ? page : 0);
}

int DataTableModelPrivate::pageCount() const
{
    if (m_totalCount <= 0)
        return 0;
    else if (m_totalCount <= m_itemsPerPage)
        return 1;
    else
        return qCeil(double(m_totalCount) / m_itemsPerPage);
}

void DataTableModelPrivate::setItemCount(int count)
{
    m_totalCount = count;
}

int DataTableModelPrivate::itemsPerPage() const
{
    return m_itemsPerPage;
}

void DataTableModelPrivate::setItemsPerPage(int items)
{
    m_itemsPerPage = items;
}

QString DataTableModelPrivate::propertyName(int index) const
{
    if (!properties.isEmpty())
        return (index < properties.size() ? properties.at(index) : QString());
    else
        return table.className() + '.' + table.propertyName(index);
}

}
}
