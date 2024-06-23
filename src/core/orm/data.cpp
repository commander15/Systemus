#include "data.h"
#include "data_p.h"

#include <SystemusCore/private/debug_p.h>

#include <QtSql/qsqldatabase.h>
#include <QtSql/qsqlquery.h>
#include <QtSql/qsqlrecord.h>
#include <QtSql/qsqlfield.h>
#include <QtSql/qsqldriver.h>

#include <QtCore/qmetaobject.h>
#include <QtCore/qjsondocument.h>

namespace Systemus {

Data::Data() :
    d_ptr(new DataPrivate),
    metaObject(&staticMetaObject)
{
}

Data::Data(DataPrivate *data) :
    d_ptr(data),
    metaObject(&staticMetaObject)
{
}

Data::Data(const QSharedDataPointer<DataPrivate> &other) :
    d_ptr(other),
    metaObject(&staticMetaObject)
{
}

Data::Data(const Data &other) :
    d_ptr(other.d_ptr),
    metaObject(&staticMetaObject)
{
}

Data::~Data()
{
}

Data &Data::operator=(const Data &other)
{
    if (metaObject == &Data::staticMetaObject)
        qWarning() << Q_FUNC_INFO << "called with base class, forgoten init<Super>() call in constructor ?";
    else if (this != &other && metaObject == other.metaObject)
        d_ptr = other.d_ptr;

    return *this;
}

int Data::id() const
{
    S_D(const Data);
    return d->id;
}

void Data::setId(int id)
{
    S_D(Data);
    d->id = id;
}

QVariant Data::property(const QString &name) const
{
    S_D(const Data);

    const QMetaProperty property = metaPropertyByName(name);
    if (property.isValid())
        return property.readOnGadget(this);
    else if (d->values.contains(name))
        return d->values.value(name);
    else
        return QVariant();
}

void Data::setProperty(const QString &name, const QVariant &value)
{
    S_D(Data);

    const QMetaProperty property = metaPropertyByName(name);
    if (property.isValid())
        property.writeOnGadget(this, value);
    else if (true)
        d->values.insert(name, value);
}

QMetaProperty Data::metaPropertyByName(const QString &name) const
{
    for (int i(0); i < metaObject->propertyCount(); ++i) {
        const QMetaProperty property = metaObject->property(i);
        if ((property.name()) == name)
            return property;
    }

    return QMetaProperty();
}

bool Data::isValid() const
{
    S_D(const Data);
    return d->id > 0;
}

void Data::fill(const Data &other)
{
    for (int i(1); i < metaObject->propertyCount(); ++i) {
        const QMetaProperty property = metaObject->property(i);
        if (!property.isStored())
            continue;

        property.writeOnGadget(this, property.readOnGadget(&other));
    }
}

void Data::dumpInfos() const
{
    QJsonDocument doc;
    doc.setObject(toJsonObject());
    qInfo().noquote() << doc.toJson(QJsonDocument::Indented);
}

void Data::clear()
{
    S_D(Data);
    d->clear();
}

bool Data::get(const QString &filter, bool withExtras)
{
    const QString request = selectStatement() + " WHERE " + filter + " LIMIT 1";
    bool ok;
    QSqlQuery query = exec(request, &ok);

    if (ok && query.next()) {
        extractRecord(query.record());
        query.finish();
        return (withExtras ? getExtras() : true);
    } else
        return false;
}

bool Data::getExtras()
{
    return true;
}

bool Data::save()
{
    S_D(Data);
    
    if (d->id == 0)
        return insert();
    else
        return update();
}

bool Data::insert()
{
    S_D(Data);

    const QString request = insertStatement();
    bool ok;
    QSqlQuery query = exec(request, &ok);

    if (ok) {
        d->id = query.lastInsertId().toInt();
        return true;
    } else
        return false;
}

bool Data::update()
{
    S_D(Data);

    const QString request = updateStatement() + " WHERE id = " + QString::number(d->id);
    bool ok;
    exec(request, &ok);

    return ok;
}

bool Data::deleteData()
{
    S_D(Data);

    const QString request = deleteStatement() + " WHERE id = " + QString::number(d->id);   
    bool ok;
    exec(request, &ok);

    if (ok) {
        d->id = 0;
        return true;
    } else
        return false;
}

QSqlError Data::lastError() const
{
    S_D(const Data);
    return d->lastError;
}

QString Data::tableName(const QMetaObject &metaObject)
{
    int index = metaObject.indexOfClassInfo("table");

    if (index >= 0)
        return metaObject.classInfo(index).value();
    else
        return DataPrivate::tableNameFromClassName(metaObject.className());
}

QSqlRecord Data::tableRecord(const QMetaObject &metaObject, bool includeTable, bool excludeReadOnly)
{
    QSqlRecord record;

    for (int i(0); i < metaObject.propertyCount(); ++i) {
        const QMetaProperty property = metaObject.property(i);
        if (excludeReadOnly && !property.isWritable())
            continue;
        else if (DataPrivate::isStandardFieldProperty(property, metaObject))
            record.append(DataPrivate::standardFieldFromProperty(property, metaObject, includeTable));
        else if (DataPrivate::isForeignProperty(property, metaObject))
            record.append(DataPrivate::foreignFieldFromProperty(property, metaObject, includeTable));
    }

    {
        int fieldsId = metaObject.indexOfClassInfo("fields");
        QStringList fields = QString(metaObject.classInfo(fieldsId).value()).split(", ", Qt::SkipEmptyParts);

        for (const QString &field : fields)
            record.append(QSqlField(field, QMetaType(QMetaType::Int)));
    }

    return record;
}

QList<QSqlRelation> Data::tableRelations(const QMetaObject &metaObject)
{
    QList<QSqlRelation> relations;

    for (int i(0); i < metaObject.propertyCount(); ++i) {
        const QMetaProperty property = metaObject.property(i);
        if (DataPrivate::isRelationProperty(property, metaObject))
            relations.append(DataPrivate::relationFromProperty(property, metaObject));
    }

    return relations;
}

QString Data::displayFieldName(const QMetaObject &object)
{
    for (int i(0); i < object.propertyCount(); ++i) {
        const QMetaProperty property = object.property(i);
        if (!property.isReadable() || !property.isStored())
            continue;

        if (property.isUser())
            return property.name();
    }

    return QString();
}

QString Data::foreignFieldName(const QMetaObject &metaObject)
{
    int index = metaObject.indexOfClassInfo("foreign_field");

    if (index >= 0)
        return metaObject.classInfo(index).value();
    else
        return tableName(metaObject).toLower().removeLast() + "_id";
}

QSqlRecord Data::toSqlRecord(bool includeTable) const
{
    QSqlRecord record = tableRecord(*metaObject, includeTable);
    fillRecord(&record);
    return record;
}

QJsonObject Data::toJsonObject() const
{
    QJsonObject object;

    const QSqlRecord record = toSqlRecord(false);
    for (int i(0); i < record.count(); ++i)
        object.insert(record.fieldName(i), QJsonValue::fromVariant(record.value(i)));

    return object;
}

bool Data::operator==(const Data &other) const
{
    if (this == &other || d_ptr == other.d_ptr)
        return true;
    else
        return d_ptr->equalsTo(d_ptr);
}

void Data::fillRecord(QSqlRecord *record) const
{
    for (int i(0); i < metaObject->propertyCount(); ++i) {
        const QMetaProperty property = metaObject->property(i);
        if (!property.isReadable() || !property.isStored())
            continue;

        const QString fieldName = DataPrivate::fieldNameFromPropertyName(property.name());
        if (record->contains(fieldName))
            record->setValue(fieldName, property.readOnGadget(this));
    }

    int id = metaObject->indexOfClassInfo("fields");
    const QStringList fields = QString(metaObject->classInfo(id).value()).split(", ", Qt::SkipEmptyParts);
    for (const QString &field : fields)
        record->setValue(field, property(field));
}

void Data::extractRecord(const QSqlRecord &record)
{
    for (int i(0); i < metaObject->propertyCount(); ++i) {
        const QMetaProperty property = metaObject->property(i);
        if (!property.isReadable() || !property.isStored())
            continue;

        const QString fieldName = DataPrivate::fieldNameFromPropertyName(property.name());
        if (record.contains(fieldName))
            property.writeOnGadget(this, record.value(fieldName));
    }

    int id = metaObject->indexOfClassInfo("fields");
    const QStringList fields = QString(metaObject->classInfo(id).value()).split(", ", Qt::SkipEmptyParts);
    for (const QString &field : fields)
        setProperty(field, record.value(field));
}

QSqlQuery Data::exec(const QString &query, bool *ok) const
{
    S_D(const Data);
    return execQuery(query, ok, &d->lastError);
}

QSqlQuery Data::execQuery(const QString &query, bool *ok, QSqlError *error)
{
    QSqlQuery qu;
    qu.setForwardOnly(true);

    if (ok)
        *ok = qu.exec(query);
    else
        qu.exec(query);

    systemusSqlInfo() << query;
    if (qu.lastError().isValid()) {
        if (error)
            *error = qu.lastError();

        systemusSqlWarning() << qu.lastError().databaseText();
    }

    return qu;
}

QString Data::selectStatement(const QMetaObject &object)
{
    return driver()->sqlStatement(QSqlDriver::SelectStatement, tableName(object), tableRecord(object), false);
}

QString Data::insertStatement() const
{
    QSqlRecord record = toSqlRecord();
    record.remove(0);
    return driver()->sqlStatement(QSqlDriver::InsertStatement, tableName(), record, false);
}

QString Data::updateStatement() const
{
    QSqlRecord record = tableRecord(true);
    record.remove(0);
    fillRecord(&record);
    return driver()->sqlStatement(QSqlDriver::UpdateStatement, tableName(), record, false);
}

QString Data::deleteStatement() const
{
    return driver()->sqlStatement(QSqlDriver::DeleteStatement, tableName(), QSqlRecord(), false);
}

QSqlDriver *Data::driver()
{
    return QSqlDatabase::database().driver();
}

void Data::bindQueryValues(QSqlQuery &query) const
{
    for (int i(0); i < metaObject->propertyCount(); ++i) {
        const QMetaProperty property = metaObject->property(i);
        if (!property.isReadable() || !property.isStored())
            continue;

        query.bindValue(':' + QString(property.name()), property.readOnGadget(this));
    }
}

AuthorizationData::AuthorizationData() :
    Data(new AuthorizationDataPrivate)
{
    init<AuthorizationData>();
}

AuthorizationData::AuthorizationData(AuthorizationDataPrivate *data) :
    Data(data)
{
    init<AuthorizationData>();
}

AuthorizationData::AuthorizationData(const AuthorizationData &other) :
    Data(other)
{
    init<AuthorizationData>();
}

AuthorizationData::~AuthorizationData()
{
}

QString AuthorizationData::name() const
{
    S_D(const AuthorizationData);
    return d->name;
}

void AuthorizationData::setName(const QString &name)
{
    S_D(AuthorizationData);
    d->name = name;
}

QString AuthorizationData::description() const
{
    S_D(const AuthorizationData);
    return d->description;
}

void AuthorizationData::setDescription(const QString &description)
{
    S_D(AuthorizationData);
    d->description = description;
}

QDate AuthorizationData::creationDate() const
{
    S_D(const AuthorizationData);
    return d->creationDate;
}

QTime AuthorizationData::creationTime() const
{
    S_D(const AuthorizationData);
    return d->creationTime;
}

bool AuthorizationData::isValid() const
{
    S_D(const AuthorizationData);
    return !d->name.isEmpty() && Data::isValid();
}

IssuedAuthorizationData::IssuedAuthorizationData() :
    Data(new IssuedAuthorizationDataPrivate)
{
    init<IssuedAuthorizationData>();
}

IssuedAuthorizationData::IssuedAuthorizationData(IssuedAuthorizationDataPrivate *data) :
    Data(data)
{
    init<IssuedAuthorizationData>();
}

IssuedAuthorizationData::IssuedAuthorizationData(const IssuedAuthorizationData &other) :
    Data(other)
{
    init<IssuedAuthorizationData>();
}

IssuedAuthorizationData::~IssuedAuthorizationData()
{
}

QString IssuedAuthorizationData::name() const
{
    S_D(const IssuedAuthorizationData);
    return d->refData.name();
}

QString IssuedAuthorizationData::description() const
{
    S_D(const IssuedAuthorizationData);
    return d->refData.description();
}

bool IssuedAuthorizationData::isActive() const
{
    S_D(const IssuedAuthorizationData);
    return d->active;
}

void IssuedAuthorizationData::setActive(bool active)
{
    S_D(IssuedAuthorizationData);
    d->active = active;
}

QDate IssuedAuthorizationData::issueDate() const
{
    S_D(const IssuedAuthorizationData);
    return d->issueDate;
}

QTime IssuedAuthorizationData::issueTime() const
{
    S_D(const IssuedAuthorizationData);
    return d->issueTime;
}

QString DataPrivate::tableNameFromClassName(const QString &className)
{
    return className.split("::").constLast() + 's';
}

QString DataPrivate::fieldNameFromPropertyName(const QString &propertyName)
{
    QString name;

    for (const QChar &c : propertyName) {
        if (c.isLower())
            name.append(c);
        else
            name.append('_' + c.toLower());
    }

    return name;
}

bool DataPrivate::isStandardFieldProperty(const QMetaProperty &property, const QMetaObject &metaObject)
{
    return property.isReadable()
           && property.isStored()
           && !isForeignProperty(property, metaObject)
           && !isRelationProperty(property, metaObject);
}

QSqlField DataPrivate::standardFieldFromProperty(const QMetaProperty &property, const QMetaObject &metaObject, bool includeTable)
{
    QSqlField field;
    field.setName(fieldNameFromPropertyName(property.name()));
    field.setMetaType(property.metaType());
    field.setRequired(property.isRequired());

    if (includeTable)
        field.setTableName(Data::tableName(metaObject));

    return field;
}

bool DataPrivate::isForeignProperty(const QMetaProperty &property, const QMetaObject &metaObject)
{
    Q_UNUSED(metaObject);

    const QMetaObject *object = property.metaType().metaObject();
    return (object ? object->inherits(&Data::staticMetaObject) : false);
}

QSqlField DataPrivate::foreignFieldFromProperty(const QMetaProperty &property, const QMetaObject &metaObject, bool includeTable)
{
    QSqlField field;
    field.setName(fieldNameFromPropertyName(property.name()));
    field.setMetaType(QMetaType(QMetaType::Int));
    field.setRequired(property.isRequired());

    if (includeTable)
        field.setTableName(Data::tableName(metaObject));

    return field;
}

bool DataPrivate::isRelationProperty(const QMetaProperty &property, const QMetaObject &metaObject)
{
    return (QString(property.typeName()).startsWith("QList<"));
}

QSqlField DataPrivate::relationFieldFromProperty(const QMetaProperty &property, const QMetaObject &metaObject, bool includeTable)
{
    QSqlField field;
    field.setName(fieldNameFromPropertyName(property.name()));
    field.setRequired(property.isRequired());

    {
        QString type(property.typeName());
        type.remove("QList<");
        type.remove('>');
        field.setMetaType(QMetaType::fromName(type.toLatin1()));
    }

    if (includeTable)
        field.setTableName(Data::tableName(metaObject));

    return field;
}

QSqlRelation DataPrivate::relationFromProperty(const QMetaProperty &property, const QMetaObject &metaObject)
{
    const QMetaObject *object = nullptr;
    {
        QString name = property.typeName();
        if (name.startsWith("QList<")) {
            name.remove(0, 6);
            name.chop(1);
            const QMetaType type = QMetaType::fromName(name.toLatin1());
            object = type.metaObject();
        }
    }

    if (object) {
        QString table = Data::tableName(*object);
        QString index = Data::foreignFieldName(*object);
        QString display = Data::displayFieldName(*object);
        return QSqlRelation(table, index, display);
    } else
        return QSqlRelation();
}

QSqlField DataPrivate::fieldFromProperty(const QMetaProperty &property, const QMetaObject &object, bool includeTable)
{
    QSqlField field;
    field.setName(fieldNameFromPropertyName(property.name()));
    field.setMetaType(property.metaType());
    field.setRequired(property.isRequired());

    if (includeTable)
        field.setTableName(Data::tableName(object));

    return field;
}

QList<QMetaProperty> DataPrivate::fieldProperties(const QMetaObject &metaObject, FieldType type)
{
    QList<QMetaProperty> properties;

    for (int i(0); i < metaObject.propertyCount(); ++i) {
        const QMetaProperty property = metaObject.property(i);

        if (!property.isReadable() || !property.isStored())
            continue;

        const QMetaObject *object = property.metaType().metaObject();
        if (!object && type == StandardField)
            properties.append(property);
        else if (object && type == ForeignField)
            properties.append(property);
    }

    return properties;
}

}
