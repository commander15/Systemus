#include "ormdata.h"
#include "ormdata_p.h"

#include <SystemusCore/orm.h>
#include <SystemusCore/ormbackend.h>
#include <SystemusCore/metamapper.h>
#include <SystemusCore/private/debug_p.h>

#include <QtSql/qsqldatabase.h>
#include <QtSql/qsqlquery.h>
#include <QtSql/qsqlrecord.h>
#include <QtSql/qsqlfield.h>
#include <QtSql/qsqldriver.h>

#include <QtCore/qmetaobject.h>
#include <QtCore/qjsondocument.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>

namespace Systemus {
namespace Orm {

/*!
 * Constructs a default Data.
 */

Data::Data() :
    d_ptr(new DataPrivate())
{
}

Data::Data(DataPrivate *data) :
    d_ptr(data)
{
}

Data::Data(const QSharedDataPointer<DataPrivate> &data) :
    d_ptr(data)
{
}

/*!
 * Constructs a copy of other.
 */

Data::Data(const Data &other) :
    d_ptr(other.d_ptr)
{
}

Data::Data(Data &&other) :
    d_ptr(std::move(other.d_ptr))
{
}

Data::~Data()
{
}

Data &Data::operator=(const Data &other)
{
    if (this != &other && d_ptr != other.d_ptr)
        d_ptr = other.d_ptr;
    return *this;
}

bool Data::get()
{
    return getByFilter(QString(), true);
}

bool Data::getByPrimary(const QVariant &value, bool full)
{
    const QString property = metaTable().primaryPropertyName();
    const QString filter = QStringLiteral("{{ %1 }} = %2");
    return getByFilter(filter.arg(property, formatValue(value)), full);
}

bool Data::getByFilter(const QString &filter, bool full)
{
    S_D(Data);

    if ((full && !preGet()))
        return false;

    const MetaTable table = metaTable();
    QString statement = QueryBuilder::selectStatement(table) + ' ';
    if (!filter.isEmpty())
        statement.append("WHERE " + Orm::formatExpression(filter, className()));
    else
        statement.append(QueryBuilder::whereStatement(this, table));
    statement.append(QStringLiteral(" LIMIT 1"));

    bool ok;
    QSqlQuery query = execQuery(statement, &ok);

    if (ok && query.next()) {
        fill(query.record());
        return (full ? postGet() : true);
    } else {
        return false;
    }
}

bool Data::preGet()
{
    return true;
}

bool Data::postGet()
{
    return true;
}

bool Data::update()
{
    S_D(Data);

    if (!preUpdate())
        return false;

    const MetaTable table = metaTable();
    const QString statement = QueryBuilder::updateStatement(this, table) + ' ' + QueryBuilder::whereStatement(this, table);

    bool ok;
    execQuery(statement, &ok);

    return (ok ? postUpdate() : false);
}

bool Data::preUpdate()
{
    return true;
}

bool Data::postUpdate()
{
    return true;
}

bool Data::insert()
{
    S_D(Data);

    if (!preInsert())
        return false;

    const MetaTable table = metaTable();
    const QString statement = QueryBuilder::insertStatement(this, table);

    bool ok;
    QSqlQuery query = execQuery(statement, &ok);

    if (ok) {
        setPrimaryValue(query.lastInsertId());
        return postInsert();
    } else {
        return false;
    }
}

bool Data::preInsert()
{
    return true;
}

bool Data::postInsert()
{
    return true;
}

bool Data::deleteData()
{
    S_D(Data);

    if (!preDelete())
        return false;

    const MetaTable table = metaTable();
    const QString statement = QueryBuilder::deleteStatement(table) + ' ' + QueryBuilder::whereStatement(this, table);

    bool ok;
    execQuery(statement, &ok);

    return (ok ? postDelete() : false);
}

bool Data::preDelete()
{
    return true;
}

bool Data::postDelete()
{
    return true;
}

QSqlError Data::lastError() const
{
    S_D(const Data);
    return d->lastError;
}

QString Data::className() const
{
    S_D(const Data);
    return metaObject()->className();
}

const QMetaObject *Data::metaObject() const
{
    return &Data::staticMetaObject;
}

QMetaType Data::metaType() const
{
    return metaTable().metaType();
}

void Data::dump() const
{
    S_D(const Data);
    QJsonDocument doc;
    doc.setObject(toJsonObject());
    qInfo().noquote() << doc.toJson(QJsonDocument::Indented);
}

bool Data::operator==(const Data &other) const
{
    S_D(const Data);
    if (this == &other || d_ptr == other.d_ptr)
        return true;
    else
        return identicalTo(other);
}

void Data::initData()
{
}

void Data::registerData()
{
}

bool Data::isPropertyNull(const QString &name) const
{
    S_D(const Data);

    const QMetaObject *metaObject = this->metaObject();
    const int propertyIndex = metaObject->indexOfProperty(name.toStdString().c_str());
    if (propertyIndex >= 0)
        return metaObject->property(propertyIndex).readOnGadget(this).isNull();
    else
        return (d->properties.contains(name) ? d->properties.value(name).isNull() : true);
}

QVariant Data::readProperty(const QString &name) const
{
    S_D(const Data);

    const QMetaObject *metaObject = this->metaObject();
    const int propertyIndex = metaObject->indexOfProperty(name.toStdString().c_str());
    if (propertyIndex >= 0)
        return metaObject->property(propertyIndex).readOnGadget(this);
    else if (d->properties.contains(name))
        return d->properties.value(name);
    else
        return QVariant();
}

bool Data::writeProperty(const QString &name, const QVariant &value)
{
    S_D(Data);

    const QMetaObject *metaObject = this->metaObject();
    const int propertyIndex = metaObject->indexOfProperty(name.toStdString().c_str());
    if (propertyIndex >= 0) {
        return metaObject->property(propertyIndex).writeOnGadget(this, value);
    } else if (d->properties.contains(name) || metaTable().propertyNames().contains(name)) {
        d->properties.insert(name, value);
        return true;
    } else {
        return false;
    }
}

QSqlQuery Data::execQuery(const QString &query, bool *ok)
{
    S_D(Data);
    return Systemus::exec(query, ok, &d->lastError);
}

void Data::processError(const QSqlError &error)
{
    S_D(Data);
    d->lastError = error;
}

DataSearch::DataSearch()  :
    m_page(0), m_itemsPerPage(0)
{
}

DataSearch::DataSearch(const QString &className) :
    m_className(className), m_page(0), m_itemsPerPage(0)
{
}

DataSearch::DataSearch(const QString &filter, int page, int itemsPerPage) :
    m_page(0), m_itemsPerPage(0)
{
    this->filter(filter);
}

DataSearch &DataSearch::context(const QString &className)
{
    m_className = className;
    return *this;
}

DataSearch &DataSearch::filter(const QString &filter)
{
    if (!filter.isEmpty())
        m_filters.append(filter);
    return *this;
}

DataSearch &DataSearch::group(const QString property)
{
    if (!property.isEmpty())
        m_groupProperties.append(property);
    return *this;
}

DataSearch &DataSearch::sort(const QString &property, Qt::SortOrder order)
{
    if (!property.isEmpty()) {
        m_sortProperties.append(property);
        m_sortOrders.append(order);
    }
    return *this;
}

DataSearch &DataSearch::page(int page)
{
    m_page = (page > 0 ? page : 0);
    return *this;
}

DataSearch &DataSearch::paginate(int itemsPerPage)
{
    m_itemsPerPage = (itemsPerPage > 0 ? itemsPerPage : 0);
    return *this;
}

bool DataSearch::hasWhereClause() const
{
    return !m_filters.isEmpty();
}

QString DataSearch::whereClause(bool inclusive) const
{
    QStringList filters;
    filters.reserve(m_filters.size());
    std::transform(m_filters.begin(), m_filters.end(), std::back_inserter(filters), [this](const QString &filter) {
        return formatExpression(filter, m_className);
    });

    return (hasWhereClause() ? "WHERE " + filters.join(inclusive ? " AND " : " OR ") : QString());
}

bool DataSearch::hasGroupByClause() const
{
    return !m_groupProperties.isEmpty();
}

QString DataSearch::groupByClause() const
{
    QStringList fields;
    fields.reserve(m_groupProperties.size());
    std::transform(m_groupProperties.begin(), m_groupProperties.end(), std::back_inserter(fields), [this](const QString &property) {
        return MetaMapper::fieldName(property, m_className);
    });

    if (hasGroupByClause())
        return "GROUP BY " + fields.join(", ");
    else
        return QString();
}

bool DataSearch::hasOrderByClause() const
{
    return !m_sortProperties.isEmpty();
}

QString DataSearch::orderByClause() const
{
    if (!hasOrderByClause())
        return QString();

    QStringList clauses;
    for (int i(0); i < m_sortProperties.size(); ++i) {
        const QString field = MetaMapper::fieldName(m_sortProperties.at(i), m_className, MetaMapper::IncludeTableName|MetaMapper::EscapeIdentifiers);
        const QString order = (m_sortOrders.at(i) == Qt::AscendingOrder ? "ASC" : "DESC");
        clauses.append(field + ' ' + order);
    }

    return QStringLiteral("ORDER BY ") + clauses.join(", ");
}

bool DataSearch::hasLimitOffsetClause() const
{
    return m_page > 0 && m_itemsPerPage > 0;
}

QString DataSearch::limitOffsetClause() const
{
    if (hasLimitOffsetClause())
        return QStringLiteral("LIMIT %1 OFFSET %2").arg(m_itemsPerPage).arg((m_page - 1) * m_itemsPerPage);
    else
        return QString();
}

void DataSearch::clear()
{
    m_filters.clear();
    m_groupProperties.clear();
    m_sortProperties.clear();
    m_sortOrders.clear();
    m_page = 0;
    m_itemsPerPage = 100;
}

DataPrivate::DataPrivate() :
    initialized(false)
{
}

bool DataPrivate::isDefaultData() const
{
    return dataType() == DefaultDataType;
}

int DataPrivate::dataType() const
{
    return DefaultDataType;
}

DataPrivate *DataPrivate::clone() const
{
    return new DataPrivate(*this);
}

}
}

template<>
Systemus::Orm::DataPrivate *QSharedDataPointer<Systemus::Orm::DataPrivate>::clone()
{
    return d->clone();
}
