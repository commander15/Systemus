#include "datarelation.h"

#include <QtSql/qsqldriver.h>
#include <QtSql/qsqlerror.h>

#include <QtCore/qjsondocument.h>

namespace Systemus {
namespace Orm {

void AbstractDataRelation::dump() const
{
    const QJsonValue value = toJson();
    QJsonDocument doc;
    if (value.isObject())
        doc.setObject(value.toObject());
    else
        doc.setArray(value.toArray());
    qInfo().noquote() << doc.toJson(QJsonDocument::Indented);
}

bool AbstractDataRelation::get(Data *primary)
{
    return getAllData(primary, foreignPropertyName(primary), indexPropertyName(primary));
}

bool AbstractDataRelation::get(Data *primary, const QString &foreignProperty)
{
    return getAllData(primary, foreignProperty, indexPropertyName(primary));
}

bool AbstractDataRelation::get(Data *primary, const QString &foreignProperty, const QString &indexProperty)
{
    return getAllData(primary, foreignProperty, indexProperty);
}

bool AbstractDataRelation::save(Data *primary)
{
    return saveAllData(primary, foreignPropertyName(primary), indexPropertyName(primary));
}

bool AbstractDataRelation::save(Data *primary, const QString &foreignProperty)
{
    return saveAllData(primary, foreignProperty, indexPropertyName(primary));
}

bool AbstractDataRelation::save(Data *primary, const QString &foreignProperty, const QString &indexProperty)
{
    return saveAllData(primary, foreignProperty, indexProperty);
}

bool AbstractDataRelation::update(Data *primary)
{
    return updateAllData(primary, foreignPropertyName(primary), indexPropertyName(primary));
}

bool AbstractDataRelation::update(Data *primary, const QString &foreignProperty)
{
    return updateAllData(primary, foreignProperty, indexPropertyName(primary));
}

bool AbstractDataRelation::update(Data *primary, const QString &foreignProperty, const QString &indexProperty)
{
    return updateAllData(primary, foreignProperty, indexProperty);
}

bool AbstractDataRelation::insert(Data *primary)
{
    return insertAllData(primary, foreignPropertyName(primary), indexPropertyName(primary));
}

bool AbstractDataRelation::insert(Data *primary, const QString &foreignProperty)
{
    return insertAllData(primary, foreignProperty, indexPropertyName(primary));
}

bool AbstractDataRelation::insert(Data *primary, const QString &foreignProperty, const QString &indexProperty)
{
    return insertAllData(primary, foreignProperty, indexProperty);
}

bool AbstractDataRelation::deleteData(Data *primary)
{
    return deleteAllData(primary, foreignPropertyName(primary), indexPropertyName(primary));
}

bool AbstractDataRelation::deleteData(Data *primary, const QString &foreignProperty)
{
    return deleteAllData(primary, foreignProperty, indexPropertyName(primary));
}

bool AbstractDataRelation::deleteData(Data *primary, const QString &foreignProperty, const QString &indexProperty)
{
    return deleteAllData(primary, foreignProperty, indexProperty);
}

QString AbstractDataRelation::foreignPropertyName(const Data *primary) const
{
    switch (relationshipType()) {
    case OneToOneRelationship:
    case OneToManyRelationship:
        return primary->metaTable().primaryPropertyName();

    case ManyToOneRelationship:
    case ManyToManyRelationship:
    default:
        return foreignTable().foreignPropertyName();
    }
}

QString AbstractDataRelation::foreignTableName() const
{
    return foreignTable().tableName();
}

MetaTable AbstractDataRelation::foreignTable() const
{
    return MetaTable::fromClassName(foreignClassName());
}

QString AbstractDataRelation::indexPropertyName(const Data *primary) const
{
    switch (relationshipType()) {
    case OneToOneRelationship:
    case OneToManyRelationship:
    case ManyToManyRelationship:
        return primary->metaTable().foreignPropertyName();

    case ManyToOneRelationship:
    default:
        return foreignTable().primaryPropertyName();
    }
}

bool AbstractDataRelation::getAllData(Data *primary, const QString &foreignProperty, const QString &indexProperty)
{
    const QString selectStatement = this->selectStatement(primary, foreignProperty, indexProperty);
    const QString whereStatement = this->whereClause(primary, foreignProperty, indexProperty);
    bool ok;

    QSqlQuery query = exec(selectStatement + ' ' + whereStatement, &ok, primary);
    if (ok) {
        clear();
        while (query.next())
            processReceivedRecord(query.record());
        return true;
    } else {
        return false;
    }
}

bool AbstractDataRelation::deleteAllData(Data *primary, const QString &foreignProperty, const QString &indexProperty)
{
    const QString deleteStatement = this->deleteStatement(primary);
    bool ok;

    exec(deleteStatement, &ok, primary);
    return ok;
}

QString AbstractDataRelation::selectStatement(Data *primary, const QString &foreignProperty, const QString &indexProperty) const
{
    const MetaTable foreignTable = this->foreignTable();

    QSqlRecord record = foreignTable.record();
    if (relationshipType() == ManyToManyRelationship) {
        QSqlField field = record.field(0);
        field.setName(field.tableName() + '.' + field.name());
        record.replace(0, field);
    }

    QString statement = Systemus::sqlStatement(QSqlDriver::SelectStatement, foreignTable.tableName(), record, false);
    const QString join = joinClause(primary, foreignProperty, indexProperty);
    if (!join.isEmpty())
        statement.append(' ' + join);

    return statement;
}

QString AbstractDataRelation::deleteStatement(Data *primary) const
{
    return Systemus::sqlStatement(QSqlDriver::DeleteStatement, foreignTableName(), QSqlRecord(), false);
}

QString AbstractDataRelation::joinClause(Data *primary, const QString &foreignProperty, const QString &indexProperty) const
{
    Q_UNUSED(primary);
    Q_UNUSED(foreignProperty);
    Q_UNUSED(indexProperty);
    return QString();
}

QString AbstractDataRelation::whereClause(Data *primary, const QString &foreignProperty, const QString &indexProperty) const
{
    return Systemus::sqlStatement(QSqlDriver::WhereStatement, filterTableName(primary), filterRecord(primary, foreignProperty, indexProperty));
}

QString AbstractDataRelation::filterTableName(Data *primary) const
{
    Q_UNUSED(primary);
    return foreignTableName();
}

QSqlRecord AbstractDataRelation::filterRecord(Data *primary, const QString &foreignProperty, const QString &indexProperty) const
{
    const MetaTable foreignTable = this->foreignTable();
    QSqlField field = foreignTable.primaryField();
    field.setName(Backend::instance()->fieldNameFromPropertyName(indexProperty, foreignClassName()));
    field.setValue(primary->property(foreignProperty));

    QSqlRecord record;
    record.append(field);
    return record;
}

QSqlQuery AbstractDataRelation::exec(const QString &query, bool *ok, Data *primary) const
{
    return primary->exec(query, ok);
}

}
}
