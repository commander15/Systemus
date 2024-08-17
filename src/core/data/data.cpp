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

/*!
 * Constructs a default Data.
 */

Data::Data() :
    d_ptr(new DefaultDataPrivate())
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

Data::Data(const Data &other, bool adapt)
{
    if (adapt) {
        QMetaType metaType = QMetaType::fromName(other.dataClassName());
        if (metaType.isValid()) {
            Data *otherCopy = static_cast<Data *>(metaType.create(&other));
            d_ptr.reset(new AdapterDataPrivate(otherCopy));
        } else {
            qDebug() << "Meta type for " << other.dataClassName() << " not found !";
        }

        qDebug() << "Adapted: ";
        other.dumpInfos();
    }

    if (!d_ptr)
        d_ptr = other.d_ptr;
}

/*!
 * Constructs a copy of the data other passed as argument.
 * \warning Don't use this constructor on base classes, see subclassing Data class section.
 */

Data::Data(const Data &other) :
    Data(other, !other.d_ptr->isAdapted())
{
}

Data::Data(Data &&other) :
    d_ptr(other.d_ptr)
{
}

Data::~Data()
{
}

Data &Data::operator=(const Data &other)
{
    if (this != &other) {
        const QByteArray dataClassName = staticMetaObject.className();
        bool attach = true;
        if (instanceClassName() == dataClassName && other.instanceClassName() != dataClassName) {
            QMetaType metaType = QMetaType::fromName(other.dataClassName());
            if (metaType.isValid()) {
                Data *otherCopy = static_cast<Data *>(metaType.create(&other));
                d_ptr.reset(new AdapterDataPrivate(otherCopy));
                attach = false;
            } else {
                qWarning() << "Systemus::Data: can't adapt " << other.instanceClassName() << "'s object to a generic data object"
                            << "ensure that it has been registered with Q_DECLARE_METATYPE()";
            }
        }

        if (attach)
            d_ptr = other.d_ptr;
    }

    return *this;
}

void Data::init()
{
    S_D(Data);
    initData();
    d->init();
}

int Data::id() const
{
    S_D(const Data);
    return d->id();
}

void Data::setId(int id)
{
    S_D(Data);
    d->setId(id);
}

bool Data::hasProperty(const QString &name) const
{
    S_D(const Data);
    return d->hasProperty(name, this);
}

QVariant Data::property(const QString &name) const
{
    S_D(const Data);
    return d->property(name, this);
}

void Data::setProperty(const QString &name, const QVariant &value)
{
    S_D(Data);
    d->setProperty(name, value, this);
}

bool Data::isValid() const
{
    S_D(const Data);
    return d->isValid();
}

bool Data::isEmpty() const
{
    S_D(const Data);
    return d->isEmpty();
}

void Data::dumpInfos() const
{
    S_D(const Data);
    QJsonDocument doc;
    doc.setObject(toJsonObject());
    qInfo().noquote() << doc.toJson(QJsonDocument::Indented);
}

void Data::fill(const Data &other, bool withId)
{
    S_D(Data);

    const DataInfo info = d->dataInfo();
    for (int i(withId ? 0 : 1); i < info.count(); ++i) {
        const QString property = info.fieldPropertyName(i);
        setProperty(property, other.property(property));
    }
}

void Data::fill(const QSqlRecord &record, bool withId)
{
    S_D(Data);

    if (withId) {
        d->fillWithRecord(record, this);
    } else {
        QSqlRecord fillRecord(record);
        int idIndex = fillRecord.indexOf(d->dataInfo().idFieldName());
        if (idIndex >= 0)
            fillRecord.remove(idIndex);
        d->fillWithRecord(fillRecord, this);
    }
}

void Data::clear()
{
    S_D(Data);
    d->clear();
}

QSqlRecord Data::toSqlRecord() const
{
    S_D(const Data);

    QSqlRecord record = d->dataInfo().record();
    d->fillRecord(&record, this);
    return record;
}

QJsonObject Data::toJsonObject() const
{
    S_D(const Data);

    QJsonObject object;
    d->fillJsonObject(&object, this);
    return object;
}

bool Data::get()
{
    S_D(Data);
    return get(d->dataInfo().idFieldName() + " = " + QString::number(d->id()), true);
}

bool Data::get(int id, bool withExtras)
{
    S_D(Data);
    return get(d->dataInfo().idFieldName() + " = " + QString::number(id), withExtras);
}

bool Data::get(const QString &filter, bool withExtras)
{
    return (withExtras ? getExtras(PreExtra) : true) && getData(filter) && (withExtras ? getExtras(PostExtra) : true);
}

bool Data::getData(const QString &filter)
{
    S_D(Data);

    const QString request = selectStatement() + " WHERE " + DataSearch::formatExpression(filter) + " LIMIT 1";
    bool ok;
    QSqlQuery query = exec(request, &ok);

    if (ok && query.next()) {
        d->fillWithRecord(query.record(), this);
        query.finish();
    }

    return ok;
}

bool Data::getExtras(ExtraType type)
{
    return (d_ptr->isAdapted() ? static_cast<AdapterDataPrivate *>(d_ptr.get())->adaptedData()->getExtras(type) : true);
}

bool Data::save()
{
    S_D(Data);
    
    if (d->id() == 0)
        return insert();
    else
        return update();
}

bool Data::insert()
{
    S_D(Data);

    if (!insertExtras(PreExtra))
        return false;

    const QString request = insertStatement();
    bool ok;
    QSqlQuery query = exec(request, &ok);

    if (ok) {
        d->setId(query.lastInsertId().toInt());
        return insertExtras(PostExtra);
    } else
        return false;
}

bool Data::insertExtras(ExtraType type)
{
    return (d_ptr->isAdapted() ? static_cast<AdapterDataPrivate *>(d_ptr.get())->adaptedData()->insertExtras(type) : true);
}

bool Data::update()
{
    S_D(Data);

    if (!updateExtras(PreExtra))
        return false;

    const QString request = updateStatement() + " WHERE id = " + QString::number(d->id());
    bool ok;
    exec(request, &ok);
    return (ok ? updateExtras(PostExtra) : false);
}

bool Data::updateExtras(ExtraType type)
{
    return (d_ptr->isAdapted() ? static_cast<AdapterDataPrivate *>(d_ptr.get())->adaptedData()->updateExtras(type) : true);
}

bool Data::deleteData()
{
    S_D(Data);

    if (!deleteExtras(PreExtra))
        return false;

    const QString request = deleteStatement() + " WHERE id = " + QString::number(d->id());
    bool ok;
    exec(request, &ok);

    if (ok) {
        d->setId(0);
        return deleteExtras(PostExtra);
    } else
        return false;
}

bool Data::deleteExtras(ExtraType type)
{
    return (d_ptr->isAdapted() ? static_cast<AdapterDataPrivate *>(d_ptr.get())->adaptedData()->deleteExtras(type) : true);
}

DataInfo Data::info() const
{
    S_D(const Data);
    return d->dataInfo();
}

QSqlError Data::lastError() const
{
    S_D(const Data);
    return *d->lastError();
}

bool Data::isAdapted() const
{
    S_D(const Data);
    return d->isAdapted();
}

const Data *Data::adaptedData() const
{
    S_D(const Data);
    if (d->isAdapted())
        return static_cast<const AdapterDataPrivate *>(d)->adaptedData();
    else
        return nullptr;
}

const void *Data::internalData() const
{
    return d_ptr.get();
}

void *Data::internalData()
{
    return d_ptr.get();
}

QSqlQuery Data::prepareQuery(const QString &query)
{
    QSqlQuery qu;
    qu.prepare(query);
    return qu;
}

QSqlQuery Data::execCachedQuery(const QString &query, bool *ok, QSqlError *error)
{
    return execQuery(query, ok, error, true);
}

QSqlQuery Data::execQuery(const QString &query, bool *ok, QSqlError *error, bool cached)
{
    return execQuery(query, QVariantList(), ok, error, cached);
}

QSqlQuery Data::execQuery(const QString &query, const QVariantList &values, bool *ok, QSqlError *error, bool cached)
{
    QSqlQuery qu;
    qu.prepare(query);
    for (const QVariant &value : values)
        qu.addBindValue(value);

    if (ok)
        *ok = execQuery(qu, cached, error);
    else
        execQuery(qu, cached, error);

    return qu;
}

QSqlQuery Data::execQuery(const QString &query, const QVariantHash &values, bool *ok, QSqlError *error, bool cached)
{
    QSqlQuery qu;
    qu.prepare(query);

    const QStringList fields = values.keys();
    for (const QString &field : fields)
        qu.bindValue(field, values.value(field));

    if (ok)
        *ok = execQuery(qu, cached, error);
    else
        execQuery(qu, cached, error);

    return qu;
}

bool Data::execQuery(QSqlQuery &query, bool cached, QSqlError *error)
{
    query.setForwardOnly(!cached);

    bool ok = query.exec();
    systemusSqlInfo() << query.lastQuery();

    if (query.lastError().isValid()) {
        if (error)
            *error = query.lastError();
        systemusSqlWarning() << query.lastError().databaseText();
    }

    return ok;
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

QString Data::formatValue(const QVariant &value)
{
    QSqlField field(QString(), value.metaType());
    field.setValue(value);
    return driver()->formatValue(field);
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
        return d_ptr->equals(d_ptr);
}

void Data::initData()
{
}

bool Data::saveReadOnlyProperty(const QString &name, const QVariant &value)
{
    qWarning().noquote().nospace() << "Systemus::Data: " << "read only property'" << name << "' save not handled !";
    Q_UNUSED(value);
    return false;
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
    return d->dataClassName();
}

void Data::setDataClassName(const QByteArray &name)
{
    S_D(Data);
    d->setDataClassName(name);
}

QSqlQuery Data::exec(const QString &query, bool *ok) const
{
    S_D(const Data);
    return execQuery(query, ok, d->lastError());
}

QString Data::selectStatement() const
{
    S_D(const Data);
    const DataInfo info = d->dataInfo();
    return driver()->sqlStatement(QSqlDriver::SelectStatement, info.tableName(), info.record(), false);
}

QString Data::selectStatement(const DataInfo &info)
{
    return driver()->sqlStatement(QSqlDriver::SelectStatement, info.tableName(), info.record(), false);
}

QString Data::insertStatement() const
{
    S_D(const Data);
    QSqlRecord record = toSqlRecord();
    record.remove(0);
    return driver()->sqlStatement(QSqlDriver::InsertStatement, info().tableName(), record, false);
}

QString Data::updateStatement() const
{
    S_D(const Data);
    QSqlRecord record = toSqlRecord();
    record.remove(0);
    return driver()->sqlStatement(QSqlDriver::UpdateStatement, info().tableName(), record, false);
}

QString Data::deleteStatement() const
{
    S_D(const Data);
    return driver()->sqlStatement(QSqlDriver::DeleteStatement, d->dataInfo().tableName(), QSqlRecord(), false);
}

DataSearch::DataSearch()  :
    _page(0), _itemsPerPage(0)
{
}

DataSearch::DataSearch(const QString &filter, int page, int itemsPerPage) :
    _page(page), _itemsPerPage(itemsPerPage)
{
    this->filter(filter);
}

void DataSearch::filter(const QString &filter)
{
    if (!filter.isEmpty())
        _filters.append(formatExpression(filter));
}

void DataSearch::group(const QString field)
{
    if (!field.isEmpty())
        _groups.append(formatExpression(field));
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
        _sortFields.append(formatExpression(field));
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

QString DataSearch::formatExpression(const QString &filter) {
    static QRegularExpression camelCasePattern(QStringLiteral("\\b[a-z]+(?:[A-Z][a-z]*)+\\b"));
    QString result = filter;

    QRegularExpressionMatchIterator i = camelCasePattern.globalMatch(filter);
    while (i.hasNext()) {
        const QRegularExpressionMatch match = i.next();
        const QString property = match.captured();
        const QString replacement = DataInfo::fieldNameFromPropertyName(property);
        result.replace(match.capturedStart(), match.capturedLength(), replacement);
    }

    return result;
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

QStringList DataInfo::secretFieldNames() const
{
    S_D(const DataInfo);

    QStringList names;
    for (int index : d->secretFieldIndexes)
        names.append(fieldName(index));
    return names;
}

QString DataInfo::secretFieldName(int index) const
{
    S_D(const DataInfo);
    return fieldName(d->secretFieldIndexes.at(index));
}

QSqlField DataInfo::secretField(int index) const
{
    S_D(const DataInfo);
    return field(d->secretFieldIndexes.at(index));
}

QStringList DataInfo::secretPropertyNames() const
{
    S_D(const DataInfo);

    QStringList names;
    for (int index : d->secretFieldIndexes)
        names.append(fieldPropertyName(index));
    return names;
}

QString DataInfo::secretPropertyName(int index) const
{
    S_D(const DataInfo);
    return fieldPropertyName(d->secretFieldIndexes.at(index));
}

QMetaProperty DataInfo::secretProperty(int index) const
{
    S_D(const DataInfo);
    return fieldProperty(d->secretFieldIndexes.at(index));
}

int DataInfo::secretCount() const
{
    S_D(const DataInfo);
    return d->secretFieldIndexes.size();
}

QSqlRecord DataInfo::secretRecord() const
{
    S_D(const DataInfo);

    QSqlRecord record;
    for (int index : d->secretFieldIndexes)
        record.append(field(index));
    return record;
}

QStringList DataInfo::fieldNames() const
{
    QStringList names;
    for (int i(0); i < count(); ++i)
        names.append(fieldName(i));
    return names;
}

int DataInfo::fieldIndex(const QString &name) const
{
    S_D(const DataInfo);
    return d->tableRecord.indexOf(name);
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

int DataInfo::fieldPropertyIndex(const QString &name) const
{
    S_D(const DataInfo);
    return d->tableRecord.indexOf(fieldNameFromPropertyName(name));
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

Data DataInfo::newData() const
{
    return newData(QSqlRecord());
}

Data DataInfo::newData(const QSqlRecord &record) const
{
    S_D(const DataInfo);
    QMetaType type = QMetaType::fromName(d->metaObject->className());
    if (type.isValid()) {
        AdapterDataPrivate *data = new AdapterDataPrivate(static_cast<Data *>(type.create()));
        return Data(data);
    } else {
        return Data();
    }
}

QList<Data> DataInfo::find(const DataSearch &query, bool withExtras, QSqlError *error) const
{
    S_D(const DataInfo);
    if (d->findFunctions.contains(d->metaObject->className()))
        return d->findFunctions.value(d->metaObject->className())(query, withExtras, error);
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

bool DataInfo::isRegistered(const QByteArray &className)
{
    return DataInfoPrivate::registry.contains(className);
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

QString DataInfo::tableNameFromClassName(const QString &className)
{
    return DataInfoPrivate::tableNameFromClassName(className);
}

QString DataInfo::propertyNameFromFieldName(const QString &fieldName)
{
    return DataInfoPrivate::propertyNameFromFieldName(fieldName);
}

QString DataInfo::fieldNameFromPropertyName(const QString &propertyName)
{
    return DataInfoPrivate::fieldNameFromPropertyName(propertyName);
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
    if (isValid())
        function();
    return *this;
}

DataRegistration &DataRegistration::transfer(const std::function<DataTransferFunction> &function)
{
    if (isValid())
        DataInfoPrivate::transferFunctions.insert(_className, function);
    return *this;
}

DataRegistration &DataRegistration::jsonGenerator(const std::function<JsonGenerationFunction> &function)
{
    if (isValid())
        DataInfoPrivate::jsonGenerationFunctions.insert(_className, function);
    return *this;
}

DataPrivate::~DataPrivate()
{
}

void DataPrivate::init()
{
}

bool DataPrivate::hasProperty(const QString &name, const Data *data) const
{
    if (hasDataProperty(name))
        return true;

    int index = data->instanceMetaObject()->indexOfProperty(name.toStdString().c_str());
    if (index >= 0)
        return true;

    return false;
}

QVariant DataPrivate::property(const QString &name, const Data *data) const
{
    if (name == "id")
        return id();
    else if (hasDataProperty(name))
        return dataProperty(name);
    else if (data) {
        const QMetaObject *object = data->instanceMetaObject();
        int index = object->indexOfProperty(name.toStdString().c_str());
        if (index >= 0)
            return object->property(index).readOnGadget(data);
    }

    return QVariant();
}

bool DataPrivate::setProperty(const QString &name, const QVariant &value, Data *data)
{
    if (name == "id") {
        setId(value.toInt());
        return true;
    } else if (hasDataProperty(name)) {
        setDataProperty(name, value);
        return true;
    } else if (data) {
        const QMetaObject *object = data->instanceMetaObject();
        int index = object->indexOfProperty(name.toStdString().c_str());
        if (index >= 0) {
            const QMetaProperty property = object->property(index);
            if (property.isWritable())
                return object->property(index).writeOnGadget(data, value);
            else
                return data->saveReadOnlyProperty(name, value);
        }
    }

    return false;
}

void DataPrivate::fillRecord(QSqlRecord *record, const Data *data) const
{
    const DataInfo info = dataInfo();
    for (int i(0); i < info.count(); ++i) {
        const QString field = info.fieldName(i);
        if (record->contains(field))
            record->setValue(field, property(info.fieldPropertyName(i), data));
    }
}

void DataPrivate::fillWithRecord(const QSqlRecord &record, Data *data)
{
    const DataInfo info = dataInfo();
    for (int i(0); i < info.count(); ++i) {
        const QString field = info.fieldName(i);
        if (record.contains(field))
            setProperty(info.fieldPropertyName(i), record.value(field), data);
    }
}

void DataPrivate::fillJsonObject(QJsonObject *object, const Data *data) const
{
    const DataInfo info = dataInfo();
    for (int i(0); i < info.count(); ++i)
        object->insert(info.fieldName(i), QJsonValue::fromVariant(property(info.fieldPropertyName(i), data)));
}

bool DataPrivate::isAdapted() const
{
    return false;
}

DataInfo DataPrivate::dataInfo() const
{
    return DataInfoPrivate::registry.value(dataClassName());
}

DefaultDataPrivate::DefaultDataPrivate()
{
}

DefaultDataPrivate::~DefaultDataPrivate()
{
}

void DefaultDataPrivate::init()
{
    const QStringList properties = dataInfo().secretPropertyNames();
    for (const QString &property : properties)
        _properties.insert(property, QVariant());
}

QVariant DefaultDataPrivate::id() const
{
    return _id;
}

void DefaultDataPrivate::setId(const QVariant &id)
{
    _id = id;
}

bool DefaultDataPrivate::isValid() const
{
    return !_id.isNull();
}

bool DefaultDataPrivate::isEmpty() const
{
    return _id.isNull() && _properties.isEmpty();
}

bool DefaultDataPrivate::equals(const DataPrivate *o) const
{
    auto *other = static_cast<const DefaultDataPrivate *>(o);
    return _id == other->_id && _properties == other->_properties && _className == other->_className;
}

void DefaultDataPrivate::clear()
{
    _id = 0;

    const QStringList properties = dataInfo().secretPropertyNames();
    for (const QString &property : properties)
        _properties.insert(property, QVariant());
}

QByteArray DefaultDataPrivate::dataClassName() const
{
    return _className;
}

void DefaultDataPrivate::setDataClassName(const QByteArray &className)
{
    _className = className;
}

QSqlError *DefaultDataPrivate::lastError() const
{
    return &_lastError;
}

bool DefaultDataPrivate::hasDataProperty(const QString &name) const
{
    return _properties.contains(name);
}

QVariant DefaultDataPrivate::dataProperty(const QString &name) const
{
    return _properties.value(name);
}

bool DefaultDataPrivate::setDataProperty(const QString &name, const QVariant &value)
{
    _properties.insert(name, value);
    return true;
}

AdapterDataPrivate::AdapterDataPrivate(Data *data) :
    _data(data)
{
}

AdapterDataPrivate::AdapterDataPrivate(const AdapterDataPrivate &other)
{
    QMetaType metaType = QMetaType::fromName(other.dataClassName());
    _data = static_cast<Data *>(metaType.create(other._data));
}

AdapterDataPrivate::~AdapterDataPrivate()
{
    if (_data)
        delete _data;
}

const Data *AdapterDataPrivate::adaptedData() const
{
    return _data;
}

Data *AdapterDataPrivate::adaptedData()
{
    return _data;
}

void AdapterDataPrivate::init()
{
    _data->init();
}

QVariant AdapterDataPrivate::id() const
{
    return _data->id();
}

void AdapterDataPrivate::setId(const QVariant &id)
{
    _data->setId(id);
}

void AdapterDataPrivate::fillRecord(QSqlRecord *record, const Data *data) const
{
    *record = _data->toSqlRecord();
    Q_UNUSED(data);
}

void AdapterDataPrivate::fillWithRecord(const QSqlRecord &record, Data *data)
{
    _data->fill(record, record.contains("id"));
    Q_UNUSED(data);
}

void AdapterDataPrivate::fillJsonObject(QJsonObject *object, const Data *data) const
{
    *object = _data->toJsonObject();
    Q_UNUSED(data);
}

bool AdapterDataPrivate::isValid() const
{
    return _data->isValid();
}

bool AdapterDataPrivate::isEmpty() const
{
    return _data->isEmpty();
}

bool AdapterDataPrivate::isAdapted() const
{
    return true;
}

bool AdapterDataPrivate::equals(const DataPrivate *other) const
{
    return _data->d_ptr->equals(other);
}

void AdapterDataPrivate::clear()
{
    _data->clear();
}

QByteArray AdapterDataPrivate::dataClassName() const
{
    return _data->dataClassName();
}

void AdapterDataPrivate::setDataClassName(const QByteArray &className)
{
    _data->setDataClassName(className);
}

QSqlError *AdapterDataPrivate::lastError() const
{
    return _data->d_ptr->lastError();
}

bool AdapterDataPrivate::hasDataProperty(const QString &name) const
{
    return _data->hasProperty(name);
}

QVariant AdapterDataPrivate::dataProperty(const QString &name) const
{
    return _data->property(name);
}

bool AdapterDataPrivate::setDataProperty(const QString &name, const QVariant &value)
{
    _data->setProperty(name, value);
    return true;
}

DataInfoPrivate::DataInfoPrivate() :
    idFieldIndex(0),
    userFieldIndex(1)
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
    secretFieldIndexes.clear();

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
        for (const QSqlField &field : fields) {
            int index = tableRecord.count();
            tableRecord.append(field);
            secretFieldIndexes.append(index);
        }
    }
}

bool DataInfoPrivate::equals(const DataInfoPrivate &other) const
{
    return idFieldIndex == other.idFieldIndex
        && userFieldIndex == other.userFieldIndex
        && searchFieldIndexes == other.searchFieldIndexes
        && secretFieldIndexes == other.secretFieldIndexes
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

    static QRegularExpression regExp("^(?<name>[a-zA-Z]+)\\((?<type>[A-Za-z]+),?(?<options>[a-z, ]*)\\)$");

    for (const QString &extraField : extraFields) {
        QRegularExpressionMatch match = regExp.match(extraField);

        if (match.hasMatch()) {
            QSqlField field(fieldNameFromPropertyName(match.captured("name")), QMetaType::fromName(match.captured("type").toLatin1()), tableName);

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

template<>
Systemus::DataPrivate *QSharedDataPointer<Systemus::DataPrivate>::clone()
{
    return d->clone();
}
