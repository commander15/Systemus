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
#include <QtCore/qjsonobject.h>
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

Data::Data(const QSharedDataPointer<DataPrivate> &data) :
    d_ptr(data)
{
}

Data::Data(const Data &other) :
    Data(other, true)
{
}

Data::Data(const Data &other, bool transferProperties)
{
    if (transferProperties) {
        DataPrivate *d = new DataPrivate(*other.d_ptr.get());

        const DataInfo info = other.dataInfo();
        for (int i(0); i < info.displayCount(); ++i) {
            const QString property = info.displayPropertyName(i);
            const QVariant value = other.property(property);
            d->properties.insert(property, value);
        }

        d_ptr.reset(d);

        auto transferFunction = DataInfoPrivate::transferFunctions.value(d_ptr->className, nullptr);
        if (transferFunction) {
            transferFunction(&other, this, 0);
        }
    }
}

Data::~Data()
{
}

Data &Data::operator=(const Data &other)
{
    if (this != &other) {
        d_ptr = other.d_ptr;
    }
    return *this;
}

void Data::init()
{
    S_D(Data);

    if (!d->initialized) {
        initData();
        d->initialized = true;
    }
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
    if (property.isValid() && property.isReadable())
        return property.readOnGadget(this);
    else
        return d->property(name);
}

void Data::setProperty(const QString &name, const QVariant &value)
{
    S_D(Data);

    const QMetaProperty property = metaPropertyByName(name);
    if (property.isValid() && property.isWritable())
        property.writeOnGadget(this, value);
    else
        d->setProperty(name, value);
}

QMetaProperty Data::metaPropertyByName(const QString &name) const
{
    const QMetaObject *metaObject = instanceMetaObject();
    int propertyIndex = metaObject->indexOfProperty(name.toStdString().c_str());
    return (propertyIndex >= 0 ? metaObject->property(propertyIndex) : QMetaProperty());
}

void Data::fill(const Data &other)
{
    S_D(Data);

    const DataInfo info = dataInfo();
    for (int i(1); i < info.count(); ++i) {
        const QString property = info.fieldPropertyName(i);
        setProperty(property, other.property(property));
    }
}

void Data::fill(const QSqlRecord &record)
{
    S_D(Data);

    const DataInfo info = dataInfo();
    for (int i(1); i < info.count(); ++i)
        if (record.contains(info.fieldName(i)))
            setProperty(info.fieldPropertyName(i), record.value(info.fieldName(i)));
}

DataInfo Data::dataInfo() const
{
    S_D(const Data);
    return DataInfo::fromName(d->className);
}

bool Data::isValid() const
{
    S_D(const Data);
    return d->id > 0;
}

void Data::dumpInfos() const
{
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

QSqlRecord Data::toSqlRecord() const
{
    S_D(const Data);

    QSqlRecord record = dataInfo().record();
    fillRecord(&record);
    return record;
}

QJsonObject Data::toJsonObject() const
{
    S_D(const Data);

    QJsonObject object;

    object.insert("id", d->id);

    const DataInfo info = dataInfo();
    for (int i(0); i < info.displayCount(); ++i)
        object.insert(info.displayFieldName(i), QJsonValue::fromVariant(property(info.displayPropertyName(i))));

    std::function<JsonGenerationFunction> function = DataInfoPrivate::jsonGenerationFunctions.value(d->className);
    if (function)
        function(*this, &object);
    else {
        const QStringList properties = d->properties.keys();
        for (const QString &property : properties)
            object.insert(property, QJsonValue::fromVariant(d->properties.value(property)));
    }

    return object;
}

const void *Data::internalData() const
{
    return d_ptr.get();
}

void *Data::internalData()
{
    return d_ptr.get();
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

bool Data::beginTransaction()
{
    return QSqlDatabase::database().transaction();
}

bool Data::commitTransaction()
{
    return QSqlDatabase::database().commit();
}

bool Data::rollbackTransaction()
{
    return QSqlDatabase::database().rollback();
}

QSqlDriver *Data::driver()
{
    return QSqlDatabase::database().driver();
}

bool Data::operator==(const Data &other) const
{

    if (this == &other || d_ptr == other.d_ptr)
        return true;
    else
        return d_ptr->equalsTo(d_ptr);
}

void Data::initData()
{
}

QByteArray Data::instanceClassName() const
{
    return instanceMetaObject()->className();
}

const QMetaObject *Data::instanceMetaObject() const
{
    return &staticMetaObject;
}

QByteArray Data::dataClassName() const
{
    S_D(const Data);
    return d->className;
}

void Data::setDataClassName(const QByteArray &name)
{
    S_D(Data);
    d->className = name;
}

void Data::fillRecord(QSqlRecord *record) const
{
    S_D(const Data);

    const DataInfo info = dataInfo();
    for (int i(0); i < info.count(); ++i)
        if (record->contains(info.fieldName(i)))
            record->setValue(info.fieldName(i), property(info.fieldPropertyName(i)));
}

void Data::extractRecord(const QSqlRecord &record)
{
    S_D(Data);
    init();

    const DataInfo info = dataInfo();
    for (int i(0); i < info.count(); ++i)
        if (record.contains(info.fieldName(i)))
            setProperty(info.fieldPropertyName(i), record.value(info.fieldName(i)));
}

QSqlQuery Data::exec(const QString &query, bool *ok) const
{
    S_D(const Data);
    return execQuery(query, ok, &d->lastError);
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

QString DataInfo::tableName() const
{
    S_D(const DataInfo);
    return d->tableName;
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

QString DataInfo::idPropertyName() const
{
    S_D(const DataInfo);
    return fieldPropertyName(d->idFieldIndex);
}

QMetaProperty DataInfo::idProperty() const
{
    S_D(const DataInfo);
    return fieldProperty(d->idFieldIndex);
}

QString DataInfo::userFieldName() const
{
    S_D(const DataInfo);
    return fieldName(d->userFieldIndex);
}

QSqlField DataInfo::userField() const
{
    S_D(const DataInfo);
    return field(d->userFieldIndex);
}

QString DataInfo::userPropertyName() const
{
    S_D(const DataInfo);
    return fieldPropertyName(d->userFieldIndex);
}

QMetaProperty DataInfo::userProperty() const
{
    S_D(const DataInfo);
    return fieldProperty(d->userFieldIndex);
}

QStringList DataInfo::displayFieldNames() const
{
    S_D(const DataInfo);

    QStringList names;
    for (int index : d->displayFieldIndexes)
        names.append(fieldName(index));
    return names;
}

QString DataInfo::displayFieldName(int index) const
{
    S_D(const DataInfo);
    return fieldName(d->displayFieldIndexes.value(index));
}

QSqlField DataInfo::displayField(int index) const
{
    S_D(const DataInfo);
    return field(d->displayFieldIndexes.value(index));
}

QStringList DataInfo::displayPropertyNames() const
{
    S_D(const DataInfo);

    QStringList names;
    for (int index : d->displayFieldIndexes)
        names.append(fieldPropertyName(index));
    return names;
}

QString DataInfo::displayPropertyName(int index) const
{
    S_D(const DataInfo);
    return fieldPropertyName(d->displayFieldIndexes.value(index));
}

QMetaProperty DataInfo::displayProperty(int index) const
{
    S_D(const DataInfo);
    return fieldProperty(d->displayFieldIndexes.value(index));
}

int DataInfo::displayCount() const
{
    S_D(const DataInfo);
    return d->displayFieldIndexes.size();
}

QSqlRecord DataInfo::displayRecord() const
{
    S_D(const DataInfo);

    QSqlRecord record;
    for (int index : d->displayFieldIndexes)
        record.append(d->tableRecord.field(index));
    return record;
}

QStringList DataInfo::searchFieldNames() const
{
    S_D(const DataInfo);

    QStringList fields;
    for (int field : d->searchFieldIndexes)
        fields.append(d->tableRecord.fieldName(field));
    return fields;
}

QStringList DataInfo::searchPropertyNames() const
{
    S_D(const DataInfo);

    QStringList names;
    for (int index : d->searchFieldIndexes)
        names.append(fieldPropertyName(index));
    return names;
}

QStringList DataInfo::fieldNames() const
{
    QStringList names;
    for (int i(0); i < count(); ++i)
        names.append(fieldName(i));
    return names;
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

QStringList DataInfo::fieldPropertyNames() const
{
    QStringList names;
    for (int i(0); i < count(); ++i)
        names.append(fieldPropertyName(i));
    return names;
}

QString DataInfo::fieldPropertyName(int index) const
{
    S_D(const DataInfo);
    if (d->fieldProperties.contains(index))
        return d->metaObject->property(d->fieldProperties.value(index)).name();
    else
        return DataInfoPrivate::propertyNameFromFieldName(d->tableRecord.fieldName(index));
}

QMetaProperty DataInfo::fieldProperty(int index) const
{
    S_D(const DataInfo);
    if (d->fieldProperties.contains(index))
        return d->metaObject->property(d->fieldProperties.value(index));
    else
        return QMetaProperty();
}

int DataInfo::count() const
{
    S_D(const DataInfo);
    return d->tableRecord.count();
}

QSqlRecord DataInfo::record() const
{
    S_D(const DataInfo);
    return d->tableRecord;
}

QString DataInfo::foreignFieldName() const
{
    S_D(const DataInfo);
    return d->tableName.toLower().chopped(1) + '_' + idFieldName();
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
    return DataInfoPrivate::propertyNameFromFieldName(foreignFieldName());
}

void DataInfo::dumpInfos()
{
    qInfo().noquote() << "DataInfo(" << tableName() << ")\n{";
    qInfo().noquote() << '}';
}

QList<Data> DataInfo::find(const DataSearch &query, QSqlError *error) const
{
    S_D(const DataInfo);
    if (d->findFunctions.contains(d->metaObject->className()))
        return d->findFunctions.value(d->metaObject->className())(query, error);
    else {
        qInfo().noquote().nospace() << Q_FUNC_INFO << ": No find function registered !";
        return QList<Data>();
    }
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
    return DataInfoPrivate::registry.value(className);
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

        d->displayFieldIndexes.append(i);

        if (field.metaType() == QMetaType::fromType<QString>())
            d->searchFieldIndexes.append(i);
    }

    d->tableName = tableName;
    d->tableRecord = record;

    return DataInfo(d);
}

void DataInfo::registerInfo(const QByteArray &className, const DataInfo &info)
{
    DataInfoPrivate::registry.insert(className, info);
}

void DataInfo::registerFindFunction(const QByteArray &className, const std::function<FindDataFunction> &function)
{
    DataInfoPrivate::findFunctions.insert(className, function);
}

void DataInfo::registerTransferFunction(const QByteArray &className, const std::function<DataTransferFunction> &function)
{
    DataInfoPrivate::transferFunctions.insert(className, function);
}

void DataInfo::registerJsonGenerationFunction(const QByteArray &className, const std::function<JsonGenerationFunction> &function)
{
    DataInfoPrivate::jsonGenerationFunctions.insert(className, function);
}

QList<QSqlField> DataInfo::fieldsFromString(const QString &str, const QString &tableName, const QString &context)
{
    return DataInfoPrivate::fieldsFromString(str, tableName, context);
}

DataRegistration::DataRegistration(const QByteArray &className) :
    _className(className)
{
}

DataRegistration &DataRegistration::init(const std::function<void ()> &function)
{
    function();
    return *this;
}

DataRegistration &DataRegistration::transfer(const std::function<DataTransferFunction> &function)
{
    DataInfoPrivate::transferFunctions.insert(_className, function);
    return *this;
}

DataRegistration &DataRegistration::jsonGenerator(const std::function<JsonGenerationFunction> &function)
{
    DataInfoPrivate::jsonGenerationFunctions.insert(_className, function);
    return *this;
}

DataPrivate::DataPrivate() :
    id(0),
    initialized(false),
    className(Data::staticMetaObject.className())
{
}

QVariant DataPrivate::property(const QString &name) const
{
    return properties.value(name);
}

void DataPrivate::setProperty(const QString &name, const QVariant &value)
{
    properties.insert(name, value);
}

bool DataPrivate::equalsTo(const DataPrivate *other) const
{
    return id == other->id
        && properties == other->properties
        && className == other->className;
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

    fieldProperties.clear();
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
            const int index = tableRecord.count();

            QSqlField field(fieldNameFromPropertyName(property.name()), property.metaType(), tableName);
            field.setRequired(property.isRequired());
            tableRecord.append(field);

            if (index == 0)
                idFieldIndex = index;

            if (property.isUser())
                userFieldIndex = index;

            if (property.metaType() == QMetaType::fromType<QString>())
                searchFieldIndexes.append(index);

            if (index > 0)
                displayFieldIndexes.append(index);

            fieldProperties.insert(index, property.propertyIndex());
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
            if (c == ' ' || (c == ',' && field.isEmpty()))
                continue;
            else
                field.append(c);

            if (c == ')') {
                extraFields.append(field);
                field.clear();
            }
        }
    }

    QList<QSqlField> fields;

    static QRegularExpression regExp("^(?<name>[a-zA-Z_]+)\\((?<type>[A-Za-z_]+),?(?<options>[a-z, ]*)\\)$");

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

QString DataInfoPrivate::propertyNameFromFieldName(const QString &fieldName)
{
    QString name;
    bool upperNext = false;
    for (const QChar &c : fieldName) {
        if (c != '_') {
            if (!upperNext)
                name.append(c);
            else {
                name.append(c.toUpper());
                upperNext = false;
            }
        } else
            upperNext = true;
    }
    return name;
}

QHash<QByteArray, DataInfo> DataInfoPrivate::registry;
QHash<QByteArray, std::function<FindDataFunction>> DataInfoPrivate::findFunctions;
QHash<QByteArray, std::function<JsonGenerationFunction>> DataInfoPrivate::jsonGenerationFunctions;
QHash<QByteArray, std::function<DataTransferFunction>> DataInfoPrivate::transferFunctions;

}
