#ifndef SYSTEMUS_ABSTRACTDATA_H
#define SYSTEMUS_ABSTRACTDATA_H

#include <SystemusCore/global.h>
#include <SystemusCore/metatable.h>

#include <QtCore/qvariant.h>

class QJsonObject;

class QSqlQuery;
class QSqlError;
class QSqlRecord;

namespace Systemus {
namespace Orm {
class MetaTable;

class SYSTEMUS_CORE_EXPORT AbstractData
{
public:
    AbstractData();
    virtual ~AbstractData();

    void init();

#if defined(SYSTEMUS_PRIMARY_GETTER) && defined(SYSTEMUS_PRIMARY_SETTER) && defined(SYSTEMUS_PRIMARY_TYPE)
    inline bool SYSTEMUS_PRIMARY_CHECKER() const;
    template<typename T = SYSTEMUS_PRIMARY_TYPE> inline T SYSTEMUS_PRIMARY_GETTER() const;
    template<typename T = SYSTEMUS_PRIMARY_TYPE> inline void SYSTEMUS_PRIMARY_SETTER(const T &id);
#endif

    bool hasPrimaryValue() const;
    QVariant primaryValue() const;
    void setPrimaryValue(const QVariant &value);

    bool hasUserProperty() const;
    QVariant userProperty() const;
    void setUserProperty(const QVariant &value);

    bool hasProperty(const QString &name) const;
    QVariant property(const QString &name) const;
    void setProperty(const QString &name, const QVariant &value);

    virtual bool isValid() const;
    virtual bool isEmpty() const;

    inline void fill(const AbstractData &data) { fillWithData(data); }
    inline void fill(const QJsonObject &object) { fillWithJsonObject(object); }
    inline void fill(const QVariantMap &map) { fillWithMap(map); }
    inline void fill(const QVariantHash &hash) { fillWithHash(hash); }
    inline void fill(const QSqlRecord &record) { fillWithSqlRecord(record); }

    bool identicalTo(const AbstractData &other) const;
    virtual bool refersTo(const AbstractData &other) const;
    virtual bool sameAs(const AbstractData &other) const;

    virtual bool get() = 0;

    virtual bool save();
    virtual bool update() = 0;
    virtual bool insert() = 0;

    virtual bool deleteData() = 0;

    virtual int lastErrorCode() const;
    virtual QList<int> lastErrorCodes() const;
    virtual QString lastErrorString() const;
    virtual int lastErrorType() const;
    virtual QSqlError lastError() const = 0;

    virtual void clear();

    virtual QString className() const = 0;
    QString tableName() const;
    MetaTable metaTable() const;

    virtual QJsonObject toJsonObject() const;
    virtual QSqlRecord toSqlRecord() const;

protected:
    virtual void initData() = 0;
    virtual void registerData() = 0;

    virtual bool isPropertyNull(const QString &name) const = 0;
    virtual QVariant readProperty(const QString &name) const = 0;
    virtual bool writeProperty(const QString &name, const QVariant &value) = 0;

    virtual void fillWithData(const AbstractData &other);
    virtual void fillWithJsonObject(const QJsonObject &object);
    virtual void fillWithMap(const QVariantMap &data);
    virtual void fillWithHash(const QVariantHash &data);
    virtual void fillWithSqlRecord(const QSqlRecord &record);

    QSqlQuery exec(const QString &query, bool *ok = nullptr, QSqlError *error = nullptr);
    virtual void processError(const QSqlError &error) = 0;

    friend class AbstractDataRelation;
};

#if defined(SYSTEMUS_PRIMARY_GETTER) && defined(SYSTEMUS_PRIMARY_SETTER) && defined(SYSTEMUS_PRIMARY_TYPE)
inline bool AbstractData::SYSTEMUS_PRIMARY_CHECKER() const
{ return hasPrimaryValue(); }

template<typename T>
inline T AbstractData::SYSTEMUS_PRIMARY_GETTER() const
{ return primaryValue().value<T>(); }

template<typename T>
inline void AbstractData::SYSTEMUS_PRIMARY_SETTER(const T &id)
{ setPrimaryValue(id); }
#endif

}
}

#endif // SYSTEMUS_ABSTRACTDATA_H
