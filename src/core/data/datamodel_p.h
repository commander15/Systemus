#ifndef SYSTEMUS_DATAMODEL_P_H
#define SYSTEMUS_DATAMODEL_P_H

#include <SystemusCore/datamodel.h>
#include <SystemusCore/data.h>

#include <QtSql/qsqlerror.h>

namespace Systemus {

class DataModelPrivate : public DataSearch
{
public:
    DataModelPrivate(DataModel *qq);
    ~DataModelPrivate();

    QString columnName(int index) const;
    int itemNumber(int index) const;

    QString filter() const;
    void setFilter(const QString &filter);

    void setSort(int column, Qt::SortOrder order);

    DataModel *q;

    QHash<int, QString> headerData;

    DataInfo info;
    QList<Data> data;

    QSqlError lastError;
};

}

#endif // SYSTEMUS_DATAMODEL_P_H
