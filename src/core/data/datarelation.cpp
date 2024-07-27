#include "datarelation.h"

#include <QtSql/qsqldriver.h>
#include <QtSql/qsqlerror.h>

#include <QtCore/qjsondocument.h>

namespace Systemus {

QString AbstractDataRelation::name() const
{
    return _name;
}

void AbstractDataRelation::setName(const QString &name)
{
    _name = name;
}

void AbstractDataRelation::dumpInfo() const
{
    const QJsonValue value = toJson();
    QJsonDocument doc;
    if (value.isObject())
        doc.setObject(value.toObject());
    else
        doc.setArray(value.toArray());
    qDebug().noquote() << doc.toJson(QJsonDocument::Indented);
}

bool AbstractDataRelation::get(const Data *primary)
{
    return getAllData(*primary, QString(), QString());
}

bool AbstractDataRelation::get(const Data *primary, const QString &foreignProperty)
{
    return getAllData(*primary, foreignProperty, QString());
}

bool AbstractDataRelation::get(const Data *primary, const QString &foreignProperty, const QString &indexProperty)
{
    return getAllData(*primary, foreignProperty, indexProperty);
}

bool AbstractDataRelation::get(const Data &primary)
{
    return getAllData(primary, QString(), QString());
}

bool AbstractDataRelation::get(const Data &primary, const QString &foreignProperty)
{
    return getAllData(primary, foreignProperty, QString());
}

bool AbstractDataRelation::get(const Data &primary, const QString &foreignProperty, const QString &indexProperty)
{
    return getAllData(primary, foreignProperty, indexProperty);
}

bool AbstractDataRelation::insert(Data *primary)
{
    return insertAllData(primary, QString(), QString());
}

bool AbstractDataRelation::insert(Data *primary, const QString &foreignProperty)
{
    return insertAllData(primary, foreignProperty, QString());
}

bool AbstractDataRelation::insert(Data *primary, const QString &foreignProperty, const QString &indexProperty)
{
    return insertAllData(primary, foreignProperty, indexProperty);
}

bool AbstractDataRelation::insert(Data &primary)
{
    return insertAllData(&primary, QString(), QString());
}

bool AbstractDataRelation::insert(Data &primary, const QString &foreignProperty)
{
    return insertAllData(&primary, foreignProperty, QString());
}

bool AbstractDataRelation::insert(Data &primary, const QString &foreignProperty, const QString &indexProperty)
{
    return insertAllData(&primary, foreignProperty, indexProperty);
}

bool AbstractDataRelation::update(Data *primary)
{
    return updateAllData(primary, QString(), QString());
}

bool AbstractDataRelation::update(Data *primary, const QString &foreignProperty)
{
    return updateAllData(primary, foreignProperty, QString());
}

bool AbstractDataRelation::update(Data *primary, const QString &foreignProperty, const QString &indexProperty)
{
    return updateAllData(primary, foreignProperty, indexProperty);
}

bool AbstractDataRelation::update(Data &primary)
{
    return updateAllData(&primary, QString(), QString());
}

bool AbstractDataRelation::update(Data &primary, const QString &foreignProperty)
{
    return updateAllData(&primary, foreignProperty, QString());
}

bool AbstractDataRelation::update(Data &primary, const QString &foreignProperty, const QString &indexProperty)
{
    return updateAllData(&primary, foreignProperty, indexProperty);
}

bool AbstractDataRelation::deleteData(Data *primary)
{
    return deleteAllData(primary, QString(), QString());
}

bool AbstractDataRelation::deleteData(Data *primary, const QString &foreignProperty)
{
    return deleteAllData(primary, foreignProperty, QString());
}

bool AbstractDataRelation::deleteData(Data *primary, const QString &foreignProperty, const QString &indexProperty)
{
    return deleteAllData(primary, foreignProperty, indexProperty);
}

bool AbstractDataRelation::deleteData(Data &primary)
{
    return deleteAllData(&primary, QString(), QString());
}

bool AbstractDataRelation::deleteData(Data &primary, const QString &foreignProperty)
{
    return deleteAllData(&primary, foreignProperty, QString());
}

bool AbstractDataRelation::deleteData(Data &primary, const QString &foreignProperty, const QString &indexProperty)
{
    return deleteAllData(&primary, foreignProperty, indexProperty);
}

QString AbstractDataRelation::generateSelectStatement(const QString &table, const QSqlRecord &record, const QString &filter) const
{
    QString statement = Data::driver()->sqlStatement(QSqlDriver::SelectStatement, table, record, false);

    if (!filter.isEmpty())
        statement.append(" WHERE " + filter);

    return statement;
}

QString AbstractDataRelation::generateInsertStatement(const QString &table, const QSqlRecord &record) const
{
    return Data::driver()->sqlStatement(QSqlDriver::InsertStatement, table, record, false);
}

QString AbstractDataRelation::generateUpdateStatement(const QString &table, const QSqlRecord &record, const QString &filter) const
{
    QString statement = Data::driver()->sqlStatement(QSqlDriver::UpdateStatement, table, record, false);

    if (!filter.isEmpty())
        statement.append(" WHERE " + filter);

    return statement;
}

QString AbstractDataRelation::generateDeleteStatement(const QString &table, const QString &filter) const
{
    QString statement = Data::driver()->sqlStatement(QSqlDriver::UpdateStatement, table, QSqlRecord(), false);

    if (!filter.isEmpty())
        statement.append(" WHERE " + filter);

    return statement;
}

QString AbstractDataRelation::formatedPropertyValue(const QString &property, const Data &data)
{
    return formatValue(data.property(property));
}

QString AbstractDataRelation::formatValue(const QVariant &value)
{
    QSqlField field(QString(), value.metaType());
    field.setValue(value);
    return Data::driver()->formatValue(field);
}

QSqlQuery AbstractDataRelation::exec(const QString &query, bool *ok, const Data &primary) const
{
    return primary.exec(query, ok);
}

QSqlQuery AbstractDataRelation::exec(const QString &query, bool *ok) const
{
    return Data::execQuery(query, ok);
}

}
