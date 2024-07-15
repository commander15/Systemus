#ifndef SYSTEMUS_DATARELATION_H
#define SYSTEMUS_DATARELATION_H

#include <SystemusCore/global.h>
#include <SystemusCore/data.h>

#include <QtSql/qsqlrecord.h>
#include <QtSql/qsqlfield.h>

#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>

namespace Systemus {

class SYSTEMUS_CORE_EXPORT AbstractDataRelation
{
public:
    enum RelationshipType {
        OneToOneRelationship,
        OneToManyRelationship,
        ManyToOneRelationship,
        ManyToManyRelationship
    };

    QString name() const;
    void setName(const QString &name);

    virtual bool isEmpty() const = 0;
    virtual void clear() = 0;

    void dumpInfo() const;

    virtual QJsonValue toJson() const = 0;

    bool get(const Data *primary);
    bool get(const Data &primary);

    virtual int relationshipType() const = 0;

protected:
    virtual void processRecord(const QSqlRecord &record) = 0;

    virtual QString selectStatement(const Data &primary) const = 0;

    QString generateSelectStatement(const QString &table, const QSqlRecord &record) const;
    QString generateSelectStatement(const QString &table, const QSqlRecord &record, const QString &filter) const;

    QString _name;
};

template<typename ForeignData>
class SYSTEMUS_CORE_EXPORT DataRelation : public AbstractDataRelation
{
public:
    bool operator==(const DataRelation<ForeignData> &other) const
    { return _name == other._name; }
    inline bool operator!=(const DataRelation<ForeignData> &other) const
    { return !operator==(other); }

protected:
    virtual ForeignData dataFromRecord(const QSqlRecord &record)
    { return Data::fromSqlRecord<ForeignData>(record); }

    virtual void saveData(const ForeignData &data) = 0;

    void processRecord(const QSqlRecord &record) override
    { saveData(dataFromRecord(record)); }
};

template<typename ForeignData>
class SingleRelation : public DataRelation<ForeignData>
{
public:
    operator const ForeignData &() const
    { return _data; }

    operator ForeignData &()
    { return _data; }

    ForeignData &operator*()
    { return _data; }

    const ForeignData &operator*() const
    { return _data; }

    const ForeignData *operator->() const
    { return &_data; }

    ForeignData *operator->()
    { return &_data; }

    ForeignData data() const
    { return _data; }

    void setData(const ForeignData &data)
    { _data = data; }

    SingleRelation<ForeignData> &operator=(const SingleRelation<ForeignData> &other)
    { _data = other._data; DataRelation<ForeignData>::operator=(other); return *this; }

    SingleRelation<ForeignData> &operator=(const ForeignData &other)
    { _data = other._data; return *this; }

    bool isEmpty() const override
    { return _data.isEmpty(); }

    void clear() override
    { _data.clear(); }

    QJsonValue toJson() const override
    { return _data.toJsonObject(); }

protected:
    void saveData(const ForeignData &data) override
    { _data = data; }

    QString selectStatement(const Data &primary) const override
    {
        const DataInfo foreignInfo = DataInfo::fromType<ForeignData>();

        QSqlRecord record = foreignInfo.record();

        const QString property = foreignInfo.foreignPropertyName();
        const QString filter = QStringLiteral("id = %1").arg(primary.property(property).toInt());

        return this->generateSelectStatement(foreignInfo.tableName(), record, filter);
    }

private:
    ForeignData _data;
};

template<typename ForeignData>
class MultiRelation : public DataRelation<ForeignData>, public QList<ForeignData>
{
public:
    bool operator==(const MultiRelation<ForeignData> &other) const
    { return DataRelation<ForeignData>::operator==(other) && QList<ForeignData>::operator==(other); }
    inline bool operator!=(const MultiRelation<ForeignData> &other) const
    { return !operator==(other); }

    bool isEmpty() const override
    { return QList<ForeignData>::isEmpty(); }

    void clear() override
    { QList<ForeignData>::clear(); }

    QJsonValue toJson() const override
    {
        QJsonArray all;
        for (const ForeignData &data : *this)
            all.append(data.toJsonObject());
        return all;
    }

protected:
    void saveData(const ForeignData &data) override
    { QList<ForeignData>::append(data); }
};

// One To One Relationship

template<typename ForeignData>
class OneToOneRelation : public SingleRelation<ForeignData>
{
public:
    int relationshipType() const override
    { return DataRelation<ForeignData>::OneToManyRelationship; }
};

// One To Many Relationship

template<typename ForeignData>
class OneToManyRelation : public MultiRelation<ForeignData>
{
public:
    int relationshipType() const override
    { return DataRelation<ForeignData>::OneToManyRelationship; }

protected:
    QString selectStatement(const Data &primary) const override
    {
        const DataInfo info = primary.dataInfo();
        const DataInfo foreignInfo = DataInfo::fromType<ForeignData>();

        const QString filter = QStringLiteral("%1 = %2").arg(info.foreignFieldName()).arg(primary.id());
        return this->generateSelectStatement(foreignInfo.tableName(), foreignInfo.record(), filter);
    }
};

// Many To One Relationship

template<typename ForeignData>
class ManyToOneRelation : public SingleRelation<ForeignData>
{
public:
    int relationshipType() const override
    { return DataRelation<ForeignData>::ManyToOneRelationship; }
};

// Many To Many Relationship

template<typename ForeignData>
class ManyToManyRelation : public MultiRelation<ForeignData>
{
public:
    ManyToManyRelation() = default;

    ManyToManyRelation(const QString &table, const QStringList &fields) :
        _junctionTable(table),
        _junctionFields(fields)
    { _junctionFields.prepend(DataInfo::fromType<ForeignData>().foreignFieldName()); }

    ManyToManyRelation(const ManyToManyRelation<ForeignData> &data) = default;

    QVariant junctionData(const QString &field, int index) const
    { return _junctionData.at(index).value(field); }

    QVariantHash junctionData(int index) const
    { return _junctionData.at(index); }

    void clear() override
    { _junctionData.clear(); MultiRelation<ForeignData>::clear(); }

    int relationshipType() const override
    { return DataRelation<ForeignData>::ManyToManyRelationship; }

protected:
    ForeignData dataFromRecord(const QSqlRecord &record) override
    {
        QVariantHash data;
        for (const QString &field : _junctionFields)
            data.insert(field, record.value(field));
        _junctionData.append(data);
        return MultiRelation<ForeignData>::dataFromRecord(record);
    }

    QString selectStatement(const Data &primary) const override
    {
        const DataInfo info = primary.dataInfo();
        const DataInfo foreignInfo = DataInfo::fromType<ForeignData>();

        QSqlRecord record = foreignInfo.record();
        {
            QSqlField field = record.field(0);
            field.setName(foreignInfo.tableName() + '.' + field.name());
            record.replace(0, field);

            const QList<QSqlField> fields = DataInfo::fieldsFromString(_junctionFields.join(','), _junctionTable, this->name());
            for (const QSqlField &field : fields)
                record.append(field);
        }

        QString filter = QStringLiteral("%1 = %2").arg(info.foreignFieldName()).arg(primary.id());
        QString join = QStringLiteral("%1 ON %1.%2 = %3.%4").arg(foreignInfo.tableName(), foreignInfo.idFieldName(), _junctionTable, foreignInfo.foreignFieldName());
        QString statement = this->generateSelectStatement(_junctionTable, record, QString());
        return statement + " LEFT  JOIN " + join + " WHERE " + filter;
    }

private:
    QString _junctionTable;
    QStringList _junctionFields;
    QList<QVariantHash> _junctionData;
};

/*
class DataRelationImpl;
class SYSTEMUS_CORE_EXPORT DataRelation
{
public:
    enum RelationshipType {
        OneToOneRelationship,
        OneToManyRelationship,
        ManyToManyRelationship
    };

    DataRelation();
    DataRelation(const DataRelation &other);
    ~DataRelation();

    DataRelation &operator=(const DataRelation &other);

    QString name() const;

    QString foreignFieldName() const;

    Data data(int index) const;

    QStringList propertyNames() const;
    QVariant property(const QString &name, int index) const;
    QVariant property(const QString &name, int index, const QVariant &defaultValue) const;
    void setProperty(const QString &name, int index, const QVariant &value);

    QJsonValue toJson() const;

    bool isEmpty() const;
    int count() const;
    void clear();

    bool get(const Data &primary);
    bool get(const Data &primary, int index);

    RelationshipType type() const;

    bool operator==(const DataRelation &other) const;
    inline bool operator!=(const DataRelation &other) const
    { return !operator==(other); }

protected:
    DataRelation(DataRelationImpl *d);

    QSharedDataPointer<DataRelationImpl> d;
};

class SYSTEMUS_CORE_EXPORT DataRelationImpl : public QSharedData
{
public:
    DataRelationImpl(const QString &name);
    DataRelationImpl(const DataRelationImpl &other);
    virtual ~DataRelationImpl();

    DataRelationImpl &operator=(const DataRelationImpl &other)
    { _name = other._name; return *this; }

    QString name() const;

    virtual QString foreignFieldName() const;

    virtual Data data(int index) const = 0;

    virtual QStringList propertyNames() const = 0;
    virtual QVariant property(const QString &name, int index) const = 0;
    virtual void setProperty(const QString &name, int index, const QVariant &data) = 0;

    virtual int count() const = 0;
    virtual void clear() = 0;

    virtual DataRelationImpl *clone() const = 0;

    virtual bool equals(const DataRelationImpl *other) const = 0;

    virtual QJsonValue toJson() const;

    bool get(const Data &primary, int index);

    virtual int type() const = 0;

protected:
    virtual void processRecord(const QSqlRecord &record) = 0;

    virtual QString selectStatement(const Data &primary, int index) const = 0;

private:
    QString _name;
};

template<typename ForeignData>
class SYSTEMUS_CORE_EXPORT DataRelationshipImpl : public DataRelationImpl
{
public:
    DataRelationshipImpl() :
        DataRelationImpl("relation")
    {}

    DataRelationshipImpl(const DataRelationshipImpl &other) = default;

    DataRelationshipImpl<ForeignData> &operator=(const DataRelationshipImpl<ForeignData> &other)
    { DataRelationImpl::operator=(other); return *this; }

    QString foreignFieldName() const override
    { return DataInfo::fromType<ForeignData>().foreignFieldName(); }

    QStringList propertyNames() const override
    {
        QStringList properties;
        const DataInfo info = DataInfo::fromType<ForeignData>();
        for (int i(0); i < info.fieldCount(); ++i)
            properties.append(info.propertyName(i));
        return properties;
    }

protected:
    void processRecord(const QSqlRecord &record) override
    {
        ForeignData data = Data::fromSqlRecord<ForeignData>(record);
        data.getExtras();
        saveData(data);
    }

    virtual void saveData(const ForeignData &data) = 0;

    QString selectStatementForForeignData(const QString &filter) const
    { return selectStatementForData<ForeignData>(filter); }

    template<typename T>
    QString selectStatementForData(const QString &filter) const
    {
        const DataInfo info = DataInfo::fromType<ForeignData>();
        return selectStatementForRecord(info.tableName(), info.record(), QList<QSqlRelation>(), filter);
    }

    QString selectStatementForRecord(const QString &table, const QSqlRecord &record, const QList<QSqlRelation> &relations, const QString &filter) const
    {
        QStringList filters;
        if (!filter.isEmpty())
            filters.append(filter);

        QString statement = Data::driver()->sqlStatement(QSqlDriver::SelectStatement, table, record, false);
        if (!filters.isEmpty())
            statement.append(" WHERE " + filters.join(" AND "));
        return statement;
    }
};

template<typename PrimaryData, typename ForeignData, typename RelationshipImpl>
class DataRelationship : public DataRelationshipImpl<ForeignData>
{
public:
    template<typename... Args>
    DataRelationship(Args... args)
    { d = new RelationshipImpl(std::forward<Args>(args) ...); }

    ForeignData operator->() const
    { return d->data(0); }

    ForeignData &operator->()
    { return d->data(0); }

    operator ForeignData() const
    { return d->data(0); }

    ForeignData object() const
    { return d->data(0); }

    ForeignData at(int index) const
    { return d->data(index); }

    ForeignData operator[](int index) const
    { return d->data(index); }

    ForeignData &operator[](int index)
    { return d->data(index); }

    QString foreignFieldName() const override;

    Data data(int index) const override;

    QStringList propertyNames() const override;
    QVariant property(const QString &name, int index) const override;
    void setProperty(const QString &name, int index, const QVariant &data) override;

    int count() const override;
    void clear() override;

    DataRelationImpl *clone() const override;

    bool equals(const DataRelationImpl *other) const override;

    QJsonValue toJson() const override;

    int type() const override;

protected:
    void processRecord(const QSqlRecord &record) override;
    QString selectStatement(const Data &primary, int index) const override;

private:
    QSharedDataPointer<RelationshipImpl> d;
};

template<typename ForeignData>
class SYSTEMUS_CORE_EXPORT OneToOneRelationshipImpl : public DataRelationshipImpl<ForeignData>
{
public:
    OneToOneRelationshipImpl() :
        OneToOneRelationshipImpl<ForeignData>(DataInfo::fromType<ForeignData>().foreignFieldName())
    {}

    OneToOneRelationshipImpl(const QString &foreignField) :
        _foreignFieldName(_foreignFieldName)
    {}

    OneToOneRelationshipImpl(const OneToOneRelationshipImpl<ForeignData> &other) = default;

    ForeignData operator->() const
    { return _data; }

    ForeignData &operator->()
    { return _data; }

    operator ForeignData() const
    { return _data; }

    ForeignData object() const
    { return _data; }

    Data data(int index) const override
    { Q_UNUSED(index); return _data; }

    QVariant property(const QString &name, int index) const override
    { Q_UNUSED(index); return _data.property(name); }

    void setProperty(const QString &name, int index, const QVariant &value) override
    { Q_UNUSED(index); _data.setProperty(name, value); }

    int count() const override
    { return (_data.isValid() ? 1 : 0); }

    void clear() override
    { _data.clear(); }

    DataRelationImpl *clone() const override
    { return new OneToOneRelationshipImpl<ForeignData>(*this); }

    bool equals(const DataRelationImpl *o) const override
    {
        auto other = static_cast<const OneToOneRelationshipImpl<ForeignData> *>(o);
        return _data == other->_data && _foreignFieldName == other->_foreignFieldName;
    }

    int type() const override
    { return DataRelation::OneToOneRelationship; }

protected:
    void processsRecord(const QSqlRecord &record) override
    { _data = Data::fromSqlRecord<ForeignData>(record); }

    QString selectStatement(const Data &primary, int index) const override
    { return selectStatement(QStringLiteral("id = %1").arg(primary.id())); }

private:
    ForeignData _data;
    const QString _foreignFieldName;
};

template<typename ForeignData>
class MultiDataRelationship : public DataRelationshipImpl<ForeignData>, public QList<ForeignData>
{
public:
    MultiDataRelationship(const QString &foreignFieldName) :
        _foreignFieldName(foreignFieldName)
    {}

    MultiDataRelationship(const MultiDataRelationship<ForeignData> &other)
    { DataRelationshipImpl<ForeignData>::operator=(other); }

    virtual ~MultiDataRelationship() = default;

    MultiDataRelationship<ForeignData> &operator=(const MultiDataRelationship<ForeignData> &other)
    { DataRelationshipImpl<ForeignData>::operator=(other); QList<ForeignData>::operator=(other); return *this; }

    QString foreignFieldName() const override
    { return _foreignFieldName; }

    Data data(int index) const override
    { return QList<ForeignData>::at(index); }

    QVariant property(const QString &name, int index) const override
    { return QList<ForeignData>::at(index).property(name); }
    void setProperty(const QString &name, int index, const QVariant &data) override
    { QList<ForeignData>::operator[](index).setProperty(name, data); }

    int count() const override
    { return QList<ForeignData>::count(); }
    void clear() override
    { QList<ForeignData>::clear(); }
    bool equals(const DataRelationImpl *o) const override
    {
        auto other = static_cast<const MultiDataRelationship<ForeignData> *>(o);
        return QList<ForeignData>::operator==(*other)
               && _foreignFieldName == other->_foreignFieldName;
    }

protected:
    void saveData(const ForeignData &data) override
    { QList<ForeignData>::append(data); }

    const QString _foreignFieldName;
};

template<typename ForeignData>
class OneToManyRelationshipImpl : public MultiDataRelationship<ForeignData>
{
public:
    OneToManyRelationshipImpl(const QString &foreignFieldName) :
        MultiDataRelationship<ForeignData>(foreignFieldName)
    {}

    OneToManyRelationshipImpl(const OneToManyRelationshipImpl<ForeignData> &other) = default;

    OneToManyRelationshipImpl<ForeignData> &operator=(const OneToManyRelationshipImpl<ForeignData> &other)
    { MultiDataRelationship<ForeignData>::operator=(other); return *this; }

    DataRelationImpl *clone() const override
    { return new OneToManyRelationshipImpl<ForeignData>(*this); }

    int type() const override
    { return DataRelation::OneToManyRelationship; }

protected:
    QString selectStatement(const Data &primary, int index) const override
    {
        QString filter = QStringLiteral("%1 = %2").arg(primary.dataInfo().foreignFieldName()).arg(primary.id());
        return this->selectStatementForForeignData(filter);
    }
};

template<typename ForeignData>
class ManyToManyRelationshipImpl : public MultiDataRelationship<ForeignData>
{
public:
    ManyToManyRelationshipImpl(const QString &table, const QStringList &fields) :
        _junctionTable(table),
        _junctionFields(DataInfo::fromType<ForeignData>().foreignFieldName() + fields)
    {}

    ManyToManyRelationshipImpl(const ManyToManyRelationshipImpl<ForeignData> &other) = default;

    QStringList propertyNames() const override
    { return MultiDataRelationship<ForeignData>::propertyNames() + _junctionFields; }

    QVariant property(const QString &name, int index) const override
    {
        int dataIndex = _junctionFields.indexOf(name);
        if (dataIndex >= 0)
            return _junctionData.at(index).value(name);
        else
            return MultiDataRelationship<ForeignData>::property(name, index);
    }

    void setProperty(const QString &name, int index, const QVariant &value) override
    {
        int dataIndex = _junctionFields.indexOf(name);
        if (dataIndex >= 0)
            _junctionData[dataIndex].insert(name, value);
        else
            MultiDataRelationship<ForeignData>::setProperty(name, index, value);
    }

    DataRelationImpl *clone() const override
    { return new ManyToManyRelationshipImpl<ForeignData>(*this); }

    int type() const override
    { return DataRelation::ManyToManyRelationship; }

protected:
    void processRecord(const QSqlRecord &record) override
    {
        QVariantHash data;
        for (const QString &field : _junctionFields)
            data.insert(field, record.value(field));
        _junctionData.append(data);

        MultiDataRelationship<ForeignData>::processRecord(record);
    }

    QString selectStatement(const Data &primary, int index) const override
    {
        QString table = _junctionTable;

        const DataInfo foreignInfo = DataInfo::fromType<ForeignData>();

        QSqlRecord record = foreignInfo.record();

        QSqlField idField = record.field(0);
        idField.setName(_junctionTable + ".id");
        record.replace(0, idField);

        for (const QString &field : _junctionFields)
            record.append(QSqlField(field, QMetaType()));

        QSqlRelation relation(foreignInfo.tableName(), foreignInfo.foreignFieldName(), QString());

        QString filter = QStringLiteral("%1 = %2").arg(foreignInfo.foreignFieldName()).arg(primary.id());

        return this->selectStatementForRecord(table, record, QList<QSqlRelation>() << relation, filter);
    }

    const QString _junctionTable;
    const QStringList _junctionFields;
    QList<QVariantHash> _junctionData;
};
*/

}

#endif // SYSTEMUS_DATARELATION_H

//#include <SystemusCore/datarelation_impl.h>
