#ifndef SYSTEMUS_ORM_DATA_P_H
#define SYSTEMUS_ORM_DATA_P_H

#include <SystemusCore/data.h>

#include <QtSql/qsqlerror.h>

#include <QtCore/qshareddata.h>

namespace Systemus {
namespace Orm {

class DataPrivate : public QSharedData
{
public:
    enum DataType {
        DefaultDataType,
        AdapterDataType,
        AuthorizationDataType,
        PrivilegedDataType,
        UserDataType,
        RoleDataType,
        GroupDataType,
        PrivilegeDataType,
        PermissionDataType
    };

    DataPrivate();
    DataPrivate(const DataPrivate &other) = default;
    virtual ~DataPrivate() = default;

    QVariantHash properties;

    QSqlError lastError;

    bool initialized;

    bool isDefaultData() const;
    virtual int dataType() const;

    virtual DataPrivate *clone() const;

private:
    S_DECLARE_PUBLIC(Data)
};

}
}

#endif // SYSTEMUS_ORM_DATA_P_H
