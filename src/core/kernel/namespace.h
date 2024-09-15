#ifndef SYSTEMUS_CORE_NAMESPACE_H
#define SYSTEMUS_CORE_NAMESPACE_H

#include <SystemusCore/global.h>
#include <QtCore/qstring.h>

#ifdef SYSTEMUS_GUI_LIB
#   include <SystemusGui/namespace.h>
#   include <QtGui/qguiapplication.h>
#endif

#ifdef SYSTEMUS_WIDGETS_LIB
#   include <SystemusWidgets/namespace.h>
#   include <QtWidgets/qapplication.h>
#endif

class QCoreApplication;
class QSettings;

class QSqlQuery;
class QSqlRecord;
class QSqlError;
class QSqlDriver;

namespace Systemus {

SYSTEMUS_CORE_EXPORT inline void init(QCoreApplication &app, const QString &dbConnection = QString());
SYSTEMUS_CORE_EXPORT inline void init(QCoreApplication &app, QSettings &settings, const QString &dbConnection = QString());
SYSTEMUS_CORE_EXPORT void initCore(QCoreApplication *app, QSettings *settings, const QString &dbConnection);

SYSTEMUS_CORE_EXPORT bool beginTransaction();
SYSTEMUS_CORE_EXPORT bool commitTransaction();
SYSTEMUS_CORE_EXPORT bool rollbackTransaction();

SYSTEMUS_CORE_EXPORT QString sqlStatement(int type, const QString &tableName, const QSqlRecord &record, bool prepared = false);

SYSTEMUS_CORE_EXPORT QString escapeTableName(const QString &tableName);
SYSTEMUS_CORE_EXPORT QString escapeFieldName(const QString &fieldName);
SYSTEMUS_CORE_EXPORT QString escapeIdentifier(const QString &identifier, int type);
SYSTEMUS_CORE_EXPORT QString formatValue(const QVariant &value);

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

inline void init(QCoreApplication &app, const QString &dbConnection)
{
    initCore(&app, nullptr, dbConnection);

#ifdef SYSTEMUS_GUI_LIB
    initGui(static_cast<QGuiApplication &>(app));
#endif

#ifdef SYSTEMUS_WIDGETS_LIB
    initWidgets(static_cast<QApplication &>(app));
#endif
}

inline void init(QCoreApplication &app, QSettings &settings, const QString &dbConnection)
{
    initCore(&app, &settings, dbConnection);

#ifdef SYSTEMUS_GUI_LIB
    initGui(static_cast<QGuiApplication &>(app));
#endif

#ifdef SYSTEMUS_WIDGETS_LIB
    initWidgets(static_cast<QApplication &>(app));
#endif
}

}

#endif // SYSTEMUS_CORE_NAMESPACE_H
