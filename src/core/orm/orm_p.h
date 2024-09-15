#ifndef SYSTEMUS_ORM_P_H
#define SYSTEMUS_ORM_P_H

#include <SystemusCore/orm.h>

namespace Systemus {

namespace OrmPrivate {

enum OrmOptions {
    NoOptions = 0x0
};

QFlags<OrmOptions> ormOptions;

QStringList sqlKeywords();

}

}

#endif // SYSTEMUS_ORM_P_H
