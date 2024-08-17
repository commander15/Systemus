#ifndef SYSTEMUS_ORM_P_H
#define SYSTEMUS_ORM_P_H

#include <SystemusCore/orm.h>

namespace Systemus {

namespace OrmPrivate {

enum OrmOptions {
    NoOptions = 0x0,
    UseNamespaceAsDatabase = 0x1
};

QFlags<OrmOptions> ormOptions;

}

}

#endif // SYSTEMUS_ORM_P_H
