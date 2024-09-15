#include "querybuilder.h"

#include <SystemusCore/metatable.h>
#include <SystemusCore/ormdata.h>
#include <SystemusCore/private/namespace_p.h>

#include <QtSql/qsqldriver.h>
#include <QtSql/qsqlrecord.h>
#include <QtSql/qsqlfield.h>

namespace Systemus {
namespace Orm {

QString QueryBuilder::selectStatement(const QMetaObject *metaObject)
{
    return selectStatement(MetaTable::fromClassName(metaObject->className()));
}

QString QueryBuilder::selectStatement(const QString &className)
{
    return selectStatement(MetaTable::fromClassName(className));
}

QString QueryBuilder::selectStatement(const MetaTable &table)
{
    return sqlDriver()->sqlStatement(QSqlDriver::SelectStatement, table.tableName(), table.record(), false);
}

QString QueryBuilder::insertStatement(const Data *data, const MetaTable &table)
{
    QSqlRecord record = data->toSqlRecord();
    record.remove(record.indexOf(table.primaryFieldName()));
    return sqlDriver()->sqlStatement(QSqlDriver::InsertStatement, table.tableName(), record, false);
}

QString QueryBuilder::updateStatement(const Data *data, const MetaTable &table)
{
    QSqlRecord record = data->toSqlRecord();
    record.remove(record.indexOf(table.primaryFieldName()));
    return sqlDriver()->sqlStatement(QSqlDriver::UpdateStatement, table.tableName(), record, false);
}

QString QueryBuilder::deleteStatement(const MetaTable &table)
{
    return sqlDriver()->sqlStatement(QSqlDriver::DeleteStatement, table.tableName(), QSqlRecord(), false);
}

QString QueryBuilder::whereStatement(const Data *data, const MetaTable &table)
{
    QSqlField field = table.primaryField();
    field.setValue(data->primaryValue());

    QSqlRecord record;
    record.append(field);

    return sqlDriver()->sqlStatement(QSqlDriver::WhereStatement, table.tableName(), record, false);
}

QSqlDriver *QueryBuilder::sqlDriver()
{
    return QSqlDatabase::database(SystemusPrivate::dbConnection).driver();
}

}
}
