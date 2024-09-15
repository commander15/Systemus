#ifndef SYSTEMUS_DATARELATION_H
#define SYSTEMUS_DATARELATION_H

#include <SystemusCore/global.h>
#include <SystemusCore/data.h>
#include <SystemusCore/ormbackend.h>

#include <QtSql/qsqlquery.h>
#include <QtSql/qsqlrecord.h>
#include <QtSql/qsqlfield.h>

#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>

namespace Systemus {
namespace Orm {

class SYSTEMUS_CORE_EXPORT AbstractDataRelation
{
public:
    enum RelationshipType {
        OneToOneRelationship,
        OneToManyRelationship,
        ManyToOneRelationship,
        ManyToManyRelationship
    };

    virtual bool isEmpty() const = 0;
    virtual void clear() = 0;

    void dump() const;

    virtual QJsonValue toJson() const = 0;

    bool get(Data *primary);
    bool get(Data *primary, const QString &foreignProperty);
    bool get(Data *primary, const QString &foreignProperty, const QString &indexProperty);

    bool save(Data *primary);
    bool save(Data *primary, const QString &foreignProperty);
    bool save(Data *primary, const QString &foreignProperty, const QString &indexProperty);

    bool update(Data *primary);
    bool update(Data *primary, const QString &foreignProperty);
    bool update(Data *primary, const QString &foreignProperty, const QString &indexProperty);

    bool insert(Data *primary);
    bool insert(Data *primary, const QString &foreignProperty);
    bool insert(Data *primary, const QString &foreignProperty, const QString &indexProperty);

    bool deleteData(Data *primary);
    bool deleteData(Data *primary, const QString &foreignProperty);
    bool deleteData(Data *primary, const QString &foreignProperty, const QString &indexProperty);

    virtual QString foreignClassName() const = 0;
    QString foreignPropertyName(const Data *primary) const;
    virtual QString foreignTableName() const;
    MetaTable foreignTable() const;

    virtual QString indexPropertyName(const Data *primary) const;

    virtual int relationshipType() const = 0;

protected:
    bool getAllData(Data *primary, const QString &foreignProperty, const QString &indexProperty);
    virtual bool saveAllData(Data *primary, const QString &foreignProperty, const QString &indexProperty) = 0;
    virtual bool updateAllData(Data *primary, const QString &foreignProperty, const QString &indexProperty) = 0;
    virtual bool insertAllData(Data *primary, const QString &foreignProperty, const QString &indexProperty) = 0;
    bool deleteAllData(Data *primary, const QString &foreignProperty, const QString &indexProperty);

    QString selectStatement(Data *primary, const QString &foreignProperty, const QString &indexProperty) const;
    QString deleteStatement(Data *primary) const;

    virtual QString joinClause(Data *primary, const QString &foreignProperty, const QString &indexProperty) const;
    QString whereClause(Data *primary, const QString &foreignProperty, const QString &indexProperty) const;

    virtual QString filterTableName(Data *primary) const;
    virtual QSqlRecord filterRecord(Data *primary, const QString &foreignProperty, const QString &indexProperty) const;

    QSqlQuery exec(const QString &query, bool *ok, Data *primary) const;

    virtual void processReceivedRecord(const QSqlRecord &record) = 0;
};

template<typename ForeignData>
class DataRelation : public AbstractDataRelation
{
public:
    QString foreignClassName() const override final
    { return ForeignData::staticMetaObject.className(); }
};

template<typename ForeignData>
class SingleRelation : public DataRelation<ForeignData>
{
public:
    SingleRelation() :
        m_data(nullptr)
    {}

    SingleRelation(const SingleRelation<ForeignData> &other) :
        m_data(other.m_data ? new ForeignData(*other.m_data) : nullptr)
    {}

    virtual ~SingleRelation()
    { if (m_data) delete m_data; }

    ForeignData &operator=(const ForeignData &other)
    { return *this->data() = other; }

    SingleRelation<ForeignData> &operator=(const SingleRelation<ForeignData> &other)
    { *this->data() = *other.data(); return *this; }

    bool isEmpty() const override
    { return this->data()->isEmpty(); }

    void clear() override
    { this->data()->clear(); }

    QJsonValue toJson() const override
    { return this->data()->toJsonObject(); }

    const ForeignData *data() const
    { return (m_data ? m_data : m_data = new ForeignData()); }

    ForeignData *data()
    { return (m_data ? m_data : m_data = new ForeignData()); }

    operator bool() const
    { return m_data != nullptr; }

    operator const ForeignData&() const
    { return *this->data();}

    operator ForeignData&()
    { return *this->data();}

    const ForeignData *operator->() const
    { return data(); }

    ForeignData *operator->()
    { return data(); }

    const ForeignData &operator*() const
    { return data(); }

    ForeignData &operator*()
    { return data(); }

protected:
    bool saveAllData(Data *primary, const QString &foreignProperty, const QString &indexProperty) override
    {
        prepareSave(this->data(), primary, foreignProperty, indexProperty);
        return this->data()->save();
    }

    bool insertAllData(Data *primary, const QString &foreignProperty, const QString &indexProperty) override
    {
        prepareSave(this->data(), primary, foreignProperty, indexProperty);
        return this->data()->insert();
    }

    bool updateAllData(Data *primary, const QString &foreignProperty, const QString &indexProperty) override
    { return this->data()->update(); }

    void processReceivedRecord(const QSqlRecord &record) override
    { *this->data() = ForeignData::fromSqlRecord(record); }

    void prepareSave(Data *data, Data *primary, const QString &foreignProperty, const QString &indexProperty)
    { data->setProperty(indexProperty, primary->property(foreignProperty)); }

private:
    mutable ForeignData *m_data;
};

template<typename ForeignData>
class MultiRelation : public DataRelation<ForeignData>, public QList<ForeignData>
{
public:
    MultiRelation() = default;
    MultiRelation(const MultiRelation<ForeignData> &other) = default;

    QList<ForeignData> &operator=(const QList<ForeignData> &other)
    { return QList<ForeignData>::operator=(other); }

    MultiRelation<ForeignData> &operator=(const MultiRelation<ForeignData> &other)
    { QList<ForeignData>::operator=(other); return *this; }

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

    bool operator==(const MultiRelation<ForeignData> &other) const
    { return QList<ForeignData>::operator==(other); }
    inline bool operator!=(const MultiRelation<ForeignData> &other) const
    { return !operator==(other); }

protected:
    bool saveAllData(Data *primary, const QString &foreignProperty, const QString &indexProperty) override
    {
        for (ForeignData &data : *this) {
            prepareSave(&data, primary, foreignProperty, indexProperty);
            if (!data.save())
                return false;
        }
        return true;
    }

    bool insertAllData(Data *primary, const QString &foreignProperty, const QString &indexProperty) override
    {
        for (ForeignData &data : *this) {
            prepareSave(&data, primary, foreignProperty, indexProperty);
            if (!data.insert())
                return false;
        }
        return true;
    }

    bool updateAllData(Data *primary, const QString &foreignProperty, const QString &indexProperty) override
    {
        for (ForeignData &data : *this) {
            prepareSave(&data, primary, foreignProperty, indexProperty);
            if (!data.update())
                return false;
        }
        return true;
    }

    virtual void prepareSave(Data *data, Data *primary, const QString &foreignProperty, const QString &indexProperty)
    { Q_UNUSED(data); Q_UNUSED(primary); Q_UNUSED(foreignProperty); Q_UNUSED(indexProperty); }

    void processReceivedRecord(const QSqlRecord &record) override
    { QList<ForeignData>::append(ForeignData::fromSqlRecord(record, true)); }
};

// One To One Relationship

template<typename ForeignData>
class OneToOneRelation : public SingleRelation<ForeignData>
{
public:
    OneToOneRelation() = default;
    OneToOneRelation(const OneToOneRelation<ForeignData> &other) = default;

    ForeignData &operator=(const ForeignData &other)
    { return SingleRelation<ForeignData>::operator=(other); }

    OneToOneRelation<ForeignData> &operator=(const OneToOneRelation<ForeignData> &other)
    { SingleRelation<ForeignData>::operator=(other); return *this; }

    int relationshipType() const override
    { return DataRelation<ForeignData>::OneToOneRelationship; }
};

// One To Many Relationship

template<typename ForeignData>
class OneToManyRelation : public MultiRelation<ForeignData>
{
public:
    OneToManyRelation() = default;
    OneToManyRelation(const OneToManyRelation<ForeignData> &other) = default;

    QList<ForeignData> &operator=(const QList<ForeignData> &other)
    { return QList<ForeignData>::operator=(other); }

    OneToManyRelation<ForeignData> &operator=(const OneToManyRelation<ForeignData> &other)
    { MultiRelation<ForeignData>::operator=(other); return *this; }

    int relationshipType() const override
    { return DataRelation<ForeignData>::OneToManyRelationship; }
};

// Many To One Relationship

template<typename ForeignData>
class ManyToOneRelation : public SingleRelation<ForeignData>
{
public:
    ForeignData &operator=(const ForeignData &other)
    { return SingleRelation<ForeignData>::operator=(other); }

    ManyToOneRelation<ForeignData> &operator=(const ManyToOneRelation<ForeignData> &other)
    { SingleRelation<ForeignData>::operator=(other); return *this; }

    int relationshipType() const override
    { return DataRelation<ForeignData>::ManyToOneRelationship; }
};

// Many To Many Relationship

template<typename ForeignData>
class ManyToManyRelation : public MultiRelation<ForeignData>
{
public:
    ManyToManyRelation() = default;
    ManyToManyRelation(const QString &className) : m_className(className) {}
    ManyToManyRelation(const ManyToManyRelation<ForeignData> &data) = default;

    QList<ForeignData> &operator=(const QList<ForeignData> &other)
    { return QList<ForeignData>::operator=(other); }

    ManyToManyRelation<ForeignData> &operator=(const ManyToManyRelation<ForeignData> &other)
    { m_className = other.m_className; MultiRelation<ForeignData>::operator=(other); return *this; }

    int relationshipType() const override
    { return DataRelation<ForeignData>::ManyToManyRelationship; }

protected:
    QString joinClause(Data *primary, const QString &foreignProperty, const QString &indexProperty) const override
    {
        Q_UNUSED(indexProperty);

        const MetaTable primaryTable = primary->metaTable();
        const MetaTable foreignTable = this->foreignTable();
        const Backend *backend = Backend::instance();

        QString associationTableName;
        if (!this->m_className.isEmpty())
            associationTableName = backend->tableNameFromClassName(this->m_className);
        else
            associationTableName = backend->associationTableNameFromClassNames(primaryTable.className(), foreignTable.className());

        QString join = QStringLiteral("INNER JOIN %1 ON (%1.%2 = %3.%4)");
        join = join.arg(associationTableName, backend->fieldNameFromPropertyName(foreignProperty, foreignTable.className()));
        join = join.arg(foreignTable.tableName(), foreignTable.primaryFieldName());
        return join;
    }

    QSqlRecord filterRecord(Data *primary, const QString &foreignProperty, const QString &indexProperty) const override
    {
        QSqlRecord record = MultiRelation<ForeignData>::filterRecord(primary, foreignProperty, this->foreignTable().primaryFieldName());
        QSqlField field = record.field(0);
        return record;
    }

private:
    QString m_className;
};

}
}

#endif // SYSTEMUS_DATARELATION_H
