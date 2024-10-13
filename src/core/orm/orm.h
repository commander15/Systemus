#ifndef SYSTEMUS_ORM_H
#define SYSTEMUS_ORM_H

#include <SystemusCore/global.h>
#include <SystemusCore/namespace.h>

namespace Systemus {

namespace Orm {

class Backend;
class MetaTable;

enum NamingConvention {
    SystemusConvention,
    LaravelConvention
};

SYSTEMUS_CORE_EXPORT void init(int convention = SystemusConvention);
template<typename Backend>
SYSTEMUS_CORE_EXPORT inline void init() { init(new Backend()); }
SYSTEMUS_CORE_EXPORT void init(Backend *backend);

SYSTEMUS_CORE_EXPORT void registerClasses();

SYSTEMUS_CORE_EXPORT Backend *backend();
SYSTEMUS_CORE_EXPORT void setBackend(Backend *backend);

SYSTEMUS_CORE_EXPORT QString formatExpression(const QString &expression);
SYSTEMUS_CORE_EXPORT QString formatExpression(const QString &expression, const QString &contextClassName);
SYSTEMUS_CORE_EXPORT QString formatValue(const QVariant &value);

}

}

#endif // SYSTEMUS_ORM_H
