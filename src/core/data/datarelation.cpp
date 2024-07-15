#include "datarelation.h"

#include <QtSql/qsqldriver.h>

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
    return get(*primary);
}

bool AbstractDataRelation::get(const Data &primary)
{
    bool ok;
    QSqlQuery query = primary.exec(selectStatement(primary), &ok);

    if (ok) {
        clear();
        while (query.next())
            processRecord(query.record());
        return true;
    } else
        return false;
}

QString AbstractDataRelation::generateSelectStatement(const QString &table, const QSqlRecord &record) const
{
    return generateSelectStatement(table, record, QString());
}

QString AbstractDataRelation::generateSelectStatement(const QString &table, const QSqlRecord &record, const QString &filter) const
{
    QString statement = Data::driver()->sqlStatement(QSqlDriver::SelectStatement, table, record, false);

    if (!filter.isEmpty())
        statement.append(" WHERE " + filter);

    return statement;
}

}
