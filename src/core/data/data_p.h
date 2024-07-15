#ifndef SYSTEMUS_DATA_P_H
#define SYSTEMUS_DATA_P_H

#include <SystemusCore/data.h>

#include <QtSql/qsqlerror.h>

#include <QtCore/qshareddata.h>

namespace Systemus {

class DataPrivate : public QSharedData
{
public:
    enum DataType {
        DefaultDataType,
        AuthorizationDataType,
        PrivilegedDataType,
        UserDataType,
        ProfileDataType,
        RoleDataType,
        GroupDataType,
        PrivilegeDataType,
        PermissionDataType
    };
    
    DataPrivate();
    DataPrivate(const DataPrivate &other) = default;

    virtual QVariant property(const QString &name) const;
    virtual void setProperty(const QString &name, const QVariant &value);

    virtual DataPrivate *clone() const
    {
        DataPrivate *d = new DataPrivate();
        d->initialized = initialized; // May or may not produce bugs
        d->id = id;
        d->properties = properties;
        d->className = className;
        d->lastError = lastError; // Not mandatory
        return d;
    }
    
    virtual bool equalsTo(const DataPrivate *other) const;
    
    virtual void clear()
    {
        id = 0;
        properties.clear();
        lastError = QSqlError();
    }
    
    virtual inline int dataType() const
    { return DefaultDataType; }

    bool initialized;
    
    int id;
    QVariantHash properties;
    QByteArray className;

    mutable QSqlError lastError;
};

class DataInfoPrivate : public QSharedData
{
public:
    DataInfoPrivate();
    DataInfoPrivate(const DataInfoPrivate &other) = default;
    ~DataInfoPrivate();

    QMetaProperty fieldProperty(int fieldIndex) const;

    QMetaProperty property(const QString &name) const;
    QMetaClassInfo classInfo(const QString &name) const;

    void update(const QMetaObject *object);

    bool equals(const DataInfoPrivate &other) const;

    static QString tableNameFromClassName(const QString &className);
    static QString fieldNameFromPropertyName(const QString &propertyName);
    static QSqlField fieldFromProperty(const QMetaProperty &property, const QMetaObject *object);
    static QList<QSqlField> fieldsFromString(const QString &str, const QString &tableName, const QString &context);

    static QString propertyNameFromFieldName(const QString &fieldName);

    int idFieldIndex;
    int userFieldIndex;
    QList<int> displayFieldIndexes;
    QList<int> searchFieldIndexes;

    QString tableName;
    QSqlRecord tableRecord;

    QHash<int, int> fieldProperties;
    const QMetaObject *metaObject;

    static QHash<QByteArray, DataInfo> registry;
    static QHash<QByteArray, std::function<FindDataFunction>> findFunctions;
    static QHash<QByteArray, std::function<JsonGenerationFunction>> jsonGenerationFunctions;
    static QHash<QByteArray, std::function<DataTransferFunction>> transferFunctions;
};

}

#endif //
