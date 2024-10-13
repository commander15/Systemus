#include "metamapper.h"

#include <SystemusCore/ormbackend.h>
#include <SystemusCore/namespace.h>
#include <SystemusCore/metatable.h>

#include <QtSql/qsqlrecord.h>

#include <QtCore/qmetaobject.h>

namespace Systemus {

QString Orm::MetaMapper::tableName(const QMetaObject *metaObject, int options)
{
    return tableName(metaObject->className(), options);
}

QString Orm::MetaMapper::tableName(const QString &className, int options)
{
    return tableName(MetaTable::fromClassName(className), options);
}

QString Orm::MetaMapper::tableName(const MetaTable &table, int options)
{
    const MapOptions mapOptions(options);
    if (mapOptions.testFlag(EscapeIdentifiers))
        return escapeTableName(table.tableName());
    else
        return table.tableName();
}

QString Orm::MetaMapper::defaultTableName(const QString &className, int options)
{
    const QString tableName = Backend::instance()->tableNameFromClassName(className);

    const MapOptions mapOptions(options);
    if (mapOptions.testFlag(EscapeIdentifiers))
        return escapeTableName(tableName);
    else
        return tableName;

}

QString Orm::MetaMapper::fieldName(const QString &propertyName, int options)
{
    return fieldName(propertyName, MetaTable(), options);
}

QString Orm::MetaMapper::fieldName(const QString &propertyName, const QMetaObject *metaObject, int options)
{
    return fieldName(propertyName, metaObject->className(), options);
}

QString Orm::MetaMapper::fieldName(const QString &propertyName, const QString &className, int options)
{
    const MetaTable table = MetaTable::fromClassName(className);
    if (table.isValid())
        return fieldName(propertyName, table, options);
    else
        return defaultFieldName(propertyName, className, options);
}

QString Orm::MetaMapper::fieldName(const QString &propertyName, const MetaTable &table, int options)
{
    if (propertyName.contains('.')) {
        const QStringList parts = propertyName.split('.', Qt::SkipEmptyParts);
        switch (parts.size()) {
        case 0:
            return QString();

        case 1:
            return fieldName(propertyName, table, options);

        default:
            return fieldName(parts.at(parts.size() - 1), parts.at(parts.size() - 2), options);
        }
    } else if (table.isValid()) {
        const MapOptions mapOptions(options);

        QString name;
        if (mapOptions.testFlag(IncludeTableName))
            name.append(tableName(table, mapOptions) + '.');

        {
            QString field = table.fieldName(table.indexOfProperty(propertyName));
            if (field.isEmpty())
                field = Backend::instance()->fieldNameFromPropertyName(propertyName, table.className());

            if (mapOptions.testFlag(EscapeIdentifiers))
                field = escapeFieldName(field);
            name.append(field);
        }

        return name;
    } else {
        return QString();
    }
}

QString Orm::MetaMapper::defaultFieldName(const QString &propertyName, const QString &className, int options)
{
    const MapOptions mapOptions(options);
    QString name;

    if (mapOptions.testFlag(IncludeTableName))
        name.append(defaultTableName(className, options) + '.');

    QString field = Backend::instance()->fieldNameFromPropertyName(propertyName, className);
    if (mapOptions.testFlag(EscapeIdentifiers))
        field = escapeFieldName(field);
    name.append(field);

    return name;
}

QSqlRecord Orm::MetaMapper::record(const QMetaObject *object)
{
    return record(MetaTable::fromClassName(object->className()));
}

QSqlRecord Orm::MetaMapper::record(const QString &className)
{
    return record(MetaTable::fromClassName(className));
}

QSqlRecord Orm::MetaMapper::record(const MetaTable &table)
{
    return table.record();
}

}
