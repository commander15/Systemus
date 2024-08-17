#ifndef SYSTEMUS_CORE_NAMESPACE_H
#define SYSTEMUS_CORE_NAMESPACE_H

#include <SystemusCore/global.h>

#ifdef SYSTEMUS_WIDGETS_LIB
#   include <SystemusWidgets/namespace.h>
#endif

class QSqlQuery;
class QSqlError;
class QSqlDriver;

namespace Systemus {

SYSTEMUS_CORE_EXPORT inline void init(int argc, char *argv[]);
SYSTEMUS_CORE_EXPORT void initCore(int argc, char *argv[]);

SYSTEMUS_CORE_EXPORT QSqlQuery exec(const QString &query, bool *ok = nullptr, QSqlError *error = nullptr);
SYSTEMUS_CORE_EXPORT QSqlQuery exec(const QString &query, const QVariantList &values, bool *ok = nullptr, QSqlError *error = nullptr);
SYSTEMUS_CORE_EXPORT QSqlQuery exec(const QString &query, const QVariantHash &values, bool *ok = nullptr, QSqlError *error = nullptr);
SYSTEMUS_CORE_EXPORT QSqlQuery execCached(const QString &query, bool *ok = nullptr, QSqlError *error = nullptr);
SYSTEMUS_CORE_EXPORT QSqlQuery execCached(const QString &query, const QVariantList &values, bool *ok = nullptr, QSqlError *error = nullptr);
SYSTEMUS_CORE_EXPORT QSqlQuery execCached(const QString &query, const QVariantHash &values, bool *ok = nullptr, QSqlError *error = nullptr);
SYSTEMUS_CORE_EXPORT QSqlQuery execQuery(const QString &query, bool cached, bool *ok = nullptr, QSqlError *error = nullptr);
SYSTEMUS_CORE_EXPORT QSqlQuery execQuery(const QString &query, const QVariantList &values, bool cached, bool *ok = nullptr, QSqlError *error = nullptr);
SYSTEMUS_CORE_EXPORT QSqlQuery execQuery(const QString &query, const QVariantHash &values, bool cached, bool *ok = nullptr, QSqlError *error = nullptr);
SYSTEMUS_CORE_EXPORT void execQuery(QSqlQuery &query, bool cache, bool *ok = nullptr, QSqlError *error = nullptr);

SYSTEMUS_CORE_EXPORT QSqlDriver *sqlDriver();

inline void init(int argc, char *argv[])
{
    initCore(argc, argv);
#ifdef SYSTEMUS_WIDGETS_LIB
    initWidgets(int argc, char *argv[]);
#endif
}

}

#endif // SYSTEMUS_CORE_NAMESPACE_H
