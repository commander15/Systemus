#ifndef SYSTEMUS_ABSTRACTDATAMODEL_P_H
#define SYSTEMUS_ABSTRACTDATAMODEL_P_H

#include <SystemusCore/abstractdatamodel.h>
#include <SystemusCore/data.h>

#include <QtSql/qsqlerror.h>

namespace Systemus {

class AbstractDataModelPrivate : public DataSearch
{
public:
    AbstractDataModelPrivate(AbstractDataModel *qq);
    ~AbstractDataModelPrivate();

    QString searchQuery() const;
    void setSearchQuery(const QString &query);

    QString filter() const;
    void setFilter(const QString &filter);

    void setSort(int column, Qt::SortOrder order);

    AbstractDataModel *q;

    bool multiSearch;
    bool customFilterSet;

    DataInfo info;
    QList<Data> items;

    QSqlError lastError;

private:
    QString _searchQuery;
};

}

#endif // SYSTEMUS_ABSTRACTDATAMODEL_P_H
