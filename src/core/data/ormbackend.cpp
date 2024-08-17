#include "ormbackend.h"
#include "ormbackend_p.h"

#include <SystemusCore/private/debug_p.h>
#include <SystemusCore/namespace.h>
#include <SystemusCore/data.h>

#include <QtSql/qsqldriver.h>
#include <QtSql/qsqlindex.h>
#include <QtSql/qsqlrecord.h>
#include <QtSql/qsqlfield.h>

#include <QtCore/qmetaobject.h>
#include <QtCore/qregularexpression.h>

namespace Systemus {
namespace Orm {

Backend::Backend()
{
}

QString Backend::statement(int type, const Data *object, const QMetaObject *metaObject, bool prepared) const
{
    const QString tableName = tableNameFromMetaObject(metaObject);
    QSqlRecord record = recordFromMetaObject(metaObject);
    QSqlField idField = record.field(0);

    QString statement;
    const QSqlDriver *driver = sqlDriver();

    switch (type) {
    case SelectStatement:
        statement = driver->sqlStatement(QSqlDriver::SelectStatement, tableName, record, prepared);
        break;

    case UpdateStatement:
        record.remove(0);
        statement = driver->sqlStatement(QSqlDriver::UpdateStatement, tableName, record, prepared);
        break;

    case InsertStatement:
        record.remove(0);
        statement = driver->sqlStatement(QSqlDriver::UpdateStatement, tableName, record, prepared);
        break;

    case DeleteStatement:
        statement = driver->sqlStatement(QSqlDriver::DeleteStatement, tableName, QSqlRecord(), prepared);
        break;

    default:
        return QString();
    }

    switch (type) {
    case SelectStatement:
    case UpdateStatement:
    case DeleteStatement:
        if (object) {
            const QVariant id = object->id();
            if (!id.isNull()) {
                idField.setValue(id);

                QSqlRecord record;
                record.append(idField);
                statement.append(' ' + driver->sqlStatement(QSqlDriver::WhereStatement, tableName, record, prepared));
            }
        }
        break;

    default:
        break;
    }

    return statement;
}

QString Backend::tableNameFromMetaObject(const QMetaObject *metaObject) const
{
    const int tableIndex = metaObject->indexOfClassInfo("table");
    return tableNameFromClassName(tableIndex >= 0 ? metaObject->classInfo(tableIndex).value() : metaObject->className());
}

QSqlRecord Backend::recordFromMetaObject(const QMetaObject *metaObject) const
{
    QSqlRecord record;

    const int primaryIndex = metaObject->indexOfClassInfo("primary");
    if (primaryIndex >= 0) {
        const QSqlField field = fieldFromString(metaObject->classInfo(primaryIndex).value(), metaObject->className());
        if (field.isValid())
            record.append(field);
    }

    for (int i(0); i < metaObject->propertyCount(); ++i) {
        const QMetaProperty property = metaObject->property(i);
        if (property.isStored())
            record.append(fieldFromMetaProperty(property));
    }

    const int fieldsIndex = metaObject->indexOfClassInfo("properties");
    if (fieldsIndex >= 0) {
        const QList<QSqlField> fields = fieldsFromString(metaObject->classInfo(fieldsIndex).value(), metaObject->className());
        for (const QSqlField &field : fields)
            record.append(field);
    }

    return record;
}

QSqlField Backend::fieldFromMetaProperty(const QMetaProperty &property) const
{
    QSqlField field;
    field.setName(fieldNameFromPropertyName(property.name()));
    field.setMetaType(property.metaType());
    field.setTableName(tableNameFromMetaObject(property.enclosingMetaObject()));
    field.setRequired(property.isRequired());
    field.setReadOnly(property.isReadable() && !property.isWritable());
    field.setGenerated(true);
    return field;
}

QList<QSqlField> Backend::fieldsFromString(const QString &str, const QString &className) const
{
    QList<QSqlField> fields;

    QStringList fieldStrings = str.split(',');
    for (QString &fieldStr : fieldStrings) {
        fieldStr.remove(' ');

        const QSqlField field = fieldFromString(fieldStr, className);
        if (field.isValid())
            fields.append(field);
    }

    return fields;
}

QSqlField Backend::fieldFromString(const QString &str, const QString &className) const
{
    static const QRegularExpression regExp("^(?<name>[a-zA-Z]+)\\((?<type>[A-Za-z]+),?(?<options>[a-z, ]*)\\)$");

    const QRegularExpressionMatch match = regExp.match(str.trimmed());
    if (match.hasMatch()) {
        QSqlField field;
        field.setName(fieldNameFromPropertyName(match.captured("name")));
        field.setMetaType(QMetaType::fromName(match.captured("type").toLatin1()));
        field.setTableName(tableNameFromClassName(className));

        if (match.hasCaptured("options")) {
            const QStringList options = match.captured("options").remove(' ').split(',', Qt::SkipEmptyParts);
            for (const QString &option : options) {
                if (option.startsWith("length")) {
                    bool ok;
                    int length = option.section('=', -1).toInt(&ok);
                    if (ok)
                        field.setLength(length);
                } else if (option == "required") {
                    field.setRequired(true);
                }
            }
        }

        return field;
    } else {
        systemusWarning() << name() << " backend: unable to parse property data \"" << str << "\" on " << className << " class";
        return QSqlField();
    }
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

QString DefaultBackend::name() const
{
    return QStringLiteral("Systemus");
}

QString DefaultBackend::tableNameFromClassName(const QString &className) const
{
    return className.section("::", -1) + 's';
}

QString DefaultBackend::classNameFromTableName(const QString &tableName) const
{
    return tableName.section(".", -1).chopped(1);
}

QString DefaultBackend::fieldNameFromPropertyName(const QString &propertyName) const
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

QString DefaultBackend::propertyNameFromFieldName(const QString &fieldName) const
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

}
}
