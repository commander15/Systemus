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
#include <QtCore/qjsonarray.h>

namespace Systemus {

Data::Data() :
    d_ptr(new DataPrivate)
{
}

Data::Data(DataPrivate *data) :
    d_ptr(data)
{
}

Data::Data(const QSharedDataPointer<DataPrivate> &other) :
    d_ptr(other)
{
}

void Data::init()
{
}

QByteArray Data::className() const
{
    return staticMetaObject.className();
}

void Data::setDataInfo(const DataInfo &info)
{
    S_D(Data);
    dataInfo() = info;
}

Data::Data(const Data &other) :
    d_ptr(other.d_ptr)
{
}

Data::~Data()
{
}

Data &Data::operator=(const Data &other)
{
    if (this != &other)
        d_ptr = other.d_ptr;
    return *this;
}

int Data::id() const
{
    S_D(const Data);
    initOperation();
    return d->id;
}

void Data::setId(int id)
{
    S_D(Data);
    initOperation();
    d->id = id;
}

QVariant Data::property(const QString &name) const
{
    S_D(const Data);
    initOperation();

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
    initOperation();

    const QMetaProperty property = metaPropertyByName(name);
    if (property.isValid())
        property.writeOnGadget(this, value);
    else if (true)
        d->values.insert(name, value);
}

QMetaProperty Data::metaPropertyByName(const QString &name) const
{
    S_D(const Data);
    initOperation();

    const DataInfo info = dataInfo();

    for (int i(0); i < info.metaObject()->propertyCount(); ++i) {
        const QMetaProperty property = info.metaObject()->property(i);
        if (property.name() == name)
            return property;
    }

    return QMetaProperty();
}

DataInfo Data::dataInfo() const
{
    initOperation();
    return DataInfo::fromRegistry(className());
}

bool Data::isValid() const
{
    S_D(const Data);
    initOperation();
    return d->id > 0;
}

void Data::fill(const Data &other)
{
    S_D(Data);
    initOperation();

    for (int i(1); i < dataInfo().metaObject()->propertyCount(); ++i) {
        const QMetaProperty property = dataInfo().metaObject()->property(i);
        property.writeOnGadget(this, property.readOnGadget(&other));
    }
}

void Data::dumpInfos() const
{
    initOperation();

    QJsonDocument doc;
    doc.setObject(toJsonObject());
    qInfo().noquote() << doc.toJson(QJsonDocument::Indented);
}

bool Data::isEmpty() const
{
    S_D(const Data);
    return d->id == 0;
}

void Data::clear()
{
    S_D(Data);
    initOperation();
    d->clear();
}

bool Data::get(const QString &filter, bool withExtras)
{
    initOperation();

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
    initOperation();
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
    initOperation();

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
    initOperation();

    const QString request = updateStatement() + " WHERE id = " + QString::number(d->id);
    bool ok;
    exec(request, &ok);

    return ok;
}

bool Data::deleteData()
{
    S_D(Data);
    initOperation();

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
    initOperation();
    return d->lastError;
}

QSqlRecord Data::toSqlRecord() const
{
    S_D(const Data);
    initOperation();

    QSqlRecord record = dataInfo().record();
    fillRecord(&record);
    return record;
}

QJsonObject Data::toJsonObject() const
{
    S_D(const Data);
    initOperation();

    QJsonObject object;

    for (int i(0); i < dataInfo().fieldCount(); ++i)
        object.insert(dataInfo().fieldName(i), QJsonValue::fromVariant(property(dataInfo().propertyName(i))));

    return object;
}

bool Data::operator==(const Data &other) const
{
    initOperation();

    if (this == &other || d_ptr == other.d_ptr)
        return true;
    else
        return d_ptr->equalsTo(d_ptr);
}

void Data::fillRecord(QSqlRecord *record) const
{
    S_D(const Data);
    initOperation();

    for (int i(0); i < dataInfo().fieldCount(); ++i)
        if (record->contains(dataInfo().fieldName(i)))
            record->setValue(dataInfo().fieldName(i), property(dataInfo().propertyName(i)));
}

void Data::extractRecord(const QSqlRecord &record)
{
    S_D(Data);
    initOperation();

    const DataInfo info = dataInfo();
    for (int i(0); i < info.fieldCount(); ++i)
        if (record.contains(info.fieldName(i)))
            setProperty(info.propertyName(i), record.value(info.fieldName(i)));
}

QSqlQuery Data::exec(const QString &query, bool *ok) const
{
    S_D(const Data);
    initOperation();
    return execQuery(query, ok, &d->lastError);
}

QSqlQuery Data::execCachedQuery(const QString &query, bool *ok, QSqlError *error)
{
    return execQuery(query, ok, error, true);
}

QSqlQuery Data::execQuery(const QString &query, bool *ok, QSqlError *error, bool cached)
{
    QSqlQuery qu;
    qu.setForwardOnly(!cached);

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

void Data::initOperation() const
{
    S_D(const Data);

    if (!d->initialized)
        qWarning().noquote() << "Systemus::Data: operating on an uninitialized instance, undefined behaviors may occurs";
}

void Data::initOperation()
{
    S_D(Data);

    if (!d->initialized) {
        init();
        d->initialized = true;
    }
}

QString Data::selectStatement() const
{
    S_D(const Data);
    return selectStatement(dataInfo());
}

QString Data::selectStatement(const DataInfo &info)
{
    return driver()->sqlStatement(QSqlDriver::SelectStatement, info.tableName(), info.record(), false);
}

QString Data::insertStatement() const
{
    S_D(const Data);
    return driver()->sqlStatement(QSqlDriver::InsertStatement, dataInfo().tableName(), toSqlRecord(), false);
}

QString Data::updateStatement() const
{
    S_D(const Data);
    return driver()->sqlStatement(QSqlDriver::UpdateStatement, dataInfo().tableName(), toSqlRecord(), false);
}

QString Data::deleteStatement() const
{
    S_D(const Data);
    return driver()->sqlStatement(QSqlDriver::DeleteStatement, dataInfo().tableName(), QSqlRecord(), false);
}

QSqlDriver *Data::driver()
{
    return QSqlDatabase::database().driver();
}

DataSearch::DataSearch()  :
    _page(0), _itemsPerPage(100)
{
}

DataSearch::DataSearch(const QString &filter, int page, int itemsPerPage) :
    _page(page), _itemsPerPage(itemsPerPage)
{
    this->filter(filter);
}

void DataSearch::filter(const QString &filter)
{
    if (!filter.isEmpty()) _filters.append(filter);
}

void DataSearch::group(const QString field)
{
    if (!field.isEmpty()) _groups.append(field);
}

void DataSearch::sort(int field, Qt::SortOrder order)
{
    if (field >= 0) {
        _sortFields.append(QString::number(field + 1));
        _sortOrders.append(order);
    }
}

void DataSearch::sort(const QString &field, Qt::SortOrder order)
{
    if (!field.isEmpty()) {
        _sortFields.append(field);
        _sortOrders.append(order);
    }
}

void DataSearch::page(int page)
{
    _page = page;
}

void DataSearch::paginate(int itemsPerPage)
{
    _itemsPerPage = itemsPerPage;
}

bool DataSearch::hasWhereClause() const
{
    return !_filters.isEmpty();
}

QString DataSearch::whereClause() const
{
    return (hasWhereClause() ? "WHERE " + _filters.join(" AND ") : QString());
}

bool DataSearch::hasGroupByClause() const
{
    return !_groups.isEmpty();
}

QString DataSearch::groupByClause() const
{
    if (hasGroupByClause())
        return "GROUP BY " + _groups.join(", ");
    else
        return QString();
}

bool DataSearch::hasOrderByClause() const
{
    return !_sortFields.isEmpty();
}

QString DataSearch::orderByClause() const
{
    if (!hasOrderByClause())
        return QString();

    QStringList clauses;
    for (int i(0); i < _sortFields.size(); ++i) {
        const QString field = _sortFields.at(i);
        const QString order = (_sortOrders.at(i) == Qt::AscendingOrder ? "ASC" : "DESC");
        clauses.append(field + ' ' + order);
    }

    return QStringLiteral("ORDER BY ") + clauses.join(", ");
}

bool DataSearch::hasLimitOffsetClause() const
{
    return _page > 0 && _itemsPerPage > 0;
}

QString DataSearch::limitOffsetClause() const
{
    if (hasLimitOffsetClause())
        return QStringLiteral("LIMIT %1 OFFSET %2").arg(_itemsPerPage).arg((_page - 1) * _itemsPerPage);
    else
        return QString();
}

void DataSearch::clear()
{
    _filters.clear();
    _groups.clear();
    _sortFields.clear();
    _sortOrders.clear();
    _page = 0;
    _itemsPerPage = 100;
}

DataInfo::DataInfo(DataInfoPrivate *d) :
    d_ptr(d)
{
}

DataInfo::DataInfo() :
    d_ptr(new DataInfoPrivate)
{
}

DataInfo::DataInfo(const DataInfo &other) :
    d_ptr(other.d_ptr)
{
}

DataInfo::~DataInfo()
{
}

DataInfo &DataInfo::operator=(const DataInfo &other)
{
    if (this != &other)
        d_ptr = other.d_ptr;
    return *this;
}

QString DataInfo::idFieldName() const
{
    S_D(const DataInfo);
    return d->tableRecord.fieldName(d->idFieldIndex);
}

QSqlField DataInfo::idField() const
{
    S_D(const DataInfo);
    return d->tableRecord.field(d->idFieldIndex);
}

QString DataInfo::userFieldName() const
{
    S_D(const DataInfo);
    return d->tableRecord.fieldName(d->userFieldIndex);
}

QSqlField DataInfo::userField() const
{
    S_D(const DataInfo);
    return d->tableRecord.field(d->userFieldIndex);
}

QStringList DataInfo::searchFieldNames() const
{
    S_D(const DataInfo);

    QStringList fields;

    for (int field : d->searchFieldIndexes)
        fields.append(d->tableRecord.fieldName(field));
    fields.append(QString(d->classInfo("search").value()).remove(' ').split(',', Qt::SkipEmptyParts));

    return fields;
}

QString DataInfo::fieldName(int index) const
{
    S_D(const DataInfo);
    return d->tableRecord.fieldName(index);
}

QSqlField DataInfo::field(int index) const
{
    S_D(const DataInfo);
    return d->tableRecord.field(index);
}

int DataInfo::fieldCount() const
{
    S_D(const DataInfo);
    return d->tableRecord.count();
}

QSqlRecord DataInfo::record() const
{
    S_D(const DataInfo);
    return d->tableRecord;
}

QString DataInfo::tableName() const
{
    S_D(const DataInfo);
    return d->tableName;
}

QString DataInfo::foreignFieldName() const
{
    S_D(const DataInfo);
    return d->tableName.toLower().chopped(1) + "_id";
}

QSqlField DataInfo::foreignField() const
{
    S_D(const DataInfo);

    QSqlField field = d->tableRecord.field(d->idFieldIndex);
    field.setName(foreignFieldName());
    return field;
}

QString DataInfo::foreignPropertyName() const
{
    QString name;

    bool uppercaseNext = false;
    for (const QChar &c : foreignFieldName()) {
        if (c == '_')
            uppercaseNext = true;
        else {
            if (!uppercaseNext)
                name.append(c);
            else {
                name.append(c.toUpper());
                uppercaseNext = false;
            }
        }
    }

    return name;
}

QString DataInfo::propertyName(int index) const
{
    S_D(const DataInfo);
    if (index < d->metaFields.size())
        return d->metaFields.at(index).name();
    else
        return d->tableRecord.fieldName(index);
}

QMetaProperty DataInfo::property(int index) const
{
    S_D(const DataInfo);
    if (index < d->metaFields.size())
        return (!d->metaFields.isEmpty() ? d->metaFields.at(index) : QMetaProperty());
    else
        return QMetaProperty();
}

const QMetaObject *DataInfo::metaObject() const
{
    S_D(const DataInfo);
    return d->metaObject;
}

bool DataInfo::operator==(const DataInfo &other) const
{
    S_D(const DataInfo);
    return d->equals(*other.d_ptr);
}

DataInfo DataInfo::fromRegistry(const QByteArray &className)
{
    return _registry.value(className);
}

DataInfo DataInfo::fromMetaObject(const QMetaObject &object)
{
    DataInfoPrivate *d = new DataInfoPrivate;
    d->update(&object);
    return DataInfo(d);
}

DataInfo DataInfo::fromTable(const QString &tableName)
{
    return fromTableRecord(tableName, QSqlDatabase::database().record(tableName));
}

DataInfo DataInfo::fromTableRecord(const QString &tableName, const QSqlRecord &record)
{
    DataInfoPrivate *d = new DataInfoPrivate;

    for (int i(0); i < record.count(); ++i) {
        if (i == 0)
            d->idFieldIndex = 0;
        else if (i == 1)
            d->userFieldIndex = 1;

        QSqlField field = record.field(i);
        if (field.metaType() == QMetaType::fromType<QString>())
            d->searchFieldIndexes.append(i);
    }

    d->tableName = tableName;
    d->tableRecord = record;

    return DataInfo(d);
}

QHash<QByteArray, DataInfo> DataInfo::_registry;

DataPrivate::DataPrivate() :
    id(0),
    initialized(false)
{
    _s_register_internal_types();
}

bool DataPrivate::equalsTo(const DataPrivate *other) const
{
    return id == other->id
        && values == other->values;
}

DataInfoPrivate::DataInfoPrivate()
{
    update(&Data::staticMetaObject);
}

DataInfoPrivate::~DataInfoPrivate()
{
}

QMetaProperty DataInfoPrivate::property(const QString &name) const
{
    return metaObject->property(metaObject->indexOfProperty(name.toStdString().c_str()));
}

QMetaClassInfo DataInfoPrivate::classInfo(const QString &name) const
{
    return metaObject->classInfo(metaObject->indexOfClassInfo(name.toStdString().c_str()));
}

void DataInfoPrivate::update(const QMetaObject *object)
{
    // Cleaning existing data
    idFieldIndex = 0;
    userFieldIndex = 1;
    searchFieldIndexes.clear();

    tableName.clear();
    tableRecord.clear();

    metaFields.clear();
    metaObject = object;

    // Extracting table name

    int tableIndex = object->indexOfClassInfo("table");
    if (tableIndex >= 0)
        tableName = QString(object->classInfo(tableIndex).value());
    else
        tableName = tableNameFromClassName(object->className());

    // Extracting fields

    for (int i(0); i < object->propertyCount(); ++i) {
        const QMetaProperty property = object->property(i);

        if (property.isStored()) {
            QSqlField field(fieldNameFromPropertyName(property.name()), property.metaType(), tableName);
            field.setRequired(property.isRequired());
            tableRecord.append(field);

            if (tableRecord.count() == 1)
                idFieldIndex = tableRecord.count() - 1;

            if (property.isUser())
                userFieldIndex = tableRecord.count() - 1;

            if (property.metaType() == QMetaType::fromType<QString>())
                searchFieldIndexes.append(tableRecord.count() - 1);

            metaFields.append(property);
        }
    }

    int extraFieldsIndex = object->indexOfClassInfo("fields");
    if (extraFieldsIndex >= 0) {
        const QList<QSqlField> fields = fieldsFromString(QString(object->classInfo(extraFieldsIndex).value()), tableName, object->className());
        for (const QSqlField &field : fields)
            tableRecord.append(field);
    }
}

bool DataInfoPrivate::equals(const DataInfoPrivate &other) const
{
    return idFieldIndex == other.idFieldIndex
        && userFieldIndex == other.userFieldIndex
        && searchFieldIndexes == other.searchFieldIndexes
        && tableName == other.tableName
        && tableRecord == other.tableRecord
        && metaObject == other.metaObject;
}

QString DataInfoPrivate::tableNameFromClassName(const QString &className)
{
    return className.split("::").last() + 's';
}

QString DataInfoPrivate::fieldNameFromPropertyName(const QString &propertyName)
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

QSqlField DataInfoPrivate::fieldFromProperty(const QMetaProperty &property, const QMetaObject *object)
{
    QSqlField field(fieldNameFromPropertyName(property.name()), property.metaType(), tableNameFromClassName(object->className()));
    field.setRequired(property.isRequired());
    //field.setReadOnly(!property.isWritable());
    return field;
}

QList<QSqlField> DataInfoPrivate::fieldsFromString(const QString &str, const QString &tableName, const QString &context)
{
    QStringList extraFields;
    {
        QString field;
        for (const QChar &c : str) {
            if (c != ' ' && c != ',')
                field.append(c);

            if (c == ')') {
                extraFields.append(field);
                field.clear();
            }
        }
    }

    QList<QSqlField> fields;

    QRegularExpression regExp("^(?<name>[a-zA-Z_]+)\\((?<type>[A-Za-z_]+),?(?<options>[a-z, ]*)\\)$");

    for (const QString &extraField : extraFields) {
        QRegularExpressionMatch match = regExp.match(extraField);

        if (match.hasMatch()) {
            QSqlField field(match.captured("name"), QMetaType::fromName(match.captured("type").toLatin1()), tableName);

            if (match.hasCaptured("options")) {
                const QStringList options = match.captured("options").remove(' ').split(',', Qt::SkipEmptyParts);
                for (const QString &option : options) {
                    if (option == "required")
                        field.setRequired(true);
                }
            }

            fields.append(field);
        } else
            qWarning().noquote() << Q_FUNC_INFO << ": unable to process field " << extraField << ", check the syntax on " << context;
    }

    return fields;
}

}
