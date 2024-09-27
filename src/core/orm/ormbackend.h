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

class SecretProperty;

class SYSTEMUS_CORE_EXPORT Backend
{
public:
    Backend();
    virtual ~Backend();

    virtual QString name() const = 0;

    // OO Side

    virtual QString classNameFromTableName(const QString &tableName) const = 0;
    QList<SecretProperty> secretPropertiesFromMetaObject(const QMetaObject *metaObject) const;
    QList<SecretProperty> secretPropertiesFromString(const QString &str, const QMetaObject *metaObject) const;
    QList<SecretProperty> searchPropertiesFromMetaObject(const QMetaObject *metaObject, QList<int> *metaIndexes = nullptr) const;
    QString foreignPropertyNameFromMetaObject(const QMetaObject *metaObject) const;
    virtual QString foreignPropertyNameFromPropertyName(const QString &propertyName, const QString &className) const = 0;
    virtual QString propertyNameFromFieldName(const QString &fieldName, const QString &table) const = 0;

    QString associationClassNameFromMetaObjects(const QMetaObject *o1, const QMetaObject *o2) const;
    virtual QString associationClassNameFromClassNames(const QString &c1, const QString &c2) const;

    // DB Side

    QString tableNameFromMetaObject(const QMetaObject *metaObject) const;
    virtual QString tableNameFromClassName(const QString &className) const = 0;

    QSqlField primaryFieldFromMetaObject(const QMetaObject *metaObject, int *index = nullptr) const;
    QSqlField userFieldFromMetaObject(const QMetaObject *metaObject, int *index = nullptr) const;
    QSqlField foreignFieldFromMetaObject(const QMetaObject *metaObject) const;
    virtual QString foreignFieldNameFromPropertyName(const QString &propertyName, const QString &className) const = 0;
    QSqlRecord recordFromMetaObject(const QMetaObject *metaObject, QMap<int, int> *metaIndexes = nullptr, QMap<int, int> *secretIndexes = nullptr) const;
    QSqlField fieldFromMetaProperty(const QMetaProperty &property, const QMetaObject *metaObject) const;
    QSqlField fieldFromSecretProperty(const SecretProperty &property, const QMetaObject *metaObject) const;
    virtual QString fieldNameFromPropertyName(const QString &propertyName, const QString &className) const = 0;

    QString associationTableNameFromMetaObjects(const QMetaObject *o1, const QMetaObject *o2) const;
    virtual QString associationTableNameFromClassNames(const QString &c1, const QString &c2) const = 0;

    static bool parsePropertyInput(const QString &input, QString *propertyName, QString *className);
    static bool parseFieldInput(const QString &input, QString *fieldName, QString *tableName);

    static Backend *instance();
    static Backend *defaultBackend();

protected:
    QMetaProperty primaryProperty(const QMetaObject *metaObject) const;
    QMetaProperty userProperty(const QMetaObject *metaObject) const;
    QList<QMetaProperty> searchProperties(const QMetaObject *metaObject) const;
    QList<QMetaProperty> dataProperties(const QMetaObject *metaObject) const;

private:
    bool isDataProperty(const QMetaProperty &property) const;

    friend class MetaTable;
};

class SYSTEMUS_CORE_EXPORT SystemusBackend : public Backend
{
public:
    QString name() const override;

    QString classNameFromTableName(const QString &tableName) const override;
    QString foreignPropertyNameFromPropertyName(const QString &propertyName, const QString &className) const override;
    QString propertyNameFromFieldName(const QString &fieldName, const QString &tableName) const override;

    QString tableNameFromClassName(const QString &className) const override;
    QString foreignFieldNameFromPropertyName(const QString &propertyName, const QString &className) const override;
    QString fieldNameFromPropertyName(const QString &propertyName, const QString &className) const override;

    QString associationTableNameFromClassNames(const QString &c1, const QString &c2) const override;
};

}
}

#endif // SYSTEMUS_ORM_BACKEND_H
