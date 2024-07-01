#ifndef SYSTEMUS_DATA_H
#define SYSTEMUS_DATA_H

#include <SystemusCore/global.h>

#include <QtSql/qsqlquery.h>
#include <QtSql/qsqlrecord.h>
#include <QtSql/qsqlfield.h>
#include <QtSql/qsqlrelationaltablemodel.h>

#include <QtSql/qsqldriver.h>

#include <QtCore/qjsonobject.h>
#include <QtCore/qdatetime.h>
#include <QtCore/qlist.h>

#define S_DATA(Class) \
    S_DATA_INIT(Class) \
    S_DATA_METHODS(Class)

#define S_DATA_INIT(Class) \
protected: \
    void init() override \
    { sRegisterType<Class>(); } \
    QByteArray className() const override \
    { return QByteArray(staticMetaObject.className()); }

#define S_DATA_METHODS(Class) \
public: \
    static inline QList<Class> all() \
    { return Systemus::Data::all<Class>(); } \
    static inline QList<Class> find(const Systemus::DataSearch &query) \
    { return Systemus::Data::find<Class>(query); } \
    static inline QList<Class> fromSqlQuery(QSqlQuery &query) \
    { return Systemus::Data::fromSqlQuery<Class>(query); } \
    static inline QList<Class> fromSqlRecords(const QList<QSqlRecord> &records) \
    { return Systemus::Data::fromSqlRecords<Class>(records); } \
    static inline Class fromSqlRecord(const QSqlRecord &record) \
    { return Systemus::Data::fromSqlRecord<Class>(record); }

class QSqlField;
class QSqlError;
class QSqlDriver;

namespace Systemus {

class DataInfo;
class DataSearch;

class SYSTEMUS_CORE_EXPORT DataSearch
{
public:
    DataSearch();
    DataSearch(const QString &filter, int page = 0, int itemsPerPage = 100);

    void filter(const QString &filter);
    void group(const QString field);
    void sort(int field, Qt::SortOrder order);
    void sort(const QString &field, Qt::SortOrder order);
    void page(int page);
    void paginate(int itemsPerPage = 100);

    bool hasWhereClause() const;
    QString whereClause() const;

    bool hasGroupByClause() const;
    QString groupByClause() const;

    bool hasOrderByClause() const;
    QString orderByClause() const;

    bool hasLimitOffsetClause() const;
    QString limitOffsetClause() const;

    void clear();

protected:
    QStringList _filters;
    QStringList _groups;
    QStringList _sortFields;
    QList<Qt::SortOrder> _sortOrders;
    int _page;
    int _itemsPerPage;
};

class DataInfoPrivate;
class SYSTEMUS_CORE_EXPORT DataInfo
{
public:
    DataInfo();
    DataInfo(const DataInfo &other);
    ~DataInfo();

    DataInfo &operator=(const DataInfo &other);

    QString idFieldName() const;
    QSqlField idField() const;

    QString userFieldName() const;
    QSqlField userField() const;

    QStringList searchFieldNames() const;

    QString fieldName(int index) const;
    QSqlField field(int index) const;
    int fieldCount() const;

    QSqlRecord record() const;

    QString tableName() const;

    QString foreignFieldName() const;
    QSqlField foreignField() const;

    QString foreignPropertyName() const;

    QString propertyName(int index) const;
    QMetaProperty property(int index) const;

    const QMetaObject *metaObject() const;

    bool operator==(const DataInfo &other) const;
    inline bool operator!=(const DataInfo &other) const
    { return !operator==(other); }

    template<typename T>
    static DataInfo fromType()
    { return fromRegistry(T::staticMetaObject.className()); }

    static DataInfo fromRegistry(const QByteArray &className);
    static DataInfo fromMetaObject(const QMetaObject &object);
    static DataInfo fromTable(const QString &tableName);
    static DataInfo fromTableRecord(const QString &tableName, const QSqlRecord &record);

    static QHash<QByteArray, DataInfo> _registry;

private:
    DataInfo(DataInfoPrivate *d);

    QSharedDataPointer<DataInfoPrivate> d_ptr;
};

class DataPrivate;
class SYSTEMUS_CORE_EXPORT Data
{
    Q_GADGET
    Q_PROPERTY(int id READ id WRITE setId)

public:
    Data();
    Data(const Data &other);
    virtual ~Data();
    
    Data &operator=(const Data &other);
    
    int id() const;
    void setId(int id);

    QVariant property(const QString &name) const;
    void setProperty(const QString &name, const QVariant &value);
    QMetaProperty metaPropertyByName(const QString &name) const;

    DataInfo dataInfo() const;

    virtual bool isValid() const;

    void fill(const Data &other);

    virtual void dumpInfos() const;

    virtual bool isEmpty() const;
    Q_INVOKABLE virtual void clear();
    
    Q_INVOKABLE inline bool get()
    { return get(id()); }
    Q_INVOKABLE inline bool get(int id)
    { return get("id = " + QString::number(id)); }
    Q_INVOKABLE virtual bool get(const QString &filter, bool withExtras = true);
    virtual bool getExtras();
    
    Q_INVOKABLE bool save();
    virtual bool insert();
    virtual bool update();
    
    Q_INVOKABLE virtual bool deleteData();
    
    QSqlError lastError() const;
    
    QSqlRecord toSqlRecord() const;

    virtual QJsonObject toJsonObject() const;

    template<typename T>
    static inline QList<T> all();
    
    template<typename T>
    static QList<T> find(const DataSearch &query);
    
    template<typename T>
    static QList<T> fromSqlQuery(QSqlQuery &query);
    
    template<typename T>
    static QList<T> fromSqlRecords(const QList<QSqlRecord> &records);
    
    template<typename T>
    static T fromSqlRecord(const QSqlRecord &record);

    bool operator==(const Data &other) const;
    inline bool operator!=(const Data &other) const
    { return !operator==(other); }
    
protected:
    Data(DataPrivate *data);
    Data(const QSharedDataPointer<DataPrivate> &other);

    virtual void init();

    virtual QByteArray className() const;

    void setDataInfo(const DataInfo &info);

    virtual void fillRecord(QSqlRecord *record) const;
    virtual void extractRecord(const QSqlRecord &record);

    QSqlQuery exec(const QString &query, bool *ok = nullptr) const;
    static QSqlQuery execCachedQuery(const QString &query, bool *ok = nullptr, QSqlError *error = nullptr);
    static QSqlQuery execQuery(const QString &query, bool *ok = nullptr, QSqlError *error = nullptr, bool cached = false);

    QSharedDataPointer<DataPrivate> d_ptr;

private:
    S_DECLARE_PRIVATE(Data)

    void initOperation() const;
    void initOperation();

    inline QString selectStatement() const;
    static QString selectStatement(const DataInfo &info);

    QString insertStatement() const;
    QString updateStatement() const;
    QString deleteStatement() const;
    static QSqlDriver *driver();
    
    void bindQueryValues(QSqlQuery &query) const;

    template<typename ForeignData>
    friend class DataRelation;
    friend class DataModel;

    friend class StandardUserInterface;
};

template<typename T>
QList<T> Data::all()
{ return find<T>(DataSearch(QString(), 0, 0)); }

template<typename T>
QList<T> Data::find(const DataSearch &query)
{
    QString statement = selectStatement(DataInfo::fromType<T>());

    if (query.hasWhereClause())
        statement.append(' ' + query.whereClause());

    if (query.hasGroupByClause())
        statement.append(' ' + query.groupByClause());

    if (query.hasOrderByClause())
        statement.append(' ' + query.orderByClause());

    if (query.hasLimitOffsetClause())
        statement.append(' ' + query.limitOffsetClause());

    bool ok;
    QSqlQuery qu = execQuery(statement, &ok);
    if (ok)
        return fromSqlQuery<T>(qu);
    else
        return QList<T>();
}

template<typename T>
QList<T> Data::fromSqlQuery(QSqlQuery &query)
{
    QList<T> data;
    while (query.next())
        data.append(fromSqlRecord<T>(query.record()));
    return data;
}

template<typename T>
QList<T> Data::fromSqlRecords(const QList<QSqlRecord> &records)
{
    QList<T> data;
    for (const QSqlRecord &record : records)
        data.append(fromSqlRecord<T>(record));
    return data;
}

template<typename T>
T Data::fromSqlRecord(const QSqlRecord &record)
{
    T data;
    data.extractRecord(record);
    data.getExtras();
    return data;
}

}
template<typename T>
static void sRegisterType(const Systemus::DataInfo &info)
{
    const QByteArray className(T::staticMetaObject.className());

    QHash<QByteArray, Systemus::DataInfo> *registry = &Systemus::DataInfo::_registry;
    if (className == Systemus::Data::staticMetaObject.className())
        qDebug() << Q_FUNC_INFO << ": registering Systemus::Data detected !";
    else
        registry->insert(className, info);

    qDebug() << "Type" << className << info.tableName();
}

template<typename T>
static void sRegisterType()
{ sRegisterType<T>(Systemus::DataInfo::fromMetaObject(T::staticMetaObject)); }

Q_DECLARE_METATYPE(Systemus::Data)

#endif // SYSTEMUS_DATA_H
