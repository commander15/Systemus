#ifndef SYSTEMUS_ORMBACKEND_P_H
#define SYSTEMUS_ORMBACKEND_P_H

#include <SystemusCore/ormbackend.h>

namespace Systemus {
namespace Orm {

class DefaultBackend : public Backend
{
public:
    DefaultBackend();
    ~DefaultBackend();

    QString name() const override;

    QString tableNameFromClassName(const QString &className) const override;
    QString foreignFieldNameFromPropertyName(const QString &propertyName, const QString &className) const override;
    QString fieldNameFromPropertyName(const QString &propertyName, const QString &className) const override;

    QString associationTableNameFromClassNames(const QString &c1, const QString &c2) const override;

    Backend *m_backend;
};

}
}

#endif // SYSTEMUS_ORMBACKEND_P_H
