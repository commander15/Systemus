#ifndef SYSTEMUS_DATAMODEL_P_H
#define SYSTEMUS_DATAMODEL_P_H

#include <SystemusCore/datamodel.h>
#include <SystemusCore/data.h>

namespace Systemus {

class DataModelPrivate : public DataSearch
{
public:
    DataModelPrivate(DataModel *qq);

    QString filter() const;
    void setFilter(const QString &filter);

    void setSort(int column, Qt::SortOrder order);

    DataModel *q;

    Data data;
};

}

#endif // SYSTEMUS_DATAMODEL_P_H
