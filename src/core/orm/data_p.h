#ifndef SYSTEMUS_DATA_P_H
#define SYSTEMUS_DATA_P_H

#include <SystemusCore/data.h>

#include <QtSql/qsqlerror.h>

#include <QtCore/qshareddata.h>

namespace Systemus {

class DataPrivate : public QSharedData
{
public:
    enum FieldType {
        StandardField,
        ForeignField
    };

    enum DataType {
        DefaultDataType,
        AccessControlDataType
    };
    
    DataPrivate() = default;
    DataPrivate(const DataPrivate &other) = default;
    
    virtual bool equalsTo(const DataPrivate *other) const
    { return id == other->id; }
    
    virtual void clear()
    {
        id = 0;
        lastError = QSqlError();
    }

    static QString tableNameFromClassName(const QString &className);
    static QString fieldNameFromPropertyName(const QString &propertyName);

    static bool isStandardFieldProperty(const QMetaProperty &property, const QMetaObject &metaObject);
    static QSqlField standardFieldFromProperty(const QMetaProperty &property, const QMetaObject &metaObject, bool includeTable);

    static bool isForeignProperty(const QMetaProperty &property, const QMetaObject &metaObject);
    static QSqlField foreignFieldFromProperty(const QMetaProperty &property, const QMetaObject &metaObject, bool includeTable);

    static bool isRelationProperty(const QMetaProperty &property, const QMetaObject &metaObject);
    static QSqlField relationFieldFromProperty(const QMetaProperty &property, const QMetaObject &metaObject, bool includeTable);
    static QSqlRelation relationFromProperty(const QMetaProperty &property, const QMetaObject &metaObject);

    static QSqlField fieldFromProperty(const QMetaProperty &property, const QMetaObject &object, bool includeTable);
    static QList<QMetaProperty> fieldProperties(const QMetaObject &metaObject, FieldType type);
    
    virtual inline DataType dataType() const
    { return DefaultDataType; }
    
    int id;

    QVariantHash values;
    
    mutable QSqlError lastError;

    bool copyPropertiesOnAssignment = true;
};

class AuthorizationDataPrivate : public DataPrivate
{
public:
    AuthorizationDataPrivate() = default;
    AuthorizationDataPrivate(const AuthorizationDataPrivate &other) = default;
    
    virtual bool equalsTo(const DataPrivate *o) const override
    {
        const AuthorizationDataPrivate *other = static_cast<const AuthorizationDataPrivate *>(o);

        return name == other->name
               && description == other->description
               && creationDate == other->creationDate
               && creationTime == other->creationTime
               && DataPrivate::equalsTo(o);
    }
    
    virtual void clear() override
    {
        name.clear();
        description.clear();
        creationDate = QDate();
        creationTime = QTime();
        DataPrivate::clear();
    }
    
    inline DataType dataType() const override
    { return AccessControlDataType; }
    
    QString name;
    QString description;

    QDate creationDate;
    QTime creationTime;
};

class IssuedAuthorizationDataPrivate : public DataPrivate
{
public:
    IssuedAuthorizationDataPrivate() = default;
    IssuedAuthorizationDataPrivate(const IssuedAuthorizationDataPrivate &other) = default;

    virtual bool equalsTo(const DataPrivate *o) const override
    {
        const IssuedAuthorizationDataPrivate *other = static_cast<const IssuedAuthorizationDataPrivate *>(o);

        return refData == other->refData
               && active == other->active
               && issueDate == other->issueDate
               && issueTime == other->issueTime
               && DataPrivate::equalsTo(o);
    }

    AuthorizationData refData;

    bool active;
    QDate issueDate;
    QTime issueTime;
};

}

#endif //
