#ifndef SYSTEMUS_DEBUG_P_H
#define SYSTEMUS_DEBUG_P_H

#include <SystemusCore/global.h>

#include <QtCore/qloggingcategory.h>

#define systemusSqlInfo()    qCInfo(systemusSql)
#define systemusSqlWarning() qCWarning(systemusSql)

Q_DECLARE_LOGGING_CATEGORY(systemusSql)

#endif // SYSTEMUS_DEBUG_P_H
