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
        AccessControlDataType
    };
    
    DataPrivate();
    DataPrivate(const DataPrivate &other) = default;
    
    virtual bool equalsTo(const DataPrivate *other) const;
    
    virtual void clear()
    {
        id = 0;
        values.clear();
        lastError = QSqlError();
    }
    
    virtual inline DataType dataType() const
    { return DefaultDataType; }
    
    int id;
    QVariantHash values;

    bool initialized;

    mutable QSqlError lastError;
};

class DataInfoPrivate : public QSharedData
{
public:
    DataInfoPrivate();
    DataInfoPrivate(const DataInfoPrivate &other) = default;
    ~DataInfoPrivate();

    QMetaProperty property(const QString &name) const;
    QMetaClassInfo classInfo(const QString &name) const;

    void update(const QMetaObject *object);

    bool equals(const DataInfoPrivate &other) const;

    static QString tableNameFromClassName(const QString &className);
    static QString fieldNameFromPropertyName(const QString &propertyName);
    static QSqlField fieldFromProperty(const QMetaProperty &property, const QMetaObject *object);
    static QList<QSqlField> fieldsFromString(const QString &str, const QString &tableName, const QString &context);

    int idFieldIndex;
    int userFieldIndex;
    QList<int> searchFieldIndexes;

    QString tableName;
    QSqlRecord tableRecord;

    QVector<QMetaProperty> metaFields;
    const QMetaObject *metaObject;
};

}

#endif //
