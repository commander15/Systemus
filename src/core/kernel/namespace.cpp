#include "namespace.h"
#include "namespace_p.h"

#include <SystemusCore/settings.h>
#include <SystemusCore/orm.h>
#include <SystemusCore/private/debug_p.h>

#include <QtSql/qsqlfield.h>
#include <QtSql/qsqlquery.h>
#include <QtSql/qsqlerror.h>
#include <QtSql/qsqldriver.h>

namespace Systemus {

void initCore(QCoreApplication *app, QSettings *settings, const QString &dbConnection)
{
    if (settings)
        Settings::setSettings("Systemus", settings);

    if (!dbConnection.isEmpty())
        SystemusPrivate::dbConnection = dbConnection;

    Orm::init();
    Q_UNUSED(app);
}

bool beginTransaction()
{
    return QSqlDatabase::database(SystemusPrivate::dbConnection).transaction();
}

bool commitTransaction()
{
    return QSqlDatabase::database(SystemusPrivate::dbConnection).commit();
}

bool rollbackTransaction()
{
    return QSqlDatabase::database(SystemusPrivate::dbConnection).rollback();
}

QString sqlStatement(int type, const QString &tableName, const QSqlRecord &record, bool prepared)
{
    return sqlDriver()->sqlStatement(static_cast<QSqlDriver::StatementType>(type), tableName, record, prepared);
}

QString escapeTableName(const QString &tableName)
{
    return sqlDriver()->escapeIdentifier(tableName, QSqlDriver::TableName);
}

QString escapeFieldName(const QString &fieldName)
{
    if (fieldName == '*')
        return fieldName;
    else
        return sqlDriver()->escapeIdentifier(fieldName, QSqlDriver::FieldName);
}

QString escapeIdentifier(const QString &identifier, int type)
{
    return sqlDriver()->escapeIdentifier(identifier, static_cast<QSqlDriver::IdentifierType>(type));
}

QString formatValue(const QVariant &value)
{
    QSqlField field(QString(), value.metaType());
    field.setValue(value);
    return sqlDriver()->formatValue(field, true);
}

QSqlQuery exec(const QString &query, bool *ok, QSqlError *error)
{
    return execQuery(query, false, ok, error);
}

QSqlQuery exec(const QString &query, const QVariantList &values, bool *ok, QSqlError *error)
{
    return execQuery(query, values, false, ok, error);
}

QSqlQuery exec(const QString &query, const QVariantHash &values, bool *ok, QSqlError *error)
{
    return execQuery(query, values, false, ok, error);
}

QSqlQuery execCached(const QString &query, bool *ok, QSqlError *error)
{
    return execQuery(query, true, ok, error);
}

QSqlQuery execCached(const QString &query, const QVariantList &values, bool *ok, QSqlError *error)
{
    return execQuery(query, values, true, ok, error);
}

QSqlQuery execCached(const QString &query, const QVariantHash &values, bool *ok, QSqlError *error)
{
    return execQuery(query, values, true, ok, error);
}

QSqlQuery execQuery(const QString &query, bool cache, bool *ok, QSqlError *error)
{
    QSqlQuery sqlQuery(QSqlDatabase::database(SystemusPrivate::dbConnection));
    sqlQuery.prepare(query);
    execQuery(sqlQuery, cache, ok, error);
    return sqlQuery;
}

QSqlQuery execQuery(const QString &query, const QVariantList &values, bool cache, bool *ok, QSqlError *error)
{
    QSqlQuery sqlQuery(QSqlDatabase::database(SystemusPrivate::dbConnection));
    sqlQuery.prepare(query);

    for (int i(0); i < values.size(); ++i)
        sqlQuery.bindValue(i, values.at(i));

    execQuery(sqlQuery, cache, ok, error);
    return sqlQuery;
}

QSqlQuery execQuery(const QString &query, const QVariantHash &values, bool cache, bool *ok, QSqlError *error)
{
    QSqlQuery sqlQuery(QSqlDatabase::database(SystemusPrivate::dbConnection));
    sqlQuery.prepare(query);

    const QStringList fields = values.keys();
    for (const QString &field : fields)
        sqlQuery.bindValue(field, values.value(field));

    execQuery(sqlQuery, cache, ok, error);
    return sqlQuery;
}

void execQuery(QSqlQuery &query, bool cache, bool *ok, QSqlError *error)
{
    query.setForwardOnly(!cache);

    const bool success = query.exec();
#ifdef QT_DEBUG
    systemusSqlInfo() << query.executedQuery();
#endif

    if (ok)
        *ok = success;

    if (!success) {
#ifdef QT_DEBUG
        systemusSqlWarning() << query.lastError().databaseText();
#endif
        if (error)
            *error = query.lastError();
    }
}

QSqlDriver *sqlDriver()
{
    return QSqlDatabase::database(SystemusPrivate::dbConnection).driver();
}


}
