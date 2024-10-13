#ifndef SYSTEMUS_METAMAPPER_H
#define SYSTEMUS_METAMAPPER_H

#include <SystemusCore/global.h>

class QMetaObject;
class QStringView;

class QSqlRecord;

namespace Systemus {
namespace Orm {

class MetaTable;

class SYSTEMUS_CORE_EXPORT MetaMapper
{
public:
    enum MapOption {
        NoFieldMapOption = 0x0,
        EscapeIdentifiers = 0x1,
        IncludeTableName = 0x2
    };
    Q_DECLARE_FLAGS(MapOptions, MapOption)

    static QString tableName(const QMetaObject *metaObject, int options = NoFieldMapOption);
    static QString tableName(const QString &className, int options = NoFieldMapOption);
    static QString tableName(const MetaTable &table, int options = NoFieldMapOption);
    static QString defaultTableName(const QString &className, int options = NoFieldMapOption);

    static QString fieldName(const QString &propertyName, int options = NoFieldMapOption);
    static QString fieldName(const QString &propertyName, const QMetaObject *metaObject, int options = NoFieldMapOption);
    static QString fieldName(const QString &propertyName, const QString &className, int options = NoFieldMapOption);
    static QString fieldName(const QString &propertyName, const MetaTable &table, int options = NoFieldMapOption);
    static QString defaultFieldName(const QString &propertyName, const QString &className, int options = NoFieldMapOption);

    static QSqlRecord record(const QMetaObject *object);
    static QSqlRecord record(const QString &className);
    static QSqlRecord record(const MetaTable &table);
};

}
}

#endif // SYSTEMUS_METAMAPPER_H
