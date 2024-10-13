#include "querybuilder.h"

#include <SystemusCore/metatable.h>
#include <SystemusCore/ormdata.h>
#include <SystemusCore/private/debug_p.h>
#include <SystemusCore/private/namespace_p.h>

#include <QtSql/qsqldriver.h>
#include <QtSql/qsqlrecord.h>
#include <QtSql/qsqlfield.h>

namespace Systemus {
namespace Orm {

QString QueryBuilder::selectStatement(const QString &className)
{
    return selectStatement(MetaTable::fromClassName(className));
}

QString QueryBuilder::selectStatement(const MetaTable &table)
{
    if (!table.isValid()) {
        systemusWarning() << "Orm::QueryBuilder::selectStatement(): can't generate, invalid meta table provided";
        return QString();
    }

    QSqlDriver *driver = sqlDriver();
    if (driver)
        return driver->sqlStatement(QSqlDriver::SelectStatement, table.tableName(), table.record(), false);
    else
        return QString();
}

QString QueryBuilder::insertStatement(const Data *data)
{
    return insertStatement(data, data->metaTable());
}

QString QueryBuilder::insertStatement(const Data *data, const MetaTable &table)
{
    if (!table.isValid()) {
        systemusWarning() << "Orm::QueryBuilder::insertStatement(): can't generate, invalid meta table provided";
        return QString();
    }

    QSqlRecord record = data->toSqlRecord();
    record.remove(record.indexOf(table.primaryFieldName()));

    QSqlDriver *driver = sqlDriver();
    if (driver)
        return driver->sqlStatement(QSqlDriver::InsertStatement, table.tableName(), record, false);
    else
        return QString();
}

QString QueryBuilder::updateStatement(const Data *data)
{
    return updateStatement(data, data->metaTable());
}

QString QueryBuilder::updateStatement(const Data *data, const MetaTable &table)
{
    if (!table.isValid()) {
        systemusWarning() << "Orm::QueryBuilder::updateStatement(): can't generate, invalid meta table provided";
        return QString();
    }

    QSqlRecord record = data->toSqlRecord();
    record.remove(record.indexOf(table.primaryFieldName()));
    return sqlDriver()->sqlStatement(QSqlDriver::UpdateStatement, table.tableName(), record, false);
}

QString QueryBuilder::deleteStatement(const QString &className)
{
    return deleteStatement(MetaTable::fromClassName(className));
}

QString QueryBuilder::deleteStatement(const MetaTable &table)
{
    if (!table.isValid()) {
        systemusWarning() << "Orm::QueryBuilder::deleteStatement(): can't generate, invalid meta table provided";
        return QString();
    }


    QSqlDriver *driver = sqlDriver();
    if (driver)
        return driver->sqlStatement(QSqlDriver::DeleteStatement, table.tableName(), QSqlRecord(), false);
    else
        return QString();
}

QString QueryBuilder::whereStatement(const Data *data)
{
    return whereStatement(data->primaryValue(), data->metaTable());
}

QString QueryBuilder::whereStatement(const Data *data, const MetaTable &table)
{
    return whereStatement(data->primaryValue(), table);
}

QString QueryBuilder::whereStatement(const QVariant &primaryValue, const MetaTable &table)
{
    if (!table.isValid()) {
        systemusWarning() << "Orm::QueryBuilder::whereStatement(): can't generate, invalid meta table provided";
        return QString();
    }

    QSqlField field = table.primaryField();
    field.setValue(primaryValue);

    QSqlRecord record;
    record.append(field);

    QSqlDriver *driver = sqlDriver();
    if (driver)
        return driver->sqlStatement(QSqlDriver::WhereStatement, table.tableName(), record, false);
    else
        return QString();
}

QSqlDriver *QueryBuilder::sqlDriver()
{
    QSqlDriver *driver = QSqlDatabase::database(SystemusPrivate::dbConnection).driver();
    if (!driver)
        systemusWarning() << "Orm::QueryBuilder: can't process, database driver not found";
    return driver;
}

}
}
