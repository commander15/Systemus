#ifndef SYSTEMUS_METATABLE_P_H
#define SYSTEMUS_METATABLE_P_H

#include "metatable.h"

#include <SystemusCore/secretproperty.h>

#include <QtCore/qmetaobject.h>
#include <QtCore/qmap.h>

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

    QString foreignPropertyName;
    QString foreignFieldName;

    std::function<void(const void *, void *)> assignmentFunction = nullptr;
};

}
}

#endif // SYSTEMUS_METATABLE_P_H
