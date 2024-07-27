#ifndef SYSTEMUS_DATARELATION_H
#define SYSTEMUS_DATARELATION_H

#include <SystemusCore/global.h>
#include <SystemusCore/data.h>

#include <QtSql/qsqlquery.h>
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
    bool get(const Data *primary, const QString &foreignProperty);
    bool get(const Data *primary, const QString &foreignProperty, const QString &indexProperty);
    bool get(const Data &primary);
    bool get(const Data &primary, const QString &foreignProperty);
    bool get(const Data &primary, const QString &foreignProperty, const QString &indexProperty);

    bool insert(Data *primary);
    bool insert(Data *primary, const QString &foreignProperty);
    bool insert(Data *primary, const QString &foreignProperty, const QString &indexProperty);
    bool insert(Data &primary);
    bool insert(Data &primary, const QString &foreignProperty);
    bool insert(Data &primary, const QString &foreignProperty, const QString &indexProperty);

    bool update(Data *primary);
    bool update(Data *primary, const QString &foreignProperty);
    bool update(Data *primary, const QString &foreignProperty, const QString &indexProperty);
    bool update(Data &primary);
    bool update(Data &primary, const QString &foreignProperty);
    bool update(Data &primary, const QString &foreignProperty, const QString &indexProperty);

    bool deleteData(Data *primary);
    bool deleteData(Data *primary, const QString &foreignProperty);
    bool deleteData(Data *primary, const QString &foreignProperty, const QString &indexProperty);
    bool deleteData(Data &primary);
    bool deleteData(Data &primary, const QString &foreignProperty);
    bool deleteData(Data &primary, const QString &foreignProperty, const QString &indexProperty);

    virtual DataInfo foreignInfo() const = 0;

    virtual int relationshipType() const = 0;

protected:
    virtual bool getAllData(const Data &primary, const QString &foreignProperty, const QString &indexProperty) = 0;
    virtual bool insertAllData(Data *primary, const QString &foreignProperty, const QString &indexProperty) = 0;
    virtual bool updateAllData(Data *primary, const QString &foreignProperty, const QString &indexProperty) = 0;
    virtual bool deleteAllData(Data *primary, const QString &foreignProperty, const QString &indexProperty) = 0;

    QString generateSelectStatement(const QString &table, const QSqlRecord &record, const QString &filter) const;
    QString generateInsertStatement(const QString &table, const QSqlRecord &record) const;
    QString generateUpdateStatement(const QString &table, const QSqlRecord &record, const QString &filter) const;
    QString generateDeleteStatement(const QString &table, const QString &filter) const;

    static QString formatedPropertyValue(const QString &property, const Data &data);
    static QString formatValue(const QVariant &value);

    QSqlQuery exec(const QString &query, bool *ok, const Data &primary) const;
    QSqlQuery exec(const QString &query, bool *ok) const;

    virtual void processReceivedRecord(const QSqlRecord &record) = 0;

    QString _name;
};

template<typename ForeignData>
class SYSTEMUS_CORE_EXPORT DataRelation : public AbstractDataRelation
{
public:
    DataInfo foreignInfo() const override
    { return DataInfo::fromType<ForeignData>(); }

    bool operator==(const DataRelation<ForeignData> &other) const
    { return _name == other._name; }
    inline bool operator!=(const DataRelation<ForeignData> &other) const
    { return !operator==(other); }

protected:
    virtual ForeignData dataFromRecord(const QSqlRecord &record)
    { return Data::fromSqlRecord<ForeignData>(record); }
    virtual void saveData(const ForeignData &data) = 0;

    virtual QString selectStatement(const Data &primary, const QString &foreignProperty, const QString &indexProperty) const
    { return QString(); }

    bool getAllData(const Data &primary, const QString &foreignProperty, const QString &indexProperty) override
    {
        const QString statement = selectStatement(primary, indexProperty, foreignProperty);

        bool ok;
        QSqlQuery query = exec(statement, &ok, primary);

        if (ok) {
            clear();
            while (query.next())
                processReceivedRecord(query.record());
            return true;
        } else
            return false;
    }

    void processReceivedRecord(const QSqlRecord &record) override
    { saveData(dataFromRecord(record)); }
};

template<typename ForeignData>
class SYSTEMUS_CORE_EXPORT SingleRelation : public DataRelation<ForeignData>
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

    ForeignData &operator=(const ForeignData &other)
    { return _data = other; }

    SingleRelation<ForeignData> &operator=(const SingleRelation<ForeignData> &other)
    { _data = other._data; DataRelation<ForeignData>::operator=(other); return *this; }

    bool isEmpty() const override
    { return _data.isEmpty(); }

    void clear() override
    { _data.clear(); }

    QJsonValue toJson() const override
    { return _data.toJsonObject(); }

protected:
    void saveData(const ForeignData &data) override
    { _data = data; }

    bool insertAllData(Data *primary, const QString &foreignProperty, const QString &indexProperty) override
    {
        if (!_data.save())
            return false;

        const DataInfo foreignInfo = this->foreignInfo();

        QString foreignPropertyName;
        if (!foreignProperty.isEmpty())
            foreignPropertyName = foreignProperty;
        else
            foreignPropertyName = foreignInfo.foreignPropertyName();

        QString indexPropertyName;
        if (!indexProperty.isEmpty())
            indexPropertyName = indexProperty;
        else
            indexPropertyName = foreignInfo.idPropertyName();

        primary->setProperty(foreignPropertyName, _data.property(indexPropertyName));
        return true;
    }

    bool updateAllData(Data *primary, const QString &foreignProperty, const QString &indexProperty) override
    {
        if (!_data.save())
            return false;

        QString foreignPropertyName;
        if (!foreignProperty.isEmpty())
            foreignPropertyName = foreignProperty;
        else
            foreignPropertyName = this->foreignInfo().foreignPropertyName();

        QString indexPropertyName;
        if (!indexProperty.isEmpty())
            indexPropertyName = indexProperty;
        else
            indexPropertyName = this->foreignInfo().idPropertyName();

        primary->setProperty(foreignPropertyName, _data.property(indexPropertyName));
        return true;
    }

    bool deleteAllData(Data *primary, const QString &foreignProperty, const QString &indexProperty) override
    {
        Q_UNUSED(indexProperty);

        if (!_data.deleteData())
            return false;

        const DataInfo foreignInfo = this->foreignInfo();

        QString foreignPropertyName;
        if (!foreignProperty.isEmpty())
            foreignPropertyName = foreignProperty;
        else
            foreignPropertyName = foreignInfo.foreignPropertyName();

        primary->setProperty(foreignPropertyName, QVariant(foreignInfo.foreignField().metaType()));
        return true;
    }

    QString selectStatement(const Data &primary, const QString &foreignProperty, const QString &indexProperty) const override
    {
        const DataInfo foreignInfo = this->foreignInfo();

        QString foreignPropertyName;
        if (!foreignProperty.isEmpty())
            foreignPropertyName = foreignProperty;
        else
            foreignPropertyName = foreignInfo.foreignPropertyName();

        QString indexFieldName;
        if (!indexProperty.isEmpty())
            indexFieldName = DataInfo::fieldNameFromPropertyName(indexProperty);
        else
            indexFieldName = foreignInfo.idPropertyName();

        const QString filter = QStringLiteral("%1 = %2").arg(indexFieldName).arg(this->formatedPropertyValue(foreignPropertyName, primary));
        return this->generateSelectStatement(foreignInfo.tableName(), foreignInfo.record(), filter);
    }

private:
    ForeignData _data;
};

template<typename ForeignData>
class SYSTEMUS_CORE_EXPORT MultiRelation : public DataRelation<ForeignData>, public QList<ForeignData>
{
public:
    bool operator==(const MultiRelation<ForeignData> &other) const
    { return DataRelation<ForeignData>::operator==(other) && QList<ForeignData>::operator==(other); }
    inline bool operator!=(const MultiRelation<ForeignData> &other) const
    { return !operator==(other); }

    bool isEmpty() const override
    { return QList<ForeignData>::isEmpty(); }

    using AbstractDataRelation::insert;

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
class SYSTEMUS_CORE_EXPORT OneToOneRelation : public SingleRelation<ForeignData>
{
public:
    ForeignData &operator=(const ForeignData &other)
    { return SingleRelation<ForeignData>::operator=(other); }

    OneToOneRelation<ForeignData> &operator=(const OneToOneRelation<ForeignData> &other)
    { SingleRelation<ForeignData>::operator=(other); return *this; }

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
    bool insertAllData(Data *primary, const QString &foreignProperty, const QString &indexProperty) override
    { return updateAllData(primary, foreignProperty, indexProperty); }

    bool updateAllData(Data *primary, const QString &foreignProperty, const QString &indexProperty) override
    {
        const DataInfo info = primary->dataInfo();

        QString idPropertyName;
        if (!foreignProperty.isEmpty())
            idPropertyName = foreignProperty;
        else
            idPropertyName = info.idPropertyName();

        QString indexPropertyName;
        if (!indexProperty.isEmpty())
            indexPropertyName = indexProperty;
        else
            indexPropertyName = info.foreignPropertyName();

        for (ForeignData &data : *this) {
            data.setProperty(indexPropertyName, primary->property(idPropertyName));
            if (!data.save())
                return false;
        }
        return true;
    }

    bool deleteAllData(Data *primary, const QString &foreignProperty, const QString &indexProperty) override
    {
        const DataInfo info = primary->dataInfo();

        QString idPropertyName;
        if (!foreignProperty.isEmpty())
            idPropertyName = foreignProperty;
        else
            idPropertyName = info.idPropertyName();

        QString indexPropertyName;
        if (!indexProperty.isEmpty())
            indexPropertyName = indexProperty;
        else
            indexPropertyName = info.foreignPropertyName();

        QString filter = QStringLiteral("%1 = %2").arg(indexPropertyName).arg(this->formatedPropertyValue(idPropertyName, *primary));
        QString statement = this->generateDeleteStatement(this->foreignInfo().tableName(), filter);

        bool ok;
        this->exec(statement, &ok);
        return ok;
    }

    QString selectStatement(const Data &primary, const QString &foreignProperty, const QString &indexProperty) const override
    {
        const DataInfo info = primary.dataInfo();

        QString indexPropertyName;
        if (!indexProperty.isEmpty())
            indexPropertyName = indexProperty;
        else
            indexPropertyName = info.idPropertyName();

        QString foreignFieldName;
        if (!foreignProperty.isEmpty())
            foreignFieldName = DataInfo::fieldNameFromPropertyName(foreignProperty);
        else
            foreignFieldName = info.foreignFieldName();

        const QString filter = QStringLiteral("%1 = %2").arg(foreignFieldName).arg(primary.property(indexPropertyName).toInt());

        const DataInfo foreignInfo = this->foreignInfo();
        QSqlRecord record = foreignInfo.record();
        {
            QSqlField field = record.field(0);
            field.setName(foreignInfo.tableName() + '.' + field.name());
            record.replace(0, field);
        }
        return this->generateSelectStatement(foreignInfo.tableName(), record, filter);
    }
};

// Many To One Relationship

template<typename ForeignData>
class SYSTEMUS_CORE_EXPORT ManyToOneRelation : public SingleRelation<ForeignData>
{
public:
    int relationshipType() const override
    { return DataRelation<ForeignData>::ManyToOneRelationship; }
};

// Many To Many Relationship

template<typename ForeignData>
class SYSTEMUS_CORE_EXPORT ManyToManyRelation : public MultiRelation<ForeignData>
{
public:
    ManyToManyRelation() = default;

    ManyToManyRelation(const QStringList &fields) :
        ManyToManyRelation(QString(), fields)
    {}

    ManyToManyRelation(const QString &table, const QStringList &properties) :
        _junctionTable(table)
    {
        _junctionProperties.append(QPair<QString, QMetaType>("id", QMetaType::fromName("int")));

        const QList<QSqlField> fields = DataInfo::fieldsFromString(properties.join(','), table, "ManyToManyRelation");
        for (const QSqlField &field : fields) {
            const QString propertyName = DataInfo::propertyNameFromFieldName(field.name());
            _junctionProperties.append(QPair<QString, QMetaType>(propertyName, field.metaType()));
        }

        QPair<QString, QMetaType> foreignProperty(DataInfo::fromType<ForeignData>().foreignPropertyName(), QMetaType::fromName("QString"));
        _junctionProperties.append(foreignProperty);
    }

    ManyToManyRelation(const ManyToManyRelation<ForeignData> &data) = default;

    int junctionId(int index) const
    { return _junctionData.at(index).value("id").toInt(); }

    void setJunctionId(int index, int id)
    { _junctionData[index].insert("id", id); }

    QVariant junctionData(const QString &property, int index) const
    { return _junctionData.at(index).value(property); }

    void setJunctionData(const QString &property, int index, const QVariant &value)
    { _junctionData[index].insert(property, value); }

    QVariantHash junctionData(int index) const
    { return _junctionData.at(index); }

    void setJunctionData(int index, const QVariantHash &data)
    { _junctionData.replace(index, data); }

    bool saveJunctionData(int index)
    {
        if (!_junctionData.at(index).contains("id"))
            return this->insertJunctionData(index);
        else
            return this->updateJunctionData(index);
    }

    int insertJunctionData(int index)
    {
        QString statement = this->generateInsertStatement(this->junctionTable(), this->junctionRecord(index));
        bool ok;
        QSqlQuery query = this->exec(statement, &ok);

        if (ok) {
            _junctionData[index].insert("id", query.lastInsertId());
            return true;
        } else
            return false;
    }

    bool updateJunctionData(int index)
    {
        const QString filter = "id = " + _junctionData.at(index).value("id").toInt();
        QString statement = this->generateUpdateStatement(this->junctionTable(), this->junctionRecord(index), filter);

        bool ok;
        this->exec(statement, &ok);
        return ok;
    }

    bool deleteJunctionData(int index)
    {
        bool ok = true;

        if (_junctionData.at(index).contains("id")) {
            const QString filter = QStringLiteral("id = %1").arg(_junctionData.at(index).value("id").toInt());
            this->exec(this->generateDeleteStatement(this->junctionTable(), filter), &ok);
        }

        if (ok) {
            this->removeAt(index);
            return true;
        } else
            return false;
    }

    QSqlRecord junctionRecord(int index) const
    {
        QSqlRecord record = junctionRecord();
        for (const QPair<QString, QMetaType> &property : _junctionProperties) {
            const QString &propertyName = property.first;
            record.setValue(DataInfo::fieldNameFromPropertyName(propertyName), _junctionData.at(index).value(propertyName));
        }
        return record;
    }

    void clear() override
    { _junctionData.clear(); MultiRelation<ForeignData>::clear(); }

    int relationshipType() const override
    { return DataRelation<ForeignData>::ManyToManyRelationship; }

protected:
    ForeignData dataFromRecord(const QSqlRecord &record) override
    {
        QVariantHash data;
        for (const QPair<QString, QMetaType> &property : _junctionProperties) {
            const QString fieldName = DataInfo::fieldNameFromPropertyName(property.first);
            data.insert(property.first, record.value(fieldName));
        }
        _junctionData.append(data);
        return MultiRelation<ForeignData>::dataFromRecord(record);
    }

    bool insertAllData(Data *primary, const QString &foreignProperty, const QString &indexProperty) override
    { return updateAllData(primary, foreignProperty, indexProperty); }

    bool updateAllData(Data *primary, const QString &foreignProperty, const QString &indexProperty) override
    {
        QString foreignPropertyName;
        if (!foreignProperty.isEmpty())
            foreignPropertyName = foreignPropertyName;
        else
            foreignPropertyName = primary->dataInfo().foreignPropertyName();

        QString indexPropertyName;
        if (!indexProperty.isEmpty())
            indexPropertyName = indexProperty;
        else
            indexPropertyName = this->foreignInfo().foreignPropertyName();

        QSqlRecord record = this->junctionRecord();

        for (int i(0); i < this->size(); ++i) {
            Data *data = &(*this)[i];
            if (!data->save() || this->saveJunctionData(i))
                return false;
        }
        return true;
    }

    bool deleteAllData(Data *primary, const QString &foreignProperty, const QString &indexProperty) override
    {
        Q_UNUSED(primary);
        Q_UNUSED(foreignProperty);
        Q_UNUSED(indexProperty);

        for (int i(0); i < this->size(); ++i)
            if (!this->deleteJunctionData(i))
                return false;
        return true;
    }

    QString selectStatement(const Data &primary, const QString &foreignProperty, const QString &indexProperty) const override
    {
        const DataInfo info = primary.dataInfo();
        const DataInfo foreignInfo = this->foreignInfo();

        QString indexPropertyName;
        QString primaryFieldName;
        if (!indexProperty.isEmpty()) {
            indexPropertyName = indexProperty;
            primaryFieldName = DataInfo::fieldNameFromPropertyName(indexProperty);
        } else {
            indexPropertyName = info.foreignPropertyName();
            primaryFieldName = info.foreignFieldName();
        }

        QString foreignPropertyName;
        QString foreignFieldName;
        if (!foreignProperty.isEmpty()) {
            foreignPropertyName = foreignProperty;
            foreignFieldName = DataInfo::fieldNameFromPropertyName(foreignProperty);
        } else {
            foreignPropertyName = foreignInfo.foreignPropertyName();
            foreignFieldName = foreignInfo.foreignFieldName();
        }

        QSqlRecord record = this->junctionRecord();
        {
            QSqlField idField;

            idField = record.field(0);
            idField.setName(this->junctionTable(primary) + ".id");
            record.replace(0, idField);

            QSqlRecord foreignRecord = foreignInfo.record();

            idField = foreignRecord.field(0);
            idField.setName(this->junctionTable(primary) + ".id");
            foreignRecord.replace(0, idField);

            for (int i(0); i < foreignRecord.count(); ++i)
                record.append(foreignRecord.field(i));
        }

        QString join = QStringLiteral("%1 ON %1.%2 = %3.%4").arg(foreignInfo.tableName(), foreignInfo.idFieldName(), this->junctionTable(primary), foreignFieldName);
        QString filter = QStringLiteral("%1 = %2").arg(primaryFieldName).arg(primary.id());
        QString statement = this->generateSelectStatement(_junctionTable, record, QString());
        return statement + (false ? " INNER JOIN " : " LEFT JOIN ") + join + " WHERE " + filter;
    }

private:
    QString junctionTable(const Data &primary = Data()) const
    {
        if (!_junctionTable.isEmpty())
            return _junctionTable;
        else
            return primary.dataInfo().tableName().chopped(1) + this->foreignInfo().tableName();
    }

    QSqlRecord junctionRecord() const
    {
        QSqlRecord record;
        for (const QPair<QString, QMetaType> &property : _junctionProperties)
            record.append(QSqlField(DataInfo::fieldNameFromPropertyName(property.first), property.second));
        return record;
    }

    QString _junctionTable;
    QList<QPair<QString, QMetaType>> _junctionProperties;
    QList<QVariantHash> _junctionData;
};

}

#endif // SYSTEMUS_DATARELATION_H
