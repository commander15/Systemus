#ifndef SYSTEMUS_DATA_H
#define SYSTEMUS_DATA_H

#include <SystemusCore/global.h>

#include <QtSql/qsqlquery.h>
#include <QtSql/qsqlrecord.h>

#include <QtCore/qlist.h>

#define S_DATA(Class) \
    S_DATA_INIT(Class) \
    S_DATA_METHODS(Class)

#define S_DATA_INIT(Class) \
protected: \
    void initData() override \
    { setDataClassName(Class::staticMetaObject.className()); } \
    const QMetaObject *instanceMetaObject() const override \
    { return &Class::staticMetaObject; } \
private:

#define S_DATA_METHODS(Class) \
public: \
    static int count(const QString &filter = QString(), QSqlError *error = nullptr) \
    { return Systemus::Data::count<Class>(filter, error); } \
    static inline QList<Class> all(QSqlError *error = nullptr) \
    { return Systemus::Data::all<Class>(error); } \
    static inline QList<Class> find(const Systemus::DataSearch &query, QSqlError *error = nullptr) \
    { return Systemus::Data::find<Class>(query, error); } \
    static inline QList<Class> fromSqlQuery(QSqlQuery &query) \
    { return Systemus::Data::fromSqlQuery<Class>(query); } \
    static inline QList<Class> fromSqlRecords(const QList<QSqlRecord> &records) \
    { return Systemus::Data::fromSqlRecords<Class>(records); } \
    static inline Class fromSqlRecord(const QSqlRecord &record) \
    { return Systemus::Data::fromSqlRecord<Class>(record); } \
private:

class QSqlField;
class QSqlError;
class QSqlDriver;

namespace Systemus {

class DataInfo;
class DataSearch;
class DataStorage;

class DataPrivate;
class SYSTEMUS_CORE_EXPORT Data
{
    Q_GADGET
    Q_PROPERTY(int id READ id WRITE setId)

public:
    Data();
    Data(const Data &other);
    Data(const Data &other, bool transferProperties);
    virtual ~Data();

    Data &operator=(const Data &other);

    void init();

    int id() const;
    void setId(int id);

    QVariant property(const QString &name) const;
    virtual void setProperty(const QString &name, const QVariant &value);
    QMetaProperty metaPropertyByName(const QString &name) const;

    void fill(const Data &other);
    void fill(const QSqlRecord &record);

    DataInfo dataInfo() const;

    virtual bool isValid() const;

    void dumpInfos() const;

    virtual bool isEmpty() const;
    Q_INVOKABLE virtual void clear();

    Q_INVOKABLE inline bool get()
    { return get("id = " + QString::number(id()), true); }
    Q_INVOKABLE inline bool get(int id)
    { return get("id = " + QString::number(id), true); }
    Q_INVOKABLE virtual bool get(const QString &filter, bool withExtras = true);
    virtual bool getExtras();

    Q_INVOKABLE bool save();
    virtual bool insert();
    virtual bool update();

    Q_INVOKABLE virtual bool deleteData();

    QSqlError lastError() const;

    template<typename T> T to() const;
    QSqlRecord toSqlRecord() const;
    QJsonObject toJsonObject() const;

    const void *internalData() const;
    void *internalData();

    static QSqlQuery execCachedQuery(const QString &query, bool *ok = nullptr, QSqlError *error = nullptr);
    static QSqlQuery execQuery(const QString &query, bool *ok = nullptr, QSqlError *error = nullptr, bool cached = false);

    static bool beginTransaction();
    static bool commitTransaction();
    static bool rollbackTransaction();

    //static QString selectStatement(const QString &table, const QSqlRecord &record, const QList<QSqlRelation> &relations);
    static QSqlDriver *driver();

    template<typename T>
    static int count(const QString &filter = QString(), QSqlError *error = nullptr);

    template<typename T>
    static inline QList<T> all(QSqlError *error = nullptr);

    template<typename T>
    static QList<T> find(const DataSearch &query, QSqlError *error = nullptr);

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
    Data(const QSharedDataPointer<DataPrivate> &data);

    virtual void initData();

    QByteArray instanceClassName() const;
    virtual const QMetaObject *instanceMetaObject() const;

    QByteArray dataClassName() const;

    void setDataClassName(const QByteArray &name);

    virtual void fillRecord(QSqlRecord *record) const;
    virtual void extractRecord(const QSqlRecord &record);

    QSqlQuery exec(const QString &query, bool *ok = nullptr) const;

    QSharedDataPointer<DataPrivate> d_ptr;

private:
    S_DECLARE_PRIVATE(Data)

    inline QString selectStatement() const;
    static QString selectStatement(const DataInfo &info);

    QString insertStatement() const;
    QString updateStatement() const;
    QString deleteStatement() const;

    void bindQueryValues(QSqlQuery &query) const;

    friend class AbstractDataRelation;
    friend class DataModel;

    friend class StandardUserInterface;
};

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

typedef QList<Data>(FindDataFunction)(const DataSearch &query, QSqlError *error);
typedef void(DataTransferFunction)(const Data *from, Data *to, int type);
typedef void(JsonGenerationFunction)(const Data &data, QJsonObject *object);

class DataInfoPrivate;
class SYSTEMUS_CORE_EXPORT DataInfo
{
public:
    DataInfo();
    DataInfo(const DataInfo &other);
    ~DataInfo();

    DataInfo &operator=(const DataInfo &other);

    QString tableName() const;

    QString idFieldName() const;
    QSqlField idField() const;
    QString idPropertyName() const;
    QMetaProperty idProperty() const;

    QString userFieldName() const;
    QSqlField userField() const;
    QString userPropertyName() const;
    QMetaProperty userProperty() const;

    QStringList displayFieldNames() const;
    QString displayFieldName(int index) const;
    QSqlField displayField(int index) const;
    QStringList displayPropertyNames() const;
    QString displayPropertyName(int index) const;
    QMetaProperty displayProperty(int index) const;
    int displayCount() const;
    QSqlRecord displayRecord() const;

    QStringList searchFieldNames() const;
    QStringList searchPropertyNames() const;

    QStringList fieldNames() const;
    QString fieldName(int index) const;
    QSqlField field(int index) const;
    QStringList fieldPropertyNames() const;
    QString fieldPropertyName(int index) const;
    QMetaProperty fieldProperty(int index) const;
    int count() const;
    QSqlRecord record() const;

    QString foreignFieldName() const;
    QSqlField foreignField() const;
    QString foreignPropertyName() const;

    void dumpInfos();

    Data newData() const;
    Data newData(const QSqlRecord &record) const;

    QList<Data> find(const DataSearch &query, QSqlError *error) const;

    const QMetaObject *metaObject() const;

    bool operator==(const DataInfo &other) const;
    inline bool operator!=(const DataInfo &other) const
    { return !operator==(other); }

    static inline DataInfo fromName(const QByteArray &className)
    { return fromRegistry(className); }

    template<typename T>
    static inline DataInfo fromType()
    { return fromRegistry(T::staticMetaObject.className()); }

    static DataInfo fromRegistry(const QByteArray &className);
    static DataInfo fromMetaObject(const QMetaObject &object);
    static DataInfo fromTable(const QString &tableName);
    static DataInfo fromTableRecord(const QString &tableName, const QSqlRecord &record);

    static void registerInfo(const QByteArray &className, const DataInfo &info);
    static void registerFindFunction(const QByteArray &className, const std::function<FindDataFunction> &function);
    static void registerTransferFunction(const QByteArray &className, const std::function<DataTransferFunction> &function);
    static void registerJsonGenerationFunction(const QByteArray &className, const std::function<JsonGenerationFunction> &function);

    static QList<QSqlField> fieldsFromString(const QString &str, const QString &tableName, const QString &context);

private:
    DataInfo(DataInfoPrivate *d);

    QSharedDataPointer<DataInfoPrivate> d_ptr;
};

class SYSTEMUS_CORE_EXPORT DataRegistration
{
public:
    DataRegistration(const QByteArray &className);

    DataRegistration &init(const std::function<void()> &function);
    DataRegistration &transfer(const std::function<DataTransferFunction> &function);
    DataRegistration &jsonGenerator(const std::function<JsonGenerationFunction> &function);

private:
    const QByteArray _className;
};

template<typename T>
inline T Data::to() const
{
    T data;
    data.setId(id());
    data.fill(*this);
    return data;
}

template<typename T>
int Data::count(const QString &filter, QSqlError *error)
{
    const DataInfo info = DataInfo::fromType<T>();
    QString statement = "SELECT COUNT(" + info.idFieldName() + ") FROM " + info.tableName();

    if (!filter.isEmpty())
        statement.append(" WHERE " + filter);

    bool ok;
    QSqlQuery qu = execQuery(statement, &ok, error);
    if (ok && qu.next())
        return qu.value(0).toInt();
    else
        return 0;
}

template<typename T>
QList<T> Data::all(QSqlError *error)
{ return find<T>(DataSearch(QString(), 0, 0), error); }

template<typename T>
QList<T> Data::find(const DataSearch &query, QSqlError *error)
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
    QSqlQuery qu = execQuery(statement, &ok, error);
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
Systemus::DataRegistration sRegisterType(Systemus::DataInfo info)
{
    const QByteArray className(T::staticMetaObject.className());

    Systemus::DataInfo::registerInfo(className, info);

    Systemus::DataInfo::registerFindFunction(className, [](const Systemus::DataSearch &query, QSqlError *error) {
        const QList<T> found = Systemus::Data::find<T>(query, error);
        QList<Systemus::Data> data;
        for (const T &f : found)
            data.append(f);
        return data;
    });

    qRegisterMetaType<T>();

    return Systemus::DataRegistration(className);
}

template<typename T>
Systemus::DataRegistration sRegisterType()
{ return sRegisterType<T>(Systemus::DataInfo::fromMetaObject(T::staticMetaObject)); }

Q_DECLARE_METATYPE(Systemus::Data)

#endif // SYSTEMUS_DATA_H
