#include "metatable.h"
#include "metatable_p.h"

#include <SystemusCore/ormbackend.h>

#include <QtSql/qsqlfield.h>

namespace Systemus {
namespace Orm {

MetaTable::MetaTable() :
    d_ptr(new MetaTablePrivate)
{
}

MetaTable::MetaTable(const MetaTable &other) :
    d_ptr(other.d_ptr)
{
}

MetaTable::~MetaTable()
{
}

MetaTable &MetaTable::operator=(const MetaTable &other)
{
    if (this != &other)
        d_ptr = other.d_ptr;
    return *this;
}

QString MetaTable::className() const
{
    S_D(const MetaTable);
    return d->metaObject->className();
}

QString MetaTable::tableName() const
{
    S_D(const MetaTable);
    return d->tableName;
}

QString MetaTable::primaryPropertyName() const
{
    S_D(const MetaTable);
    return d->metaObject->property(d->primaryPropertyIndex).name();
}

QMetaProperty MetaTable::primaryProperty() const
{
    S_D(const MetaTable);
    return d->metaObject->property(d->primaryPropertyIndex);
}

QString MetaTable::primaryFieldName() const
{
    S_D(const MetaTable);
    return d->record.fieldName(d->primaryFieldIndex);
}

QSqlField MetaTable::primaryField() const
{
    S_D(const MetaTable);
    return d->record.field(d->primaryFieldIndex);
}

QString MetaTable::userPropertyName() const
{
    S_D(const MetaTable);
    return d->metaObject->userProperty().name();
}

QMetaProperty MetaTable::userProperty() const
{
    S_D(const MetaTable);
    return d->metaObject->userProperty();
}

QString MetaTable::userFieldName() const
{
    S_D(const MetaTable);
    return d->record.fieldName(d->userFieldIndex);
}

QSqlField MetaTable::userField() const
{
    S_D(const MetaTable);
    return d->record.field(d->userFieldIndex);
}

QStringList MetaTable::metaPropertyNames() const
{
    S_D(const MetaTable);

    QStringList names;
    for (int i : d->metaPropertyIndexes)
        names.append(d->metaObject->property(i).name());
    return names;
}

QString MetaTable::metaPropertyName(int index) const
{
    S_D(const MetaTable);
    return d->metaObject->property(d->metaPropertyIndexes.at(index)).name();
}

QMetaProperty MetaTable::metaProperty(int index) const
{
    S_D(const MetaTable);
    return d->metaObject->property(d->metaPropertyIndexes.at(index));
}

QStringList MetaTable::metaFieldNames() const
{
    S_D(const MetaTable);

    QStringList names;
    for (int i : d->metaFieldIndexes)
        names.append(d->record.fieldName(i));
    return names;
}

QString MetaTable::metaFieldName(int index) const
{
    S_D(const MetaTable);
    return d->record.fieldName(d->metaFieldIndexes.at(index));
}

QSqlField MetaTable::metaField(int index) const
{
    S_D(const MetaTable);
    return d->record.field(d->metaFieldIndexes.at(index));
}

int MetaTable::metaCount() const
{
    S_D(const MetaTable);
    return d->metaPropertyIndexes.size();
}

QSqlRecord MetaTable::metaRecord() const
{
    S_D(const MetaTable);

    QSqlRecord record;
    for (int index : d->metaFieldIndexes)
        record.append(d->record.field(index));
    return record;
}

QStringList MetaTable::secretPropertyNames() const
{
    S_D(const MetaTable);

    QStringList names;
    for (int i : d->secretFieldIndexes)
        names.append(Backend::instance()->propertyNameFromFieldName(d->record.fieldName(i)));
    return names;
}

QString MetaTable::secretPropertyName(int index) const
{
    S_D(const MetaTable);
    return Backend::instance()->propertyNameFromFieldName(d->record.fieldName(d->secretFieldIndexes.at(index)));
}

QMetaProperty MetaTable::secretProperty(int index) const
{
    Q_UNUSED(index);
    return QMetaProperty();
}

QStringList MetaTable::secretFieldNames() const
{
    S_D(const MetaTable);

    QStringList names;
    for (int i : d->secretFieldIndexes)
        names.append(d->record.fieldName(i));
    return names;
}

QString MetaTable::secretFieldName(int index) const
{
    S_D(const MetaTable);
    return d->record.fieldName(d->secretFieldIndexes.at(index));
}

QSqlField MetaTable::secretField(int index) const
{
    S_D(const MetaTable);
    return d->record.field(d->secretFieldIndexes.at(index));
}

int MetaTable::secretCount() const
{
    S_D(const MetaTable);
    return d->secretFieldIndexes.size();
}

QSqlRecord MetaTable::secretRecord() const
{
    S_D(const MetaTable);

    QSqlRecord record;
    for (int index : d->secretFieldIndexes)
        record.append(d->record.field(index));
    return record;
}

QStringList MetaTable::propertyNames() const
{
    S_D(const MetaTable);

    QStringList names;
    for (int i(0); i < d->record.count(); ++i)
        names.append(propertyName(i));
    return names;
}

QString MetaTable::propertyName(int index) const
{
    S_D(const MetaTable);
    if (index < d->metaPropertyIndexes.size())
        return d->metaObject->property(d->metaPropertyIndexes.at(index)).name();
    else
        return Backend::instance()->propertyNameFromFieldName(d->record.fieldName(index));
}

QMetaProperty MetaTable::property(int index) const
{
    S_D(const MetaTable);
    if (index < d->metaPropertyIndexes.size())
        return d->metaObject->property(d->metaPropertyIndexes.at(index));
    else
        return QMetaProperty();
}

QStringList MetaTable::fieldNames() const
{
    S_D(const MetaTable);

    QStringList names;
    for (int i(0); i < d->record.count(); ++i)
        names.append(d->record.fieldName(i));
    return names;
}

QString MetaTable::fieldName(int index) const
{
    S_D(const MetaTable);
    return d->record.field(index).name();
}

QSqlField MetaTable::field(int index) const
{
    S_D(const MetaTable);
    return d->record.field(index);
}

int MetaTable::count() const
{
    S_D(const MetaTable);
    return d->record.count();
}

QSqlRecord MetaTable::record() const
{
    S_D(const MetaTable);
    return d->record;
}

MetaTable MetaTable::fromName(const QString &className)
{

}

}
}
