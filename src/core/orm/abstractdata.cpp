#include "abstractdata.h"

#include <SystemusCore/orm.h>
#include <SystemusCore/ormbackend.h>
#include <SystemusCore/metamapper.h>

#include <QtCore/qjsonobject.h>

#include <QtSql/qsqlquery.h>
#include <QtSql/qsqlrecord.h>
#include <QtSql/qsqlfield.h>
#include <QtSql/qsqlerror.h>
#include <QtSql/qsqldriver.h>

namespace Systemus {
namespace Orm {

AbstractData::AbstractData()
{
}

AbstractData::~AbstractData()
{
}

void AbstractData::init()
{
    initData();
    registerData();
}

bool AbstractData::hasPrimaryValue() const
{
    return !isPropertyNull(metaTable().primaryPropertyName());
}

QVariant AbstractData::primaryValue() const
{
    return readProperty(metaTable().primaryPropertyName());

}

void AbstractData::setPrimaryValue(const QVariant &value)
{
    writeProperty(metaTable().primaryPropertyName(), value);
}

bool AbstractData::hasUserProperty() const
{
    return !isPropertyNull(metaTable().userPropertyName());
}

QVariant AbstractData::userProperty() const
{
    return readProperty(metaTable().userPropertyName());
}

void AbstractData::setUserProperty(const QVariant &value)
{
    writeProperty(metaTable().userPropertyName(), value);
}

bool AbstractData::hasProperty(const QString &name) const
{
    return !isPropertyNull(name);
}

QVariant AbstractData::property(const QString &name) const
{
    return readProperty(name);
}

void AbstractData::setProperty(const QString &name, const QVariant &value)
{
    writeProperty(name, value);
}

bool AbstractData::isValid() const
{
    const MetaTable table = metaTable();

    for (int i(0); i < table.count(); ++i) {
        const QSqlField field = table.field(i);
        if (field.requiredStatus() == QSqlField::Required && isPropertyNull(table.propertyName(i)))
            return false;
    }

    return false;
}

bool AbstractData::isEmpty() const
{
    const QStringList properties = metaTable().propertyNames();
    for (const QString &property : properties)
        if (!isPropertyNull(property))
            return false;
    return true;
}

bool AbstractData::identicalTo(const AbstractData &other) const
{
    return refersTo(other) && sameAs(other);
}

bool AbstractData::refersTo(const AbstractData &other) const
{
    return primaryValue() == other.primaryValue();
}

bool AbstractData::sameAs(const AbstractData &other) const
{
    const MetaTable table = metaTable();
    for (int i(0); i < table.count(); ++i) {
        const QString property = table.propertyName(i);
        if (readProperty(property) != other.readProperty(property))
            return false;
    }

    return true;
}

bool AbstractData::save()
{
    return (hasPrimaryValue() ? update() : insert());
}

int AbstractData::lastErrorCode() const
{
    return lastError().nativeErrorCode().section(';', 0, 0).toInt();
}

QList<int> AbstractData::lastErrorCodes() const
{
    QList<int> codes;
    const QStringList codeStrings = lastError().nativeErrorCode().split(';');
    for (const QString &codeStr : codeStrings) {
        bool ok = true;
        const int code = codeStr.toInt(&ok);
        if (ok)
            codes.append(code);
    }
    return codes;
}

QString AbstractData::lastErrorString() const
{
    return lastError().text();
}

int AbstractData::lastErrorType() const
{
    return lastError().type();
}

void AbstractData::clear()
{
    const QStringList properties = metaTable().propertyNames();
    for (const QString &property : properties)
        writeProperty(property, QVariant());
}

QString AbstractData::tableName() const
{
    return metaTable().tableName();
}

MetaTable AbstractData::metaTable() const
{
    const QString className = this->className();
    if (MetaTable::isRegistered(className))
        return MetaTable::fromClassName(className);
    else
        return MetaTable();
}

QJsonObject AbstractData::toJsonObject() const
{
    const MetaTable table = metaTable();

    QJsonObject object;
    for (int i(0); i < table.count(); ++i) {
        const QString property = table.propertyName(i);
        object.insert(property, QJsonValue::fromVariant(readProperty(property)));
    }
    return object;
}

QSqlRecord AbstractData::toSqlRecord() const
{
    const MetaTable table = metaTable();

    QSqlRecord record = table.record();
    for (int i(0); i < table.count(); ++i)
        record.setValue(i, readProperty(table.propertyName(i)));
    return record;
}

void AbstractData::fillWithData(const AbstractData &other)
{
    const MetaTable table = metaTable();
    const QStringList properties = table.propertyNames();
    for (const QString &property : properties)
        if (other.hasProperty(property))
            writeProperty(property, other.readProperty(property));
}

void AbstractData::fillWithJsonObject(const QJsonObject &object)
{
    const QStringList properties = object.keys();
    for (const QString &property : properties)
        if (object.contains(property))
            writeProperty(property, object.value(property));
}

void AbstractData::fillWithMap(const QVariantMap &data)
{
    const MetaTable table = metaTable();
    for (int i(0); i < table.count(); ++i) {
        const QString propertyName = table.propertyName(i);
        if (data.contains(propertyName))
            writeProperty(propertyName, data.value(propertyName));
    }
}

void AbstractData::fillWithHash(const QVariantHash &data)
{
    const MetaTable table = metaTable();
    for (int i(0); i < table.count(); ++i) {
        const QString propertyName = table.propertyName(i);
        if (data.contains(propertyName))
            writeProperty(propertyName, data.value(propertyName));
    }
}

void AbstractData::fillWithSqlRecord(const QSqlRecord &record)
{
    const MetaTable table = metaTable();
    for (int i(0); i < table.count(); ++i) {
        const QString propertyName = table.propertyName(i);
        const QString fieldName = MetaMapper::fieldName(propertyName, table);

        if (record.contains(fieldName))
            writeProperty(propertyName, record.value(fieldName));
    }
}

QSqlQuery AbstractData::exec(const QString &query, bool *ok, QSqlError *error)
{
    QSqlQuery sqlQuery;

    if (error) {
        sqlQuery = Systemus::exec(query, ok, error);
        processError(*error);
    } else {
        QSqlError error;
        sqlQuery = Systemus::exec(query, ok, &error);
        processError(error);
    }

    return sqlQuery;
}

}
}
