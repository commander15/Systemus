#include "namespace.h"
#include "namespace_p.h"

#include <SystemusCore/orm.h>
#include <SystemusCore/system.h>
#include <SystemusCore/user.h>
#include <SystemusCore/role.h>
#include <SystemusCore/group.h>
#include <SystemusCore/privilege.h>
#include <SystemusCore/permission.h>

#include <QtSql/qsqlquery.h>
#include <QtSql/qsqlrecord.h>
#include <QtSql/qsqlfield.h>
#include <QtSql/qsqlerror.h>
#include <QtSql/qsqldriver.h>

#include <QtCore/qmetaobject.h>
#include <QtCore/qregularexpression.h>

namespace Systemus {

void initCore(int argc, char *argv[])
{
    Orm::init(argc, argv);
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

    if (query.exec()) {
        if (ok)
            *ok = true;
    } else {
        if (ok)
            *ok = false;

        if (error)
            *error = query.lastError();
    }
}

QSqlDriver *sqlDriver()
{
    return QSqlDatabase::database(SystemusPrivate::dbConnection).driver();
}


}
