#ifndef SYSTEMUS_ORMBACKEND_P_H
#define SYSTEMUS_ORMBACKEND_P_H

#include <SystemusCore/ormbackend.h>

namespace Systemus {
namespace Orm {

class DefaultBackend : public Backend
{
public:
    QString name() const override;

    QString tableNameFromClassName(const QString &className) const override;
    QString classNameFromTableName(const QString &tableName) const override;

    QString fieldNameFromPropertyName(const QString &propertyName) const override;
    QString propertyNameFromFieldName(const QString &fieldName) const override;
};

}
}

#endif // SYSTEMUS_ORMBACKEND_P_H
