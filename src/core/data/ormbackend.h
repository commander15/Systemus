#ifndef SYSTEMUS_ORM_BACKEND_H
#define SYSTEMUS_ORM_BACKEND_H

#include <SystemusCore/global.h>

#include <QtCore/qscopedpointer.h>

class QMetaObject;
class QMetaProperty;

class QSqlIndex;
class QSqlRecord;
class QSqlField;

namespace Systemus {

class Data;

namespace Orm {

class SYSTEMUS_CORE_EXPORT Backend
{
public:
    enum StatementType {
        SelectStatement,
        UpdateStatement,
        InsertStatement,
        DeleteStatement
    };

    Backend();
    virtual ~Backend();

    virtual QString name() const = 0;

    QString statement(int type, const Data *object, const QMetaObject *metaObject, bool prepared = false) const;

    QString tableNameFromMetaObject(const QMetaObject *metaObject) const;
    QSqlRecord recordFromMetaObject(const QMetaObject *metaObject) const;
    QSqlField fieldFromMetaProperty(const QMetaProperty &property) const;

    virtual QList<QSqlField> fieldsFromString(const QString &str, const QString &className) const;
    virtual QSqlField fieldFromString(const QString &str, const QString &className) const;

    virtual QSqlField fieldFromVariant(const QString &propertyName, const QVariant &value) const;

    virtual QString tableNameFromClassName(const QString &className) const = 0;
    virtual QString classNameFromTableName(const QString &tableName) const = 0;

    virtual QString fieldNameFromPropertyName(const QString &propertyName) const = 0;
    virtual QString propertyNameFromFieldName(const QString &fieldName) const = 0;

    static Backend *instance();
    static Backend *defaultBackend();
};

}
}

#endif // SYSTEMUS_ORM_BACKEND_H
