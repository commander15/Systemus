#ifndef SYSTEMUS_SQLQUERYMODEL_H
#define SYSTEMUS_SQLQUERYMODEL_H

#include <SystemusCore/global.h>

#include <QtSql/qsqlquerymodel.h>

namespace Systemus {
namespace Orm {

class SYSTEMUS_CORE_EXPORT SqlQueryModel : public QSqlQueryModel
{
    Q_OBJECT

public:
    explicit SqlQueryModel(QObject *parent = nullptr);
    virtual ~SqlQueryModel();

    void setQuery(const QString &query, const QString &contextClassName);

protected:
    void setQuery(QSqlQuery &&query);
};

}
}

#endif // SYSTEMUS_SQLQUERYMODEL_H
