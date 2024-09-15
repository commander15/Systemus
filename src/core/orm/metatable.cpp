#include "metatable.h"
#include "metatable_p.h"

#include <SystemusCore/ormbackend.h>
#include <SystemusCore/private/debug_p.h>

#include <QtSql/qsqlfield.h>

namespace Systemus {
namespace Orm {

MetaTable::MetaTable() :
    d_ptr(new MetaTablePrivate)
{
}

MetaTable::MetaTable(MetaTableData *d) :
    d_ptr(d)
{
}

MetaTable::MetaTable(const MetaTable &other) :
    d_ptr(other.d_ptr)
{
}

MetaTable::MetaTable(MetaTable &&other) :
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
    return (d->metaObject ? d->metaObject->className() : QString());
}

QString MetaTable::tableName() const
{
    S_D(const MetaTable);
    return d->tableName;
}

QString MetaTable::primaryPropertyName() const
{
    S_D(const MetaTable);
    if (d->metaObject)
        return d->metaObject->property(d->primaryPropertyIndex).name();
    else
        return Backend::instance()->propertyNameFromFieldName(d->record.fieldName(d->primaryFieldIndex), className());
}

QMetaProperty MetaTable::primaryProperty() const
{
    S_D(const MetaTable);
    return (d->metaObject ? d->metaObject->property(d->primaryPropertyIndex) : QMetaProperty());
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
    return (d->metaObject ? d->metaObject->userProperty().name() : QString());
}

QMetaProperty MetaTable::userProperty() const
{
    S_D(const MetaTable);
    return (d->metaObject ? d->metaObject->userProperty() : QMetaProperty());
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

    if (!d->metaObject)
        return QStringList();

    QStringList names;
    for (int i : d->metaPropertyIndexes)
        names.append(d->metaObject->property(i).name());
    return names;
}

QString MetaTable::metaPropertyName(int index) const
{
    S_D(const MetaTable);
    return (d->metaObject ? d->metaObject->property(d->metaPropertyIndexes.at(index)).name() : QString());
}

QMetaProperty MetaTable::metaProperty(int index) const
{
    S_D(const MetaTable);
    return (d->metaObject ? d->metaObject->property(d->metaPropertyIndexes.at(index)) : QMetaProperty());
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
    for (int i : d->secretPropertyIndexes)
        names.append(d->secretProperties.at(i).name());
    return names;
}

QString MetaTable::secretPropertyName(int index) const
{
    S_D(const MetaTable);
    if (index < d->secretPropertyIndexes.size())
        return d->secretProperties.at(d->secretPropertyIndexes.at(index)).name();
    else
        return QString();
}

SecretProperty MetaTable::secretProperty(int index) const
{
    S_D(const MetaTable);
    if (index < d->secretPropertyIndexes.size())
        return d->secretProperties.at(d->secretPropertyIndexes.at(index));
    else
        return SecretProperty();
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
    return d->secretPropertyIndexes.size();
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
        return (d->metaObject ? d->metaObject->property(d->metaPropertyIndexes.at(index)).name() : QString());
    else
        return Backend::instance()->propertyNameFromFieldName(d->record.fieldName(index), className());
}

SecretProperty MetaTable::property(int index) const
{
    S_D(const MetaTable);
    if (index < d->metaPropertyIndexes.size())
        return (d->metaObject ? d->metaObject->property(d->metaPropertyIndexes.at(index)) : QMetaProperty());
    else if (index - d->metaPropertyIndexes.size() < d->secretProperties.size())
        return d->secretProperties.at(index - d->metaPropertyIndexes.size());
    else
        return SecretProperty();
}

int MetaTable::indexOfProperty(const QString &propertyName) const
{
    S_D(const MetaTable);

    int index = d->metaObject->indexOfProperty(propertyName.toStdString().c_str());
    if (index >= 0 && d->metaPropertyIndexes.contains(index))
        return d->metaPropertyIndexes.indexOf(index);

    for (int i(0); i < d->secretProperties.size(); ++i)
        if (d->secretProperties.at(i).name() == propertyName)
            return i + d->metaPropertyIndexes.size();

    return -1;
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

int MetaTable::indexOfField(const QString &fieldName) const
{
    S_D(const MetaTable);
    return d->record.indexOf(fieldName);
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

QString MetaTable::foreignPropertyName() const
{
    S_D(const MetaTable);
    return d->foreignPropertyName;
}

QString MetaTable::foreignFieldName() const
{
    S_D(const MetaTable);
    return d->foreignFieldName;
}

QSqlField MetaTable::foreignField() const
{
    S_D(const MetaTable);
    QSqlField field = primaryField();
    field.setName(d->foreignFieldName);
    return field;
}

void *MetaTable::newData(const void *copy) const
{
    S_D(const MetaTable);

    const QMetaType type(d->metaTypeId);
    return (type.isValid() ? type.create(copy) : nullptr);
}

void MetaTable::assignData(const void *from, void *to) const
{
    S_D(const MetaTable);
    if (d->assignmentFunction)
        d->assignmentFunction(from, to);
}

const QMetaObject *MetaTable::metaObject() const
{
    S_D(const MetaTable);
    return d->metaObject;
}

QMetaType MetaTable::metaType() const
{
    S_D(const MetaTable);
    return QMetaType(d->metaTypeId);
}

bool MetaTable::isValid() const
{
    S_D(const MetaTable);
    return d->metaObject && d->metaTypeId > 0;
}

bool MetaTable::isRegistered(const QString &name)
{
    return s_tables.contains(name);
}

MetaTable MetaTable::fromClassName(const QString &name)
{
    if (!s_tables.contains(name)) {
        systemusWarning() << "Orm::MetaTable: can't find meta table for \"" << name << "\" class";
        return MetaTable();
    } else {
        return s_tables.value(name);
    }
}

void MetaTable::registerClass(const QString &name, const QMetaObject *metaObject, const QMetaType &metaType)
{
    if (s_tables.contains(name) || !metaObject || !metaType.isValid())
        return;

    const Backend *backend = Backend::instance();

    MetaTablePrivate *data = new MetaTablePrivate;
    data->tableName = backend->tableNameFromMetaObject(metaObject);

    data->record = backend->recordFromMetaObject(metaObject, &data->metaPropertyFieldIndexes, &data->secretPropertyFieldIndexes);

    data->metaPropertyIndexes = data->metaPropertyFieldIndexes.keys();
    data->metaFieldIndexes.reserve(data->metaPropertyIndexes.size());
    std::transform(data->metaPropertyIndexes.begin(), data->metaPropertyIndexes.end(), data->metaFieldIndexes.begin(), [data](int propertyIndex) {
        return data->metaPropertyFieldIndexes.value(propertyIndex);
    });

    data->secretPropertyIndexes = data->secretPropertyFieldIndexes.keys();
    data->secretFieldIndexes.reserve(data->secretPropertyIndexes.size());
    std::transform(data->secretPropertyIndexes.begin(), data->secretPropertyIndexes.end(), data->secretFieldIndexes.begin(), [data](int propertyIndex) {
        return data->secretPropertyIndexes.value(propertyIndex);
    });

    data->secretProperties = backend->secretPropertiesFromMetaObject(metaObject);

    backend->primaryFieldFromMetaObject(metaObject, &data->primaryPropertyIndex);
    data->primaryFieldIndex = data->metaPropertyFieldIndexes.value(data->primaryPropertyIndex);

    backend->userFieldFromMetaObject(metaObject);
    data->userFieldIndex = data->metaPropertyFieldIndexes.value(metaObject->userProperty().propertyIndex());

    data->metaObject = metaObject;
    data->metaTypeId = metaType.id();

    data->foreignPropertyName = backend->foreignPropertyNameFromMetaObject(metaObject);
    data->foreignFieldName = backend->foreignFieldNameFromPropertyName(metaObject->property(data->primaryPropertyIndex).name(), metaObject->className());

    s_tables.insert(name, MetaTable(data));
}

void MetaTable::registerClassAssignmentFunction(const QString &className, const std::function<void (const void *, void *)> &function)
{
    if (s_tables.contains(className))
        static_cast<MetaTablePrivate *>(s_tables[className].d_ptr.get())->assignmentFunction = function;
}

QHash<QString, MetaTable> MetaTable::s_tables;

}
}
