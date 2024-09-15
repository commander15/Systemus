#ifndef SYSTEMUS_ORM_DATA_H
#define SYSTEMUS_ORM_DATA_H

#include <SystemusCore/global.h>
#include <SystemusCore/namespace.h>
#include <SystemusCore/orm.h>
#include <SystemusCore/abstractdata.h>
#include <SystemusCore/metatable.h>
#include <SystemusCore/querybuilder.h>

#include <QtSql/qsqlquery.h>
#include <QtSql/qsqlrecord.h>

#include <QtCore/qlist.h>

#define S_DATA(Class) \
    S_DATA_INIT(Class) \
    S_DATA_METHODS(Class)

#define S_DATA_INIT(Class) \
public: \
    const QMetaObject *metaObject() const override \
    { return &Class::staticMetaObject; } \
    static void registerClass() \
    { Systemus::Orm::MetaTable::registerClass<Class>(); } \
protected: \
    void registerData() override \
    { Class::registerClass(); } \
private:

#define S_DATA_METHODS(Class) \
public: \
    static inline Class fromPrimaryValue(const QVariant &value, bool full = true) \
    { return Systemus::Orm::Data::fromPrimaryValue<Class>(value, full); } \
    static inline Class fromFilter(const QString &filter, bool full = true) \
    { return Systemus::Orm::Data::fromFilter<Class>(filter, full); } \
    static inline QList<Class> all(bool full = true, QSqlError *error = nullptr) \
    { return Systemus::Orm::Data::all<Class>(full, error); } \
    static inline QList<Class> find(const Systemus::Orm::DataSearch &query, bool full = true, QSqlError *error = nullptr) \
    { return Systemus::Orm::Data::find<Class>(query, full, error); } \
    static inline QList<Class> fromSqlQuery(QSqlQuery &query, bool full = true) \
    { return Systemus::Orm::Data::fromSqlQuery<Class>(query, full); } \
    static inline QList<Class> fromSqlRecords(const QList<QSqlRecord> &records, bool full = true) \
    { return Systemus::Orm::Data::fromSqlRecords<Class>(records, full); } \
    static inline Class fromSqlRecord(const QSqlRecord &record, bool full = true) \
    { return Systemus::Orm::Data::fromSqlRecord<Class>(record, full); } \
    static int count(const Systemus::Orm::DataSearch &query = Systemus::Orm::DataSearch(), QSqlError *error = nullptr) \
    { return Systemus::Orm::Data::count<Class>(query, error); } \
private:

#define S_DATA_BASE

namespace Systemus {

class Data;

namespace Orm {

class DataSearch;

class DataPrivate;
class SYSTEMUS_CORE_EXPORT Data : public AbstractData
{
    Q_GADGET
    Q_PROPERTY(bool valid READ isValid STORED false)
    Q_PROPERTY(bool empty READ isEmpty STORED false)

public:
    Data();
    Data(const Data &other);
    Data(Data &&other);
    virtual ~Data();

    Data &operator=(const Data &other);

    Q_INVOKABLE bool get() override;
    Q_INVOKABLE bool getByPrimary(const QVariant &value, bool full = true);
    Q_INVOKABLE bool getByFilter(const QString &filter, bool full = true);
    virtual bool preGet();
    virtual bool postGet();

    Q_INVOKABLE bool update() override;
    virtual bool preUpdate();
    virtual bool postUpdate();

    Q_INVOKABLE bool insert() override;
    virtual bool preInsert();
    virtual bool postInsert();

    Q_INVOKABLE bool deleteData() override;
    virtual bool preDelete();
    virtual bool postDelete();

    QSqlError lastError() const override;

    QString className() const override final;
    virtual const QMetaObject *metaObject() const;
    QMetaType metaType() const;

    void dump() const;

    template<typename T>
    static inline T fromPrimaryValue(const QVariant &id, bool full = true);

    template<typename T>
    static inline T fromFilter(const QString &filter, bool full = true);

    template<typename T>
    static inline QList<T> all(bool full = true, QSqlError *error = nullptr);

    template<typename T>
    static inline QList<T> find(const DataSearch &query, bool full = true, QSqlError *error = nullptr);

    template<typename T>
    static inline QList<T> fromSqlQuery(QSqlQuery &query, bool full = true);

    template<typename T>
    static inline QList<T> fromSqlRecords(const QList<QSqlRecord> &records, bool full = true);

    template<typename T>
    static inline T fromSqlRecord(const QSqlRecord &record, bool full = true);

    template<typename T>
    static inline int count(const DataSearch &query, QSqlError *error = nullptr);

    bool operator==(const Data &other) const;
    inline bool operator!=(const Data &other) const
    { return !operator==(other); }

protected:
    Data(DataPrivate *d);
    Data(const QSharedDataPointer<DataPrivate> &d);

    void initData() override final;
    void registerData() override;

    bool isPropertyNull(const QString &name) const override;
    QVariant readProperty(const QString &name) const override;
    bool writeProperty(const QString &name, const QVariant &value) override;

    QSqlQuery execQuery(const QString &query, bool *ok = nullptr);

    void processError(const QSqlError &error) override;

    QSharedDataPointer<DataPrivate> d_ptr;

private:
    S_DECLARE_PRIVATE(Data)

    static QString selectStatementFromMetaObject(const QMetaObject *metaObject);

    friend class Systemus::Data;
    friend class DefaultDataPrivate;
    friend class AdapterDataPrivate;
    friend class QueryBuilder;
};

class SYSTEMUS_CORE_EXPORT DataSearch
{
public:
    DataSearch();
    DataSearch(const QString &className);
    DataSearch(const QString &filter, int page = 0, int itemsPerPage = 100);

    DataSearch &context(const QString &className);

    DataSearch &filter(const QString &filter);
    DataSearch &group(const QString property);
    DataSearch &sort(const QString &property, Qt::SortOrder order);
    DataSearch &page(int page);
    DataSearch &paginate(int itemsPerPage = 100);

    bool hasWhereClause() const;
    QString whereClause(bool inclusive = true) const;

    bool hasGroupByClause() const;
    QString groupByClause() const;

    bool hasOrderByClause() const;
    QString orderByClause() const;

    bool hasLimitOffsetClause() const;
    QString limitOffsetClause() const;

    void clear();

protected:
    QString m_className;

    QStringList m_filters;
    QStringList m_groupProperties;

    QStringList m_sortProperties;
    QList<Qt::SortOrder> m_sortOrders;

    int m_page;
    int m_itemsPerPage;
};

template<typename T>
inline T Data::fromPrimaryValue(const QVariant &value, bool full)
{
    T data;
    data.getByPrimary(value, full);
    return data;
}

template<typename T>
inline T Data::fromFilter(const QString &filter, bool full)
{
    T data;
    data.getByFilter(filter, full);
    return data;
}

template<typename T>
inline QList<T> Data::all(bool full, QSqlError *error)
{ return find<T>(DataSearch(), full, error); }

template<typename T>
inline QList<T> Data::find(const DataSearch &query, bool full, QSqlError *error)
{
    QString statement = QueryBuilder::selectStatement(MetaTable::fromType<T>());

    DataSearch dataQuery(query);
    dataQuery.context(T::staticMetaObject.className());

    if (dataQuery.hasWhereClause())
        statement.append(' ' + dataQuery.whereClause());

    if (dataQuery.hasGroupByClause())
        statement.append(' ' + dataQuery.groupByClause());

    if (dataQuery.hasOrderByClause())
        statement.append(' ' + dataQuery.orderByClause());

    if (dataQuery.hasLimitOffsetClause())
        statement.append(' ' + dataQuery.limitOffsetClause());

    bool ok;
    QSqlQuery qu = Systemus::exec(statement, &ok, error);
    if (ok)
        return fromSqlQuery<T>(qu, full);
    else
        return QList<T>();
}

template<typename T>
inline QList<T> Data::fromSqlQuery(QSqlQuery &query, bool full)
{
    QList<T> data;
    while (query.next())
        data.append(fromSqlRecord<T>(query.record(), full));
    return data;
}

template<typename T>
inline QList<T> Data::fromSqlRecords(const QList<QSqlRecord> &records, bool full)
{
    QList<T> data;
    for (const QSqlRecord &record : records)
        data.append(fromSqlRecord<T>(record, full));
    return data;
}

template<typename T>
inline T Data::fromSqlRecord(const QSqlRecord &record, bool full)
{
    const MetaTable table = MetaTable::fromType<T>();
    T data;
    if (full)
        data.preGet();
    data.fill(record);
    if (full)
        data.postGet();
    return data;
}

template<typename T>
inline  int Data::count(const DataSearch &query, QSqlError *error)
{
    const MetaTable table = MetaTable::fromType<T>();
    QString statement = "SELECT COUNT(" + table.primaryFieldName() + ") FROM " + table.tableName();

    if (query.hasWhereClause())
        statement.append(' ' + query.whereClause());

    if (query.hasGroupByClause())
        statement.append(' ' + query.groupByClause());

    if (query.hasOrderByClause())
        statement.append(' ' + query.orderByClause());

    if (query.hasLimitOffsetClause())
        statement.append(' ' + query.limitOffsetClause());

    bool ok;
    QSqlQuery qu = Systemus::exec(statement, &ok, error);
    if (ok && qu.next())
        return qu.value(0).toInt();
    else
        return 0;
}

}
}

template<>
SYSTEMUS_CORE_EXPORT Systemus::Orm::DataPrivate *QSharedDataPointer<Systemus::Orm::DataPrivate>::clone();

Q_DECLARE_METATYPE(Systemus::Orm::Data)

#endif // SYSTEMUS_ORM_DATA_H
