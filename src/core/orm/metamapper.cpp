#include "metamapper.h"

#include <SystemusCore/ormbackend.h>
#include <SystemusCore/namespace.h>
#include <SystemusCore/metatable.h>

#include <QtSql/qsqlrecord.h>

#include <QtCore/qmetaobject.h>

namespace Systemus {

QString Orm::MetaMapper::tableName(const QMetaObject *metaObject, MapOptions options)
{
    return tableName(metaObject->className(), options);
}

QString Orm::MetaMapper::tableName(const QString &className, MapOptions options)
{
    return tableName(MetaTable::fromClassName(className), options);
}

QString Orm::MetaMapper::tableName(const MetaTable &table, MapOptions options)
{
    if (options.testFlag(EscapeIdentifiers))
        return escapeTableName(table.tableName());
    else
        return table.tableName();
}

QString Orm::MetaMapper::fieldName(const QString &propertyName, const QMetaObject *metaObject, MapOptions options)
{
    return fieldName(propertyName, metaObject->className(), options);
}

QString Orm::MetaMapper::fieldName(const QString &propertyName, const QString &className, MapOptions options)
{
    return fieldName(propertyName, MetaTable::fromClassName(className), options);
}

QString Orm::MetaMapper::fieldName(const QString &propertyName, const MetaTable &table, MapOptions options)
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
    } else {
        QString name;
        if (options.testFlag(IncludeTableName))
            name.append(tableName(table, options) + '.');

        {
            QString field = table.fieldName(table.indexOfProperty(propertyName));
            if (field.isEmpty())
                field = Backend::instance()->fieldNameFromPropertyName(propertyName, table.className());

            if (options.testFlag(EscapeIdentifiers))
                field = escapeFieldName(field);
            name.append(field);
        }
        return name;
    }
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
