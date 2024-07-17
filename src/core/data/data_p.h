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
        AdapterDataType,
        AuthorizationDataType,
        PrivilegedDataType,
        UserDataType,
        RoleDataType,
        GroupDataType,
        PrivilegeDataType,
        PermissionDataType
    };

    virtual ~DataPrivate();

    virtual void init();

    virtual int id() const = 0;
    virtual void setId(int id) = 0;

    bool hasProperty(const QString &name, const Data *data = nullptr) const;
    QVariant property(const QString &name, const Data *data = nullptr) const;
    bool setProperty(const QString &name, const QVariant &value, Data *data = nullptr);

    virtual void fillRecord(QSqlRecord *record, const Data *data) const;
    virtual void fillWithRecord(const QSqlRecord &record, Data *data);

    virtual void fillJsonObject(QJsonObject *object, const Data *data) const;
    //virtual void fillWithJson(const QJsonObject &object);

    virtual bool isValid() const = 0;
    virtual bool isEmpty() const = 0;
    virtual bool equals(const DataPrivate *other) const = 0;
    virtual void clear() = 0;

    virtual QByteArray dataClassName() const = 0;
    virtual void setDataClassName(const QByteArray &className) = 0;
    DataInfo dataInfo() const;

    virtual DataPrivate *clone() const = 0;

    virtual int dataType() const = 0;

    virtual QSqlError *lastError() const = 0;

protected:
    virtual bool hasDataProperty(const QString &name) const = 0;
    virtual QVariant dataProperty(const QString &name) const = 0;
    virtual bool setDataProperty(const QString &name, const QVariant &value) = 0;

};

class DefaultDataPrivate : public DataPrivate
{
public:
    virtual ~DefaultDataPrivate();

    void init() override;

    int id() const override;
    void setId(int id) override;

    bool isValid() const override;
    bool isEmpty() const override;
    bool equals(const DataPrivate *other) const override;
    void clear() override;

    QByteArray dataClassName() const override;
    void setDataClassName(const QByteArray &className) override;

    DefaultDataPrivate *clone() const override
    { return new DefaultDataPrivate(*this); }

    int dataType() const override
    { return DefaultDataType; }

    QSqlError *lastError() const override;

protected:
    bool hasDataProperty(const QString &name) const override;
    QVariant dataProperty(const QString &name) const override;
    bool setDataProperty(const QString &name, const QVariant &value) override;

private:
    int _id;
    QVariantHash _properties;
    QByteArray _className;
    mutable QSqlError _lastError;
};

class AdapterDataPrivate : public DataPrivate
{
public:
    AdapterDataPrivate(Data *data);
    AdapterDataPrivate(const AdapterDataPrivate &other);
    ~AdapterDataPrivate();

    void init() override;

    int id() const override;
    void setId(int id) override;

    bool isValid() const override;
    bool isEmpty() const override;
    bool equals(const DataPrivate *other) const override;
    void clear() override;

    QByteArray dataClassName() const override;
    void setDataClassName(const QByteArray &className) override;

    AdapterDataPrivate *clone() const override
    { return new AdapterDataPrivate(*this); }

    int dataType() const override
    { return AdapterDataType; }

    QSqlError *lastError() const override;

protected:
    bool hasDataProperty(const QString &name) const override;
    QVariant dataProperty(const QString &name) const override;
    bool setDataProperty(const QString &name, const QVariant &value) override;

private:
    Data *_data;
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
    QList<int> secretFieldIndexes;

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
