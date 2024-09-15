#ifndef SYSTEMUS_ORM_H
#define SYSTEMUS_ORM_H

#include <SystemusCore/global.h>
#include <SystemusCore/namespace.h>

namespace Systemus {

namespace Orm {

class MetaTable;

SYSTEMUS_CORE_EXPORT void init();

SYSTEMUS_CORE_EXPORT QString formatExpression(const QString &expression, const QString &contextClassName);
SYSTEMUS_CORE_EXPORT QString formatValue(const QVariant &value);

}

}

#endif // SYSTEMUS_ORM_H
