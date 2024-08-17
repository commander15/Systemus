#ifndef SYSTEMUS_METATABLE_P_H
#define SYSTEMUS_METATABLE_P_H

#include <SystemusCore/metatable.h>

#include <QtCore/qmetaobject.h>
#include <QtCore/qhash.h>

#include <QtSql/qsqlrecord.h>

namespace Systemus {
namespace Orm {

class MetaTablePrivate : public MetaTableData
{
public:
    QString tableName;

    int primaryFieldIndex = 0;
    int userFieldIndex = 0;
    QList<int> metaFieldIndexes;
    QList<int> secretFieldIndexes;
    QSqlRecord record;
};

}
}

#endif // SYSTEMUS_METATABLE_P_H
