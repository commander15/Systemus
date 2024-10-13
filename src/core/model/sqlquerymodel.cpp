#include "sqlquerymodel.h"

#include <SystemusCore/orm.h>

namespace Systemus {
namespace Orm {

SqlQueryModel::SqlQueryModel(QObject *parent) :
    QSqlQueryModel(parent)
{
}

SqlQueryModel::~SqlQueryModel()
{
}

void SqlQueryModel::setQuery(const QString &query, const QString &contextClassName)
{
    const QString &processedQuery = formatExpression(query, contextClassName);
    QSqlQueryModel::setQuery(processedQuery);
}

void SqlQueryModel::setQuery(QSqlQuery &&query)
{
    QSqlQueryModel::setQuery(std::move(query));
}

}
}
