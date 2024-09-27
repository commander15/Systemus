#include "data.h"
#include "data_p.h"
#include "debug_p.h"

#include <QtSql/qsqlrecord.h>
#include <QtSql/qsqlerror.h>

#include <QtCore/qjsonobject.h>

namespace Systemus {

Data::Data() :
    d_ptr(new DataPrivate())
{
}

Data::Data(Orm::Data *data) :
    d_ptr(new DataPrivate(data))
{
}

Data::Data(const Orm::Data &other) :
    d_ptr(new DataPrivate(other))
{
}

Data::Data(const Data &other) :
    d_ptr(other.d_ptr)
{
}

Data::~Data()
{
}

Data &Data::operator=(const Orm::Data &other)
{
    S_D(Data);
    d->changeData(other);
    return *this;
}

Data &Data::operator=(const Data &other)
{
    if (this != &other && d_ptr != other.d_ptr)
        d_ptr = other.d_ptr;
    return *this;
}

bool Data::isValid() const
{
    S_D(const Data);
    return (d->ormData ? d->ormData->isValid() : false);
}

bool Data::isEmpty() const
{
    S_D(const Data);
    return (d->ormData ? d->ormData->isEmpty() : false);
}

void Data::fill(const AbstractData &other)
{
    S_D(Data);
    if (d->ormData)
        d->ormData->fill(other);
}

void Data::fill(const QJsonObject &object)
{
    S_D(Data);
    if (d->ormData)
        d->ormData->fill(object);
}

void Data::fill(const QSqlRecord &record)
{
    S_D(Data);
    if (d->ormData)
        d->ormData->fill(record);
}

bool Data::refersTo(const AbstractData &other) const
{
    S_D(const Data);
    return (d->ormData ? d->ormData->refersTo(other) : false);
}

bool Data::sameAs(const AbstractData &other) const
{
    S_D(const Data);
    return (d->ormData ? d->ormData->sameAs(other) : false);
}

bool Data::get()
{
    S_D(Data);
    return (d->ormData ? d->ormData->get() : false);
}

bool Data::getByPrimary(const QVariant &value, bool full)
{
    S_D(Data);
    return (d->ormData ? d->ormData->getByPrimary(value, full) : false);
}

bool Data::getByFilter(const QString &filter, bool full)
{
    S_D(Data);
    return (d->ormData ? d->ormData->getByFilter(filter, full) : false);
}

bool Data::preGet()
{
    S_D(Data);
    return (d->ormData ? d->ormData->preGet() : false);
}

bool Data::postGet()
{
    S_D(Data);
    return (d->ormData ? d->ormData->postGet() : false);
}

bool Data::update()
{
    S_D(Data);
    return (d->ormData ? d->ormData->update() : false);
}

bool Data::insert()
{
    S_D(Data);
    return (d->ormData ? d->ormData->insert() : false);
}

bool Data::deleteData()
{
    S_D(Data);
    return (d->ormData ? d->ormData->deleteData() : false);
}

void Data::clear()
{
    S_D(Data);
    if (d->ormData)
        d->ormData->clear();
}

int Data::lastErrorCode() const
{
    S_D(const Data);
    return (d->ormData ? d->ormData->lastErrorCode() : 0);
}

QList<int> Data::lastErrorCodes() const
{
    S_D(const Data);
    return (d->ormData ? d->ormData->lastErrorCodes() : QList<int>());
}

QString Data::lastErrorString() const
{
    S_D(const Data);
    return (d->ormData ? d->ormData->lastErrorString() : QString());
}

int Data::lastErrorType() const
{
    S_D(const Data);
    return (d->ormData ? d->ormData->lastErrorType() : 0);
}

QSqlError Data::lastError() const
{
    S_D(const Data);
    return (d->ormData ? d->ormData->lastError() : QSqlError());
}

QString Data::className() const
{
    S_D(const Data);
    return (d->ormData ? d->ormData->className() : QString());
}

QJsonObject Data::toJsonObject() const
{
    S_D(const Data);
    return (d->ormData ? d->ormData->toJsonObject() : QJsonObject());
}

QSqlRecord Data::toSqlRecord() const
{
    S_D(const Data);
    return (d->ormData ? d->ormData->toSqlRecord() : QSqlRecord());
}

QSqlRecord Data::modelRecord() const
{
    S_D(const Data);
    return d->modelRecord;
}

void Data::setModelRecord(const QSqlRecord &record)
{
    S_D(Data);
    d->modelRecord = record;
}

void Data::initData()
{
    S_D(Data);
    if (d->ormData)
        d->ormData->initData();
}

void Data::registerData()
{
    S_D(Data);
    if (d->ormData)
        d->ormData->registerData();
}

bool Data::isPropertyNull(const QString &name) const
{
    S_D(const Data);
    return (d->ormData ? d->ormData->isPropertyNull(name) : true);
}

QVariant Data::readProperty(const QString &name) const
{
    S_D(const Data);
    if (d->properties.contains(name))
        return d->properties.value(name);
    else
        return (d->ormData ? d->ormData->readProperty(name) : QVariant());
}

bool Data::writeProperty(const QString &name, const QVariant &value)
{
    S_D(Data);

    if (d->ormData) {
        const bool ok = d->ormData->writeProperty(name, value);
        if (!ok)
            d->properties.insert(name, value);
        return ok;
    } else {
        d->properties.insert(name, value);
        return false;
    }
}

void Data::processError(const QSqlError &error)
{
    S_D(Data);
    if (d->ormData)
        d->ormData->processError(error);
}

Orm::Data *Data::ormData() const
{
    S_D(const Data);
    return d->ormData;
}

DataPrivate::DataPrivate(const Orm::Data &data)
{
    ormData = copy(&data);
}

DataPrivate::DataPrivate(Orm::Data *data)
{
    ormData = data;
}

DataPrivate::DataPrivate(const DataPrivate &other) :
    DataContent(other),
    properties(other.properties)
{
    ormData = copy(other.ormData);
}

DataPrivate::~DataPrivate()
{
    if (ormData)
        delete ormData;
}

void DataPrivate::changeData(const Orm::Data &other)
{
    if (ormData)
        delete ormData;

    ormData = copy(&other);
}

Orm::Data *DataPrivate::copy(const Orm::Data *other)
{
    if (!other)
        return nullptr;

    const QMetaType metaType = other->metaType();
    if (metaType.isValid()) {
        void *rawCopy = metaType.create(other);
        if (rawCopy) {
            return static_cast<Orm::Data *>(rawCopy);
        } else {
            systemusWarning() << "Systemus::Data: can't create a copy of \"" << other->className() << '"';
        }
    } else {
        systemusWarning() << "Systemus::Data: meta type not found for \"" << other->className() << '"';
    }

    return nullptr;
}

}

template<>
Systemus::DataContent *QSharedDataPointer<Systemus::DataContent>::clone()
{
    return new Systemus::DataPrivate(*static_cast<const Systemus::DataPrivate *>(d));
}
