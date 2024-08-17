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
    static inline QList<Class> all(bool withExtras = true, QSqlError *error = nullptr) \
    { return Systemus::Data::all<Class>(withExtras, error); } \
    static inline QList<Class> find(const Systemus::DataSearch &query, bool withExtras = true, QSqlError *error = nullptr) \
    { return Systemus::Data::find<Class>(query, withExtras, error); } \
    static inline QList<Class> fromSqlQuery(QSqlQuery &query, bool withExtras = true) \
    { return Systemus::Data::fromSqlQuery<Class>(query, withExtras); } \
    static inline QList<Class> fromSqlRecords(const QList<QSqlRecord> &records, bool withExtras = true) \
    { return Systemus::Data::fromSqlRecords<Class>(records, withExtras); } \
    static inline Class fromId(int id, bool withExtras = true) \
    { return Systemus::Data::fromId<Class>(id, withExtras); } \
    static inline Class fromSqlRecord(const QSqlRecord &record, bool withExtras = true) \
    { return Systemus::Data::fromSqlRecord<Class>(record, withExtras); } \
private:

class QSqlField;
class QSqlError;
class QSqlDriver;

namespace Systemus {

class DataInfo;
class DataSearch;

class DataPrivate;
class SYSTEMUS_CORE_EXPORT Data
{
    Q_GADGET
    Q_PROPERTY(int id READ id WRITE setId)

public:
    enum ExtraType {
        PreExtra,
        PostExtra
    };

    Data();
    Data(const Data &other, bool adapt);
    Data(const Data &other);
    Data(Data &&other);
    virtual ~Data();

    Data &operator=(const Data &other);

    void init();

    template<typename T = int> T id() const;
    template<typename T = int> void setId(const T &id);

    int id() const;
    void setId(int id);

    bool hasProperty(const QString &name) const;
    QVariant property(const QString &name) const;
    void setProperty(const QString &name, const QVariant &value);

    virtual bool isValid() const;
    virtual bool isEmpty() const;
    void dumpInfos() const;

    void fill(const Data &other, bool withId = false);
    void fill(const QSqlRecord &record, bool withId = false);
    Q_INVOKABLE virtual void clear();

    template<typename T> T to() const;
    virtual QSqlRecord toSqlRecord() const;
    virtual QJsonObject toJsonObject() const;

    //DataInfo dataInfo() const;

    Q_INVOKABLE bool get();
    Q_INVOKABLE bool get(int id, bool withExtras = true);
    Q_INVOKABLE bool get(const QString &filter, bool withExtras = true);

    bool getData(const QString &filter);
    virtual bool getExtras(ExtraType type = PostExtra);

    Q_INVOKABLE bool save();

    bool insert();
    virtual bool insertExtras(ExtraType type);

    bool update();
    virtual bool updateExtras(ExtraType type);

    Q_INVOKABLE virtual bool deleteData();
    virtual bool deleteExtras(ExtraType type);

    DataInfo info() const;
    QSqlError lastError() const;

    bool isAdapted() const;
    const Data *adaptedData() const;

    const void *internalData() const;
    void *internalData();

    static QSqlQuery prepareQuery(const QString &query);

    static QSqlQuery execCachedQuery(const QString &query, bool *ok = nullptr, QSqlError *error = nullptr);
    static QSqlQuery execQuery(const QString &query, bool *ok = nullptr, QSqlError *error = nullptr, bool cached = false);
    static QSqlQuery execQuery(const QString &query, const QVariantList &values, bool *ok = nullptr, QSqlError *error = nullptr, bool cached = false);
    static QSqlQuery execQuery(const QString &query, const QVariantHash &values, bool *ok = nullptr, QSqlError *error = nullptr, bool cached = false);
    static bool execQuery(QSqlQuery &query, bool cached, QSqlError *error = nullptr);

    static bool beginTransaction();
    static bool commitTransaction();
    static bool rollbackTransaction();

    //static QString selectStatement(const QString &table, const QSqlRecord &record, const QList<QSqlRelation> &relations);
    static QString formatValue(const QVariant &value);
    static QSqlDriver *driver();

    template<typename T>
    static int count(const QString &filter = QString(), QSqlError *error = nullptr);

    template<typename T>
    static inline QList<T> all(bool withExtras = true, QSqlError *error = nullptr);

    template<typename T>
    static QList<T> find(const DataSearch &query, bool withExtras = true, QSqlError *error = nullptr);

    template<typename T>
    static QList<T> fromSqlQuery(QSqlQuery &query, bool withExtras = true);

    template<typename T>
    static QList<T> fromSqlRecords(const QList<QSqlRecord> &records, bool withExtras = true);

    template<typename T>
    static T fromId(int id, bool withExtras = true);

    template<typename T>
    static T fromSqlRecord(const QSqlRecord &record, bool withExtras = true);

    bool operator==(const Data &other) const;
    inline bool operator!=(const Data &other) const
    { return !operator==(other); }

protected:
    Data(DataPrivate *data);
    Data(const QSharedDataPointer<DataPrivate> &data);

    virtual void initData();

    virtual bool saveReadOnlyProperty(const QString &name, const QVariant &value);

    QByteArray instanceClassName() const;
    virtual const QMetaObject *instanceMetaObject() const;

    QByteArray dataClassName() const;
    void setDataClassName(const QByteArray &name);

    QSqlQuery exec(const QString &query, bool *ok = nullptr) const;

    QSharedDataPointer<DataPrivate> d_ptr;

private:
    S_DECLARE_PRIVATE(Data)

    virtual QString selectStatement() const;
    static QString selectStatement(const DataInfo &info);

    virtual QString insertStatement() const;
    virtual QString updateStatement() const;
    virtual QString deleteStatement() const;

    void bindQueryValues(QSqlQuery &query) const;

    friend class DataInfo;
    friend class AbstractDataModel;

    friend class AdapterDataPrivate;
    friend class AbstractDataRelation;

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

    static QString formatExpression(const QString &filter);

protected:
    QStringList _filters;
    QStringList _groups;
    QStringList _sortFields;
    QList<Qt::SortOrder> _sortOrders;
    int _page;
    int _itemsPerPage;
};

typedef QList<Data>(FindDataFunction)(const DataSearch &, bool, QSqlError *);
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
    //QString searchFieldName(int index) const;
    //QSqlField searchField(int index) const;
    QStringList searchPropertyNames() const;
    //QString searchPropertyName(int index) const;
    //QMetaProperty searchProperty(int index);
    //int searchCount() const;
    //QSqlRecord searchRecord() const;

    QStringList secretFieldNames() const;
    QString secretFieldName(int index) const;
    QSqlField secretField(int index) const;
    QStringList secretPropertyNames() const;
    QString secretPropertyName(int index) const;
    QMetaProperty secretProperty(int index) const;
    int secretCount() const;
    QSqlRecord secretRecord() const;

    QStringList fieldNames() const;
    int fieldIndex(const QString &name) const;
    QString fieldName(int index) const;
    QSqlField field(int index) const;
    QStringList fieldPropertyNames() const;
    int fieldPropertyIndex(const QString &name) const;
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

    QList<Data> find(const DataSearch &query, bool withExtras, QSqlError *error) const;

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

    static bool isRegistered(const QByteArray &className);
    static void registerInfo(const QByteArray &className, const DataInfo &info);
    static void registerFindFunction(const QByteArray &className, const std::function<FindDataFunction> &function);
    static void registerTransferFunction(const QByteArray &className, const std::function<DataTransferFunction> &function);
    static void registerJsonGenerationFunction(const QByteArray &className, const std::function<JsonGenerationFunction> &function);

    static QString tableNameFromClassName(const QString &className);
    static QString propertyNameFromFieldName(const QString &fieldName);
    static QString fieldNameFromPropertyName(const QString &propertyName);
    static QList<QSqlField> fieldsFromString(const QString &str, const QString &tableName, const QString &context);

private:
    DataInfo(DataInfoPrivate *d);

    QSharedDataPointer<DataInfoPrivate> d_ptr;

    friend class DataTableModel;
    friend class TableView;
};

class SYSTEMUS_CORE_EXPORT DataRegistration
{
public:
    DataRegistration(const QByteArray &className);

    DataRegistration &init(const std::function<void()> &function);
    DataRegistration &transfer(const std::function<DataTransferFunction> &function);
    DataRegistration &jsonGenerator(const std::function<JsonGenerationFunction> &function);

private:
    inline bool isValid() const
    { return !_className.isEmpty(); }

    const QByteArray _className;
};

template<typename T>
inline T Data::to() const
{
    if (isAdapted() && dataClassName() == T::staticMetaObject.className())
        return *static_cast<const T *>(adaptedData());
    else {
        T data;
        data.setId(id());
        data.fill(*this);
        return data;
    }
}

template<typename T>
int Data::count(const QString &filter, QSqlError *error)
{
    const DataInfo info = DataInfo::fromType<T>();
    QString statement = "SELECT COUNT(" + info.idFieldName() + ") FROM " + info.tableName();

    if (!filter.isEmpty())
        statement.append(" WHERE " + DataSearch::formatExpression(filter));

    bool ok;
    QSqlQuery qu = execQuery(statement, &ok, error);
    if (ok && qu.next())
        return qu.value(0).toInt();
    else
        return 0;
}

template<typename T>
QList<T> Data::all(bool withExtras, QSqlError *error)
{ return find<T>(DataSearch(QString(), 0, 0), withExtras, error); }

template<typename T>
QList<T> Data::find(const DataSearch &query, bool withExtras, QSqlError *error)
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
        return fromSqlQuery<T>(qu, withExtras);
    else
        return QList<T>();
}

template<typename T>
QList<T> Data::fromSqlQuery(QSqlQuery &query, bool withExtras)
{
    QList<T> data;
    while (query.next())
        data.append(fromSqlRecord<T>(query.record(), withExtras));
    return data;
}

template<typename T>
QList<T> Data::fromSqlRecords(const QList<QSqlRecord> &records, bool withExtras)
{
    QList<T> data;
    for (const QSqlRecord &record : records)
        data.append(fromSqlRecord<T>(record, withExtras));
    return data;
}


template<typename T>
T Data::fromId(int id, bool withExtras)
{
    T data;
    data.get(id, withExtras);
    return data;
}

template<typename T>
T Data::fromSqlRecord(const QSqlRecord &record, bool withExtras)
{
    const DataInfo info = DataInfo::fromType<T>();
    T data;
    data.fill(record, true);
    data.getExtras(PostExtra);
    return data;
}

}

template<typename T>
Systemus::DataRegistration sRegisterType(Systemus::DataInfo info)
{
    const QByteArray className(T::staticMetaObject.className());

    if (Systemus::DataInfo::isRegistered(className))
        return Systemus::DataRegistration(QByteArray());

    Systemus::DataInfo::registerInfo(className, info);

    Systemus::DataInfo::registerFindFunction(className, [](const Systemus::DataSearch &query, bool withExtras, QSqlError *error) {
        const QList<T> found = Systemus::Data::find<T>(query, withExtras, error);
        QList<Systemus::Data> data;
        for (const T &f : found)
            data.append(f);
        return data;
    });

    qRegisterMetaType<T>();

    return Systemus::DataRegistration(className);
}

template<>
SYSTEMUS_CORE_EXPORT Systemus::DataPrivate *QSharedDataPointer<Systemus::DataPrivate>::clone();

template<typename T>
Systemus::DataRegistration sRegisterType()
{ return sRegisterType<T>(Systemus::DataInfo::fromMetaObject(T::staticMetaObject)); }

Q_DECLARE_METATYPE(Systemus::Data)

#endif // SYSTEMUS_DATA_H
