#ifndef SYSTEMUS_DATA_H
#define SYSTEMUS_DATA_H

#include <SystemusCore/global.h>
#include <SystemusCore/abstractdata.h>
#include <SystemusCore/ormdata.h>

namespace Systemus {

class SYSTEMUS_CORE_EXPORT DataContent : public QSharedData
{
public:
    Orm::Data *ormData = nullptr;
};

class SYSTEMUS_CORE_EXPORT Data : public Orm::AbstractData
{
public:
    Data();
    Data(Orm::Data *data);
    Data(const Orm::Data &other);
    Data(const Data &other);
    ~Data();

    Data &operator=(const Orm::Data &other);
    Data &operator=(const Data &other);

    bool isValid() const override;
    bool isEmpty() const override;

    bool refersTo(const AbstractData &other) const override;
    bool sameAs(const AbstractData &other) const override;

    bool get() override;
    bool getByPrimary(const QVariant &value, bool full = true);
    bool getByFilter(const QString &filter, bool full = true);
    bool preGet();
    bool postGet();

    bool update() override;
    bool insert() override;
    bool deleteData() override;

    void clear() override;

    int lastErrorCode() const override;
    QList<int> lastErrorCodes() const override;
    QString lastErrorString() const override;
    int lastErrorType() const override;
    QSqlError lastError() const override;

    QString className() const override;

    QJsonObject toJsonObject() const override;
    QSqlRecord toSqlRecord() const override;
    template<typename T> T to() const;
    template<typename T> T *toPointer() const;

    QSqlRecord modelRecord() const;
    void setModelRecord(const QSqlRecord &record);

protected:
    void initData() override;
    void registerData() override;

    bool isPropertyNull(const QString &name) const override;
    QVariant readProperty(const QString &name) const override;
    bool writeProperty(const QString &name, const QVariant &value) override;

    void fillWithData(const AbstractData &other) override;
    void fillWithJsonObject(const QJsonObject &object) override;
    void fillWithSqlRecord(const QSqlRecord &record) override;

    void processError(const QSqlError &error) override;

    Orm::Data *ormData() const;

private:
    QSharedDataPointer<DataContent> d_ptr;


    friend class QSharedDataPointer<DataContent>;
};

template<typename T>
inline T Data::to() const
{
    Orm::Data *ormData = this->ormData();
    if (ormData) {
        if (ormData->metaObject() == &T::staticMetaObject)
            return T(*static_cast<T *>(ormData));
        else {
            T data;
            data.fill(*ormData);
            return data;
        }
    }

    return T();
}

template<typename T>
inline T *Data::toPointer() const
{
    Orm::Data *ormData = this->ormData();
    return (ormData ? static_cast<T *>(ormData) : nullptr);
}

}

template<>
inline Systemus::DataContent *QSharedDataPointer<Systemus::DataContent>::clone();

#endif // SYSTEMUS_DATA_H
