#ifndef SYSTEMUS_ABSTRACTDATAMODEL_H
#define SYSTEMUS_ABSTRACTDATAMODEL_H

#include <SystemusCore/global.h>
#include <SystemusCore/data.h>

#include <QtCore/qabstractitemmodel.h>

class QSqlError;

namespace Systemus {

class DataInfo;

class AbstractDataModelPrivate;
class SYSTEMUS_CORE_EXPORT AbstractDataModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit AbstractDataModel(QObject *parent = nullptr);
    virtual ~AbstractDataModel();

    template<typename T>
    T item() const;
    Data item() const;

    template<typename T>
    T item(int row) const;
    Data item(int row) const;

    QByteArray className() const;
    void setClass(const QByteArray &className);
    template<typename T> void setClass();
    void setClass(const Data &data);
    void setClass(const DataInfo &info);

    DataInfo dataInfo() const;

    QList<Data> items() const;
    void setItems(const QList<Data> &items);

    QString searchQuery();
    void setSearchQuery(const QString &query);

    bool isMultiSearchEnabled() const;
    void setMultiSearchEnabled(bool enabled);

    QString filter() const;
    void setFilter(const QString &filter);

    void sort(int column, Qt::SortOrder order) override;
    void setSort(int column, Qt::SortOrder order);

    Q_SLOT bool select();
    Q_SLOT bool selectRow(int row);

    QSqlError lastError() const;

    Q_SLOT void clear();

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

protected:
    AbstractDataModel(AbstractDataModelPrivate *d, QObject *parent = nullptr);

    virtual bool isCustomized() const;

    QScopedPointer<AbstractDataModelPrivate> d_ptr;

    friend class TableView;
};

template<typename T>
inline T AbstractDataModel::item() const
{ return item().to<T>(); }

template<typename T>
inline T AbstractDataModel::item(int row) const
{ return item(row).to<T>(); }

template<typename T>
void AbstractDataModel::setClass()
{ setClass(T::staticMetaObject.className()); }

}

#endif // SYSTEMUS_ABSTRACTDATAMODEL_H
