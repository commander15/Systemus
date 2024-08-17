#ifndef SYSTEMUS_DATATABLEMODEL_P_H
#define SYSTEMUS_DATATABLEMODEL_P_H

#include <SystemusCore/datatablemodel.h>

#include <QtCore/qcache.h>

namespace Systemus {

struct DataTableModelLink
{
    QByteArray className;
    QString foreignProperty;
    QString indexProperty;
};

class DataTableModelPrivate : public DataSearch
{
public:
    DataTableModelPrivate(DataTableModel *q);

    Data *item(int index) const;
    void clearItems();

    QString filter() const;
    void setFilter(const QString &filter);

    QString searchQuery();
    void setSearchQuery(const QString &query);

    int sortColumn() const;
    Qt::SortOrder sortOrder() const;
    void setSort(int column, Qt::SortOrder order);

    DataTableModel *q_ptr;

    DataInfo classInfo;
    QStringList properties;
    QStringList searchProperties;
    QVector<DataTableModelLink> links;

private:
    mutable QCache<int, Data> m_items;
    QString m_searchQuery;
};

}

#endif // SYSTEMUS_DATATABLEMODEL_P_H
