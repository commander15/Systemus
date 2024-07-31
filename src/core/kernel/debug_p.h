#ifndef SYSTEMUS_DEBUG_P_H
#define SYSTEMUS_DEBUG_P_H

#include <SystemusCore/global.h>

#include <QtCore/qloggingcategory.h>

#define systemusSqlInfo()    qCInfo(systemusSql).noquote().nospace()
#define systemusSqlWarning() qCWarning(systemusSql).noquote().nospace()

Q_DECLARE_LOGGING_CATEGORY(systemusSql)

#endif // SYSTEMUS_DEBUG_P_H
