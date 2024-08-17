#ifndef SYSTEMUS_NAMESPACE_P_H
#define SYSTEMUS_NAMESPACE_P_H

#include <SystemusCore/namespace.h>

#include <QtSql/qsqldatabase.h>

namespace SystemusPrivate {

static QString dbConnection = QSqlDatabase::defaultConnection;

}

#endif // SYSTEMUS_NAMESPACE_P_H
