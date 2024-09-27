#include "ormbackend.h"
#include "ormbackend_p.h"

#include <SystemusCore/private/debug_p.h>
#include <SystemusCore/namespace.h>
#include <SystemusCore/data.h>
#include <SystemusCore/secretproperty.h>
#include <SystemusCore/private/secretproperty_p.h>

#include <QtSql/qsqldriver.h>
#include <QtSql/qsqlrecord.h>
#include <QtSql/qsqlfield.h>

#include <QtCore/qmetaobject.h>
#include <QtCore/qregularexpression.h>

namespace Systemus {
namespace Orm {

Backend::Backend()
{
}

Backend::~Backend()
{
}

QList<SecretProperty> Backend::secretPropertiesFromMetaObject(const QMetaObject *metaObject) const
{
    int index = metaObject->indexOfClassInfo("properties");
    if (index >= 0)
        return secretPropertiesFromString(metaObject->classInfo(index).value(), metaObject);
    else
        return QList<SecretProperty>();
}

QList<SecretProperty> Backend::secretPropertiesFromString(const QString &str, const QMetaObject *metaObject) const
{
    if (str.isEmpty())
        return QList<SecretProperty>();

    QList<SecretProperty> properties;

    static const QRegularExpression regExp("([a-z][A-Za-z0-9_]+)[ ]*\\([ ]*([A-Za-z]+)[ ]*,?[ ]*([A-Za-z0-9, =]*)[ ]*\\)");

    QRegularExpressionMatchIterator it = regExp.globalMatch(str);
    if (!it.hasNext()) {
        systemusWarning() << name() << " backend: unable to parse property data \"" << str << "\" on " << metaObject->className() << " class";
        return QList<SecretProperty>();
    }

    while (it.hasNext()) {
        const QRegularExpressionMatch match = it.next();

        const QString propertyName = match.captured(1).trimmed();

        SecretPropertyPrivate *d = new SecretPropertyPrivate();

        int fieldNameIndex;
        {
            const int size = 60;
            char name[size + 1];
            strncpy(name, propertyName.toStdString().c_str(), size);
            strncat(name, "Field", size);

            fieldNameIndex = metaObject->indexOfClassInfo(name);
        }

        const int size = 60;
        d->m_name = new char[size + 1];

        if (fieldNameIndex >= 0)
            strncpy(d->m_name, metaObject->classInfo(fieldNameIndex).value(), size);
        else
            strncpy(d->m_name, propertyName.toStdString().c_str(), size);

        d->m_metaType = QMetaType::fromName(match.captured(2).trimmed().toLatin1()).id();

        if (match.hasCaptured(3)) {
            QStringList options = match.captured(3).split(',', Qt::SkipEmptyParts);
            for (QString &option : options) {
                option = option.trimmed();

                if (option == "required") {
                    d->m_required = true;
                }
            }
        }

        properties.append(SecretProperty(d));
    }

    return properties;
}

QList<SecretProperty> Backend::searchPropertiesFromMetaObject(const QMetaObject *metaObject, QList<int> *metaIndexes) const
{
    QList<SecretProperty> properties;

    QList<QMetaProperty> metaProperties = searchProperties(metaObject);
    for (const QMetaProperty &property : metaProperties) {
        properties.append(SecretProperty(property));

        if (metaIndexes)
            metaIndexes->append(property.propertyIndex());
    }

    return properties;
}

QString Backend::foreignPropertyNameFromMetaObject(const QMetaObject *metaObject) const
{
    const int index = metaObject->indexOfClassInfo("foreignProperty");
    if (index >= 0)
        return metaObject->classInfo(index).value();
    else
        return foreignPropertyNameFromPropertyName(primaryProperty(metaObject).name(), metaObject->className());
}

QString Backend::associationClassNameFromMetaObjects(const QMetaObject *o1, const QMetaObject *o2) const
{
    return associationClassNameFromClassNames(o1->className(), o2->className());
}

QString Backend::associationClassNameFromClassNames(const QString &c1, const QString &c2) const
{
    return c1 + c2;
}

QString Backend::tableNameFromMetaObject(const QMetaObject *metaObject) const
{
    const int tableIndex = metaObject->indexOfClassInfo("table");
    return (tableIndex >= 0 ? metaObject->classInfo(tableIndex).value() : tableNameFromClassName(metaObject->className()));
}

QSqlField Backend::primaryFieldFromMetaObject(const QMetaObject *metaObject, int *index) const
{
    const QMetaProperty property = primaryProperty(metaObject);
    if (index)
        *index = (property.isValid() ? property.propertyIndex() : -1);
    return fieldFromMetaProperty(property, metaObject);
}

QSqlField Backend::userFieldFromMetaObject(const QMetaObject *metaObject, int *index) const
{
    const QMetaProperty property = userProperty(metaObject);
    if (index)
        *index = (property.isValid() ? property.propertyIndex() : -1);
    return fieldFromMetaProperty(property, metaObject);
}

QSqlField Backend::foreignFieldFromMetaObject(const QMetaObject *metaObject) const
{
    QSqlField field = primaryFieldFromMetaObject(metaObject);
    field.setName(foreignFieldNameFromPropertyName(primaryProperty(metaObject).name(), metaObject->className()));
    return field;
}

QSqlRecord Backend::recordFromMetaObject(const QMetaObject *metaObject, QMap<int, int> *metaIndexes, QMap<int, int> *secretIndexes) const
{
    QSqlRecord record;

    for (int i(0); i < metaObject->propertyCount(); ++i) {
        const QMetaProperty property = metaObject->property(i);
        if (isDataProperty(property)) {
            record.append(fieldFromMetaProperty(property, metaObject));

            if (metaIndexes)
                metaIndexes->insert(i, record.count() - 1);
        }
    }

    auto secretProperties = secretPropertiesFromMetaObject(metaObject);
    for (int i(0); i < secretProperties.count(); ++i) {
        const SecretProperty property = secretProperties.at(i);
        record.append(fieldFromSecretProperty(property, metaObject));

        if (secretIndexes)
            secretIndexes->insert(i, record.count() - 1);
    }

    return record;
}

QSqlField Backend::fieldFromMetaProperty(const QMetaProperty &property, const QMetaObject *metaObject) const
{
    if (!isDataProperty(property))
        return QSqlField();

    QSqlField field;

    const int fieldNameSize = 60;
    char fieldName[fieldNameSize + 1];
    strncpy(fieldName, property.name(), fieldNameSize);
    strncat(fieldName, "Field", fieldNameSize);

    const int fieldNameIndex = metaObject->indexOfClassInfo(fieldName);
    if (fieldNameIndex >= 0)
        field.setName(metaObject->classInfo(fieldNameIndex).value());
    else
        field.setName(fieldNameFromPropertyName(property.name(), metaObject->className()));

    field.setMetaType(property.metaType());
    field.setTableName(tableNameFromMetaObject(metaObject));
    field.setRequired(property.isRequired());
    field.setReadOnly(property.isReadable() && !property.isWritable());
    field.setGenerated(true);

    return field;
}

QSqlField Backend::fieldFromSecretProperty(const SecretProperty &property, const QMetaObject *metaObject) const
{
    QSqlField field;

    const int fieldNameSize = 60;
    char fieldName[fieldNameSize + 1];
    strncpy(fieldName, property.name(), fieldNameSize);
    strncat(fieldName, "Field", fieldNameSize);

    const int fieldNameIndex = metaObject->indexOfClassInfo(fieldName);
    if (fieldNameIndex >= 0)
        field.setName(metaObject->classInfo(fieldNameIndex).value());
    else
        field.setName(fieldNameFromPropertyName(property.name(), metaObject->className()));

    field.setMetaType(property.metaType());
    field.setTableName(tableNameFromMetaObject(metaObject));
    field.setRequired(property.isRequired());
    field.setGenerated(true);

    return field;
}

QString Backend::associationTableNameFromMetaObjects(const QMetaObject *o1, const QMetaObject *o2) const
{
    return associationTableNameFromClassNames(o1->className(), o2->className());
}

bool Backend::parsePropertyInput(const QString &input, QString *propertyName, QString *className)
{
    const QStringList parts = input.split('.', Qt::SkipEmptyParts);
    switch (parts.size()) {
    case 0:
        *propertyName = input;
        return false;

    case 1:
        *propertyName = parts.first();
        return false;

    case 2:
    default:
        *propertyName = parts.at(parts.size() - 1);
        *className = parts.at(parts.size() - 2);
        return true;
    }
}

bool Backend::parseFieldInput(const QString &input, QString *fieldName, QString *tableName)
{
    return parsePropertyInput(input, fieldName, tableName);
}

Backend *Backend::instance()
{
    return defaultBackend();
}

Backend *Backend::defaultBackend()
{
    static DefaultBackend instance;
    return &instance;
}

QMetaProperty Backend::primaryProperty(const QMetaObject *metaObject) const
{
    for (int i(0); i < metaObject->propertyCount(); ++i) {
        const QMetaProperty property = metaObject->property(i);
        if (isDataProperty(property))
            return property;
    }
    return QMetaProperty();
}

QMetaProperty Backend::userProperty(const QMetaObject *metaObject) const
{
    const QMetaProperty property = metaObject->userProperty();
    return (isDataProperty(property) ? property : QMetaProperty());
}

QList<QMetaProperty> Backend::searchProperties(const QMetaObject *metaObject) const
{
    const int index = metaObject->indexOfClassInfo("searchProperties");
    if (index == -1)
        return QList<QMetaProperty>();

    QList<QMetaProperty> searchProperties;

    QString properties(metaObject->classInfo(index).value());
    properties.remove(' ');

    QStringList propertyNames = properties.split(',', Qt::SkipEmptyParts);
    for (const QString &propertyName : propertyNames) {
        int index = metaObject->indexOfProperty(propertyName.toStdString().c_str());
        if (index >= 0)
            searchProperties.append(metaObject->property(index));
    }

    return searchProperties;
}

QList<QMetaProperty> Backend::dataProperties(const QMetaObject *metaObject) const
{
    QList<QMetaProperty> properties;
    for (int i(0); i < metaObject->propertyCount(); ++i) {
        const QMetaProperty property = metaObject->property(i);
        if (isDataProperty(property))
            properties.append(property);
    }
    return properties;
}

bool Backend::isDataProperty(const QMetaProperty &property) const
{
    return property.isStored();
}

QString SystemusBackend::name() const
{
    return QStringLiteral("Systemus");
}

QString SystemusBackend::classNameFromTableName(const QString &tableName) const
{
    return tableName;
}

QString SystemusBackend::foreignPropertyNameFromPropertyName(const QString &propertyName, const QString &className) const
{
    QString prop = propertyName;
    prop[0] = prop.at(0).toUpper();
    return className.section("::", -1).toLower() + prop;
}

QString SystemusBackend::propertyNameFromFieldName(const QString &fieldName, const QString &tableName) const
{
    // Systemus don't use table names
    Q_UNUSED(tableName);


    // We just convert the field name from snake case to camel case according to the Systemus naming conventions

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

QString SystemusBackend::tableNameFromClassName(const QString &className) const
{
    // We just drop namespaces and add 's'
    return className.section("::", -1) + 's';
}

QString SystemusBackend::foreignFieldNameFromPropertyName(const QString &propertyName, const QString &className) const
{
    // The foreign field name is just the lower case class name (namespace ignored) suffixed with '_id'
    QString name = className.section("::", -1);
    return name.toLower() + '_' + fieldNameFromPropertyName(propertyName, className);
}

QString SystemusBackend::fieldNameFromPropertyName(const QString &propertyName, const QString &className) const
{
    // Systemus naming don't use class name
    Q_UNUSED(className);

    // We just convert the property name from camel case to snake case
    QString name;
    for (const QChar &c : propertyName) {
        if (c.isLower())
            name.append(c);
        else
            name.append('_' + c.toLower());
    }
    return name;
}

QString SystemusBackend::associationTableNameFromClassNames(const QString &c1, const QString &c2) const
{
    // We build the association class name by concatenation of the class names and we suffix with 's'
    return c1.section("::", -1) + c2.section("::", -1) + 's';
}

DefaultBackend::DefaultBackend() :
    m_backend(new SystemusBackend())
{
}

DefaultBackend::~DefaultBackend()
{
    if (m_backend)
        delete m_backend;
}

QString DefaultBackend::name() const
{
    return m_backend->name();
}

QString DefaultBackend::classNameFromTableName(const QString &tableName) const
{
    return m_backend->classNameFromTableName(tableName);
}

QString DefaultBackend::foreignPropertyNameFromPropertyName(const QString &propertyName, const QString &className) const
{
    return m_backend->foreignPropertyNameFromPropertyName(propertyName, className);
}

QString DefaultBackend::propertyNameFromFieldName(const QString &fieldName, const QString &tableName) const
{
    return m_backend->propertyNameFromFieldName(fieldName, tableName);
}

QString DefaultBackend::tableNameFromClassName(const QString &className) const
{
    QString prop;
    QString cls;

    if (parsePropertyInput(className, &prop, &cls))
        return tableNameFromClassName(cls);
    else
        return m_backend->tableNameFromClassName(className);
}

QString DefaultBackend::foreignFieldNameFromPropertyName(const QString &propertyName, const QString &className) const
{
    return className.section("::", -1).toLower() + '_' + fieldNameFromPropertyName(propertyName, className);
}

QString DefaultBackend::fieldNameFromPropertyName(const QString &propertyName, const QString &className) const
{
    QString prop;
    QString cls;

    if (parsePropertyInput(propertyName, &prop, &cls))
        return fieldNameFromPropertyName(prop, cls);
    else if (propertyName != '*') {
        return m_backend->fieldNameFromPropertyName(propertyName, className);
    } else {
        return propertyName;
    }
}

QString DefaultBackend::associationTableNameFromClassNames(const QString &c1, const QString &c2) const
{
    return tableNameFromClassName(c1.section("::", -1) + c2.section("::", -1));
}

}
}
