#ifndef SYSTEMUS_DEBUG_P_H
#define SYSTEMUS_DEBUG_P_H

#include <QtCore/qloggingcategory.h>

#define systemusInfo()     qCInfo(systemus).noquote().nospace()
#define systemusWarning()  qCWarning(systemus).noquote().nospace()
#define systemusCritical() qCCritical(systemus).noquote().nospace()
#define systemusFatal()    qCFatal(systemus).noquote().nospace()

#define systemusSqlInfo()    qCInfo(systemusSql).noquote().nospace()
#define systemusSqlWarning() qCWarning(systemusSql).noquote().nospace()

Q_DECLARE_LOGGING_CATEGORY(systemus)
Q_DECLARE_LOGGING_CATEGORY(systemusSql)

#endif // SYSTEMUS_DEBUG_P_H
