#ifndef SYSTEMUS_ORM_H
#define SYSTEMUS_ORM_H

#include <SystemusCore/global.h>

class QMetaObject;
class QMetaProperty;

class QSqlRecord;
class QSqlField;

namespace Systemus {

namespace Orm {
SYSTEMUS_CORE_EXPORT void init(int argc, char *argv[]);

SYSTEMUS_CORE_EXPORT QString selectStatement(const QMetaObject *object, bool prepared = false);
SYSTEMUS_CORE_EXPORT QString selectStatement(const QString &tableName, const QSqlRecord &record, bool prepared = false);

SYSTEMUS_CORE_EXPORT QString updateStatement(const QMetaObject *object, bool prepared = false);
SYSTEMUS_CORE_EXPORT QString updateStatement(const QString &tableName, const QSqlRecord &record, bool prepared = false);

SYSTEMUS_CORE_EXPORT QString insertStatement(const QMetaObject *object, bool prepared = false);
SYSTEMUS_CORE_EXPORT QString insertStatement(const QString &tableName, const QSqlRecord &record, bool prepared = false);

SYSTEMUS_CORE_EXPORT QString tableNameFromMetaObject(const QMetaObject *object);
SYSTEMUS_CORE_EXPORT QSqlRecord recordFromMetaObject(const QMetaObject *object);
SYSTEMUS_CORE_EXPORT QSqlField fieldFromMetaProperty(const QMetaProperty &property);

SYSTEMUS_CORE_EXPORT QString classNameFromTableName(const QString &tableName);
SYSTEMUS_CORE_EXPORT QString tableNameFromClassName(const QString &className);

SYSTEMUS_CORE_EXPORT QString fieldNameFromPropertyName(const QString &propertyName);
SYSTEMUS_CORE_EXPORT QString propertyNameFromFieldName(const QString &fieldName);

SYSTEMUS_CORE_EXPORT QList<QSqlField> fieldsFromString(const QString &str);
SYSTEMUS_CORE_EXPORT QSqlField fieldFromString(const QString &str);

SYSTEMUS_CORE_EXPORT QSqlField fieldFromVariant(const QVariant &value);

SYSTEMUS_CORE_EXPORT QString formatValue(const QVariant &value);
}

}

#endif // SYSTEMUS_ORM_H
