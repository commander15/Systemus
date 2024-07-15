#ifndef SYSTEMUS_DATAMODEL_H
#define SYSTEMUS_DATAMODEL_H

#include <SystemusCore/global.h>
#include <SystemusCore/data.h>

#include <QtCore/qabstractitemmodel.h>

class QSqlError;

namespace Systemus {

class DataInfo;

class DataModelPrivate;
class SYSTEMUS_CORE_EXPORT DataModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit DataModel(QObject *parent = nullptr);
    virtual ~DataModel();

    template<typename T>
    Data data(int row) const
    { return data(row).to<T>(); }
    Data data(int row) const;

    DataInfo dataInfo() const;
    void setDataInfo(const DataInfo &info);

    Q_SLOT bool search(const QString &query);
    void setSearchQuery(const QString &query);

    QString filter() const;
    void setFilter(const QString &filter);

    void sort(int column, Qt::SortOrder order) override;
    void setSort(int column, Qt::SortOrder order);

    QSqlError lastError() const;

    Q_SLOT bool select();
    Q_SLOT bool selectRow(int row);

    Q_SLOT void clear();

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override;

    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

private:
    QScopedPointer<DataModelPrivate> d_ptr;
};

}

#endif // SYSTEMUS_DATAMODEL_H
