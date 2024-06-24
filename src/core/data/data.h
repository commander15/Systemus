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

#define SYSTEMUS_DATA_METHODS(Class) \
    public: \
        static inline QList<Class> all() \
        { return Systemus::Data::all<Class>(); } \
        static inline QList<Class> find(const Systemus::DataSearch &query) \
        { return Systemus::Data::find<Class>(query); }

class QSqlField;
class QSqlRelation;
class QSqlError;
class QSqlDriver;

namespace Systemus {

class SYSTEMUS_CORE_EXPORT DataSearch
{
public:
    DataSearch()  :
        _page(0), _itemsPerPage(100)
    {}
    
    DataSearch(const QString &filter, int page = 0, int itemsPerPage = 100) :
        _page(page), _itemsPerPage(itemsPerPage)
    { this->filter(filter); }
    
    void filter(const QString &filter)
    { if (!filter.isEmpty()) _filters.append(filter); }

    void group(const QString field)
    { if (!field.isEmpty()) _groups.append(field); }
    
    void sort(int field, Qt::SortOrder order)
    {
        if (field >= 0) {
            _sortFields.append(QString::number(field + 1));
            _sortOrders.append(order);
        }
    }
    
    void sort(const QString &field, Qt::SortOrder order)
    {
        if (!field.isEmpty()) {
            _sortFields.append(field);
            _sortOrders.append(order);
        }
    }

    void page(int page)
    { _page = page; }

    bool hasWhereClause() const
    { return !_filters.isEmpty(); }
    QString whereClause() const
    { return (hasWhereClause() ? "WHERE " + _filters.join(" AND ") : QString()); }

    bool hasGroupByClause() const
    { return !_groups.isEmpty(); }
    QString groupByClause() const
    {
        if (hasGroupByClause())
            return "GROUP BY " + _groups.join(", ");
        else
            return QString();
    }

    bool hasOrderByClause() const
    { return !_sortFields.isEmpty(); }
    QString orderByClause() const
    {
        if (!hasOrderByClause())
            return QString();

        QStringList clauses;
        for (int i(0); i < _sortFields.size(); ++i) {
            const QString field = _sortFields.at(i);
            const QString order = (_sortOrders.at(i) == Qt::AscendingOrder ? "ASC" : "DESC");
            clauses.append(field + ' ' + order);
        }

        return QStringLiteral("ORDER BY ") + clauses.join(", ");
    }

    bool hasLimitOffsetClause() const
    { return _page > 0 && _itemsPerPage > 0; }

    QString limitOffsetClause() const
    {
        if (hasLimitOffsetClause())
            return QStringLiteral("LIMIT %1 OFFSET %2").arg(_itemsPerPage).arg((_page - 1) * _itemsPerPage);
        else
            return QString();
    }

    void clear()
    {
        _filters.clear();
        _groups.clear();
        _sortFields.clear();
        _sortOrders.clear();
        _page = 0;
        _itemsPerPage = 100;
    }

protected:
    QStringList _filters;
    QStringList _groups;
    QStringList _sortFields;
    QList<Qt::SortOrder> _sortOrders;
    int _page;
    int _itemsPerPage;
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

    virtual bool isValid() const;

    void fill(const Data &other);

    void dumpInfos() const;

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

    inline QString tableName() const
    { return tableName(*metaObject); }
    static QString tableName(const QMetaObject &metaObject);

    inline QSqlRecord tableRecord(bool excludeReadOnly = false) const
    { return tableRecord(*metaObject, false, excludeReadOnly); }
    static QSqlRecord tableRecord(const QMetaObject &metaObject, bool includeTable = false, bool excludeReadOnly = false);

    inline QList<QSqlRelation> tableRelations() const
    { return tableRelations(*metaObject); }
    static QList<QSqlRelation> tableRelations(const QMetaObject &metaObject);

    static QString displayFieldName(const QMetaObject &object);

    inline QString foreignFieldName() const
    { return foreignFieldName(*metaObject); }
    static QString foreignFieldName(const QMetaObject &metaObject);
    
    QSqlRecord toSqlRecord(bool includeTable = false) const;

    virtual QJsonObject toJsonObject() const;

    template<typename T>
    static inline QList<T> all()
    { return find<T>(DataSearch(QString(), 0, 0)); }
    
    template<typename T>
    static QList<T> find(const DataSearch &query)
    {
        QString request = selectStatement(T::staticMetaObject);
        request.append(' ' + query.whereClause());
        request.append(' ' + query.groupByClause());
        request.append(' ' + query.orderByClause());
        request.append(' ' + query.limitOffsetClause());
        request.replace("  ", " ");

        bool ok;
        QSqlQuery qu = execQuery(request.trimmed(), &ok);
        if (ok)
            return fromSqlQuery<T>(qu);
        else
            return QList<T>();
    }
    
    template<typename T>
    static QList<T> fromSqlQuery(QSqlQuery &query)
    {
        QList<T> data;
        while (query.next())
            data.append(fromSqlRecord<T>(query.record()));
        return data;
    }
    
    template<typename T>
    static QList<T> fromSqlRecords(const QList<QSqlRecord> &records)
    {
        QList<T> data;
        for (const QSqlRecord &record : records)
            data.append(fromSqlRecord<T>(record));
        return data;
    }
    
    template<typename T>
    static T fromSqlRecord(const QSqlRecord &record)
    {
        T data;
        data.extractRecord(record);
        return data;
    }

    bool operator==(const Data &other) const;
    inline bool operator!=(const Data &other) const
    { return !operator==(other); }
    
protected:
    Data(DataPrivate *data);
    Data(const QSharedDataPointer<DataPrivate> &other);

    template<typename T>
    void init()
    {
        metaObject = &T::staticMetaObject;
        qRegisterMetaType<T>();
    }

    virtual void fillRecord(QSqlRecord *record) const;
    virtual void extractRecord(const QSqlRecord &record);

    QSqlQuery exec(const QString &query, bool *ok = nullptr) const;
    static QSqlQuery execCachedQuery(const QString &query, bool *ok = nullptr, QSqlError *error = nullptr);
    static QSqlQuery execQuery(const QString &query, bool *ok = nullptr, QSqlError *error = nullptr, bool cached = false);

    QSharedDataPointer<DataPrivate> d_ptr;

private:
    S_DECLARE_PRIVATE(Data)

    inline QString selectStatement() const
    { return selectStatement(*metaObject); }
    static QString selectStatement(const QMetaObject &object);

    QString insertStatement() const;
    QString updateStatement() const;
    QString deleteStatement() const;
    static QSqlDriver *driver();
    
    void bindQueryValues(QSqlQuery &query) const;

    const QMetaObject *metaObject;

    template<typename MainData, typename ForeignData>
    friend class DataRelationship;

    friend class DataModel;

    friend class StandardUserInterface;
};

template<typename PrimaryData, typename ForeignData>
class SYSTEMUS_CORE_EXPORT DataRelationship : public QList<ForeignData>
{
public:
    enum RelationshipType {
        OneToOneRelationship,
        OneToManyRelationship,
        ManyToManyRelationship
    };

    virtual ~DataRelationship() = default;

    inline DataRelationship<PrimaryData, ForeignData> &operator=(const DataRelationship<PrimaryData, ForeignData> &other)
    {
        QList<ForeignData>::operator=(other);
        return *this;
    }

    bool get(const PrimaryData &primaryData) {
        bool ok;
        QSqlQuery query = primaryData.exec(selectStatement(primaryData), &ok);

        QList<ForeignData>::clear();
        while (query.next()) {
            ForeignData data = dataFromRecord(query.record());
            data.getExtras();
            QList<ForeignData>::append(data);
        }

        return ok;
    }

    virtual RelationshipType type() const = 0;

protected:
    virtual QString selectStatement(const PrimaryData &primaryData) const = 0;

    virtual ForeignData dataFromRecord(const QSqlRecord &record)
    { return Data::fromSqlRecord<ForeignData>(record); }

    template<typename T>
    QString selectStatementForData(const QString &filter) const
    {
        QString table = Data::tableName(T::staticMetaObject);
        QSqlRecord record = Data::tableRecord(T::staticMetaObject);
        QStringList filters;

        if (!filter.isEmpty())
            filters.append(filter);

        QString statement = Data::driver()->sqlStatement(QSqlDriver::SelectStatement, table, record, false);
        if (!filters.isEmpty())
            statement.append(" WHERE " + filters.join(" AND "));
        return statement;
    }

    QString selectStatementForForeignData(const QString &filter) const
    {
        QString table = Data::tableName(ForeignData::staticMetaObject);
        QSqlRecord record = Data::tableRecord(ForeignData::staticMetaObject);
        QStringList filters;

        if (!filter.isEmpty())
            filters.append(filter);

        QString statement = Data::driver()->sqlStatement(QSqlDriver::SelectStatement, table, record, false);
        if (!filters.isEmpty())
            statement.append(" WHERE " + filters.join(" AND "));
        return statement;
    }

    QString selectStatementForRecord(const QString &table, const QSqlRecord &record, const QList<QSqlRelation> &relations, const QString &filter) const
    {
        QString statement = Data::driver()->sqlStatement(QSqlDriver::SelectStatement, table, record, false);

        for (const QSqlRelation &relation : relations)
            statement.append(QStringLiteral(" INNER JOIN %1 ON %1.id = %2.%3").arg(relation.tableName(), table, relation.indexColumn()));

        QStringList filters;
        if (!filter.isEmpty())
            filters.append(filter);

        if (!filters.isEmpty())
            statement.append(" WHERE " + filters.join(" AND "));

        return statement;
    }
};

template<typename PrimaryData, typename ForeignData>
class OneToOneRelationship : public DataRelationship<PrimaryData, ForeignData>
{
public:
    OneToOneRelationship() :
        _foreignFieldName(Data::foreignFieldName(ForeignData::staticMetaObject))
    {}

    OneToOneRelationship(const QString &foreignField) :
        _foreignFieldName(foreignField)
    {}

    OneToOneRelationship(const OneToOneRelationship<PrimaryData, ForeignData> &other) = default;

    operator ForeignData() const
    { return object(); }

    ForeignData object() const
    { return (!QList<ForeignData>::isEmpty() ? QList<ForeignData>::at(0) : ForeignData()); }

    inline OneToOneRelationship<PrimaryData, ForeignData> &operator=(const OneToOneRelationship<PrimaryData, ForeignData> &other)
    {
        DataRelationship<PrimaryData, ForeignData>::operator=(other);
        return *this;
    }

    typename DataRelationship<PrimaryData, ForeignData>::RelationshipType type() const override
    { return DataRelationship<PrimaryData, ForeignData>::OneToOneRelationship; }

protected:
    virtual QString selectStatement(const PrimaryData &primaryData) const override
    {
        QString table = Data::tableName(ForeignData::staticMetaObject);

        QSqlRecord record = Data::tableRecord(ForeignData::staticMetaObject, true);

        QSqlRelation relation(Data::tableName(PrimaryData::staticMetaObject), Data::foreignFieldName(ForeignData::staticMetaObject), QString());

        QString filter = QStringLiteral("id = %1").arg(primaryData.property(relation.indexColumn()).toInt());

        return this->selectStatementForForeignData(filter);
    }

private:
    const QString _foreignFieldName;
};

template<typename PrimaryData, typename ForeignData>
class OneToManyRelationship : public DataRelationship<PrimaryData, ForeignData>
{
public:
    OneToManyRelationship() :
        _foreignFieldName(Data::foreignFieldName(PrimaryData::staticMetaObject))
    {}

    OneToManyRelationship(const QString &foreignField) :
        _foreignFieldName(foreignField)
    {}

    OneToManyRelationship(const OneToManyRelationship<PrimaryData, ForeignData> &other) = default;

    inline OneToManyRelationship<PrimaryData, ForeignData> &operator=(const OneToManyRelationship<PrimaryData, ForeignData> &other)
    {
        DataRelationship<PrimaryData, ForeignData>::operator=(other);
        return *this;
    }

    typename DataRelationship<PrimaryData, ForeignData>::RelationshipType type() const override
    { return DataRelationship<PrimaryData, ForeignData>::OneToManyRelationship; }

protected:
    QString selectStatement(const PrimaryData &primaryData) const override
    {
        QString filter = QStringLiteral("%1 = %2").arg(_foreignFieldName).arg(primaryData.id());
        return this->selectStatementForForeignData(filter);
    }

private:
    const QString _foreignFieldName;
};

template<typename PrimaryData, typename ForeignData>
class ManyToManyRelationship : public DataRelationship<PrimaryData, ForeignData>
{
public:
    ManyToManyRelationship(const QStringList &junctionFields) :
        ManyToManyRelationship(Data::tableName(PrimaryData::staticMetaObject).chopped(1) + Data::tableName(ForeignData::staticMetaObject), junctionFields)
    {}

    ManyToManyRelationship(const QString &junctionTable, const QStringList &junctionFields) :
        _junctionTable(junctionTable),
        _junctionFields((QStringList() << Data::foreignFieldName(ForeignData::staticMetaObject)) + junctionFields)
    {}

    ManyToManyRelationship(const ManyToManyRelationship<PrimaryData, ForeignData> &other) = default;

    QStringList junctionFields() const
    { return _junctionFields; }

    QVariantHash junctionData(int index) const
    { return (index < _junctionData.size() ? _junctionData.at(index) : QVariantHash()); }

    inline ManyToManyRelationship<PrimaryData, ForeignData> &operator=(const ManyToManyRelationship<PrimaryData, ForeignData> &other)
    {
        DataRelationship<PrimaryData, ForeignData>::operator=(other);
        return *this;
    }

    void clear()
    {
        QList<ForeignData>::clear();
        _junctionData.clear();
    }

    typename DataRelationship<PrimaryData, ForeignData>::RelationshipType type() const override
    { return DataRelationship<PrimaryData, ForeignData>::ManyToManyRelationship; }

protected:
    QString selectStatement(const PrimaryData &primaryData) const override
    {
        QString table = _junctionTable;

        QSqlRecord record = Data::tableRecord(ForeignData::staticMetaObject, true);

        QSqlField idField = record.field(0);
        idField.setName(_junctionTable + ".id");
        record.replace(0, idField);

        for (const QString &field : _junctionFields)
            record.append(QSqlField(field, QMetaType()));

        QSqlRelation relation(Data::tableName(ForeignData::staticMetaObject), Data::foreignFieldName(ForeignData::staticMetaObject), QString());

        QString filter = QStringLiteral("%1 = %2").arg(Data::foreignFieldName(PrimaryData::staticMetaObject)).arg(primaryData.id());

        return this->selectStatementForRecord(table, record, QList<QSqlRelation>() << relation, filter);
    }

    ForeignData dataFromRecord(const QSqlRecord &record) override
    {
        QVariantHash data;
        for (const QString &field : _junctionFields)
            data.insert(field, record.value(field));
        _junctionData.append(data);

        return DataRelationship<PrimaryData, ForeignData>::dataFromRecord(record);
    }

private:
    const QString _junctionTable;
    QStringList _junctionFields;
    const QString _foreignFieldName;

    QList<QVariantHash> _junctionData;
};

class AuthorizationDataPrivate;
class SYSTEMUS_CORE_EXPORT AuthorizationData : public Data
{
    Q_GADGET
    Q_PROPERTY(QString name READ name WRITE setName USER true)
    Q_PROPERTY(QString description READ description WRITE setDescription)
    Q_PROPERTY(QDate creationDate READ creationDate)
    Q_PROPERTY(QTime creationTime READ creationTime)

public:
    AuthorizationData();
    AuthorizationData(const AuthorizationData &other);
    virtual ~AuthorizationData();

    QString name() const;
    void setName(const QString &name);

    QString description() const;
    void setDescription(const QString &description);

    QDate creationDate() const;
    QTime creationTime() const;

    bool isValid() const override;

protected:
    AuthorizationData(AuthorizationDataPrivate *data);
};

class IssuedAuthorizationDataPrivate;
class SYSTEMUS_CORE_EXPORT IssuedAuthorizationData : public Data
{
    Q_GADGET
    Q_PROPERTY(QString name READ name USER true STORED false)
    Q_PROPERTY(QString description READ description STORED false)
    Q_PROPERTY(bool active READ isActive WRITE setActive)
    Q_PROPERTY(QDate issueDate READ issueDate)
    Q_PROPERTY(QTime issueTime READ issueTime)

public:
    IssuedAuthorizationData();
    IssuedAuthorizationData(const IssuedAuthorizationData &other);
    virtual ~IssuedAuthorizationData();

    QString name() const;
    QString description() const;

    bool isActive() const;
    void setActive(bool active = true);

    QDate issueDate() const;
    QTime issueTime() const;

    AuthorizationData referenceData() const;
    void setReferenceData(const AuthorizationData &data);

protected:
    IssuedAuthorizationData(IssuedAuthorizationDataPrivate *data);
};

}

Q_DECLARE_METATYPE(Systemus::Data)
Q_DECLARE_METATYPE(Systemus::AuthorizationData)
Q_DECLARE_METATYPE(Systemus::IssuedAuthorizationData)

#endif // SYSTEMUS_DATA_H
