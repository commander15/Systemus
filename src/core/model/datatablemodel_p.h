#ifndef SYSTEMUS_DATATABLEMODEL_P_H
#define SYSTEMUS_DATATABLEMODEL_P_H

#include <SystemusCore/datatablemodel.h>

#include <QtCore/qcache.h>

namespace Systemus {
namespace Orm {

struct DataTableModelLink
{
    QString className;
    QString foreignProperty;
    QString indexProperty;
};

class DataTableModelPrivate : public Orm::DataSearch
{
public:
    DataTableModelPrivate(DataTableModel *q);

    Data *item(int index) const;
    void clearItems();

    QString filter() const;
    void setFilter(const QString &filter);

    int sortColumn() const;
    Qt::SortOrder sortOrder() const;
    void setSort(int column, Qt::SortOrder order);

    int page() const;
    void setPage(int page);

    int pageCount() const;
    void setItemCount(int count);

    int itemsPerPage() const;
    void setItemsPerPage(int items);

    DataTableModel *q_ptr;

    MetaTable table;
    QStringList properties;
    QVector<DataTableModelLink> links;

private:
    mutable QHash<int, Data *> m_items;
    int m_totalCount;
};

}
}

#endif // SYSTEMUS_DATATABLEMODEL_P_H
