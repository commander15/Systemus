#ifndef SYSTEMUS_QUERYBUILDER_H
#define SYSTEMUS_QUERYBUILDER_H

#include <SystemusCore/global.h>

class QMetaObject;

class QSqlDriver;

namespace Systemus {
namespace Orm {

class Data;

class MetaTable;

class SYSTEMUS_CORE_EXPORT QueryBuilder
{
public:
    static QString selectStatement(const QString &className);
    static QString selectStatement(const MetaTable &table);

    static QString insertStatement(const Data *data);
    static QString insertStatement(const Data *data, const MetaTable &table);

    static QString updateStatement(const Data *data);
    static QString updateStatement(const Data *data, const MetaTable &table);

    static QString deleteStatement(const QString &className);
    static QString deleteStatement(const MetaTable &table);

    static QString whereStatement(const Data *data);
    static QString whereStatement(const Data *data, const MetaTable &table);
    static QString whereStatement(const QVariant &primaryValue, const MetaTable &table);

private:
    static QSqlDriver *sqlDriver();
};

}
}

#endif // SYSTEMUS_QUERYBUILDER_H
