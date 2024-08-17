#ifndef SYSTEMUS_DATATABLEMODEL_H
#define SYSTEMUS_DATATABLEMODEL_H

#include <SystemusCore/global.h>
#include <SystemusCore/data.h>

#include <QtSql/qsqlquerymodel.h>

namespace Systemus {

class DataTableModelPrivate;
class SYSTEMUS_CORE_EXPORT DataTableModel : public QSqlQueryModel
{
    Q_OBJECT

public:
    explicit DataTableModel(QObject *parent = nullptr);
    virtual ~DataTableModel();

    template<typename T> T item() const;
    Data item() const;

    template<typename T> T item(int row) const;
    Data item(int row) const;

    QByteArray className() const;
    const QMetaObject *classMetaObject() const;
    QSqlRecord classRecord() const;
    DataInfo classDataInfo() const;
    template<typename T> void setClass();
    void setClass(const Data &data);
    void setClass(const QByteArray &className);
    void setClass(const DataInfo &info);

    QStringList properties() const;
    void setProperties(const QStringList &properties);

    QStringList searchProperties() const;
    void setSearchProperties(const QStringList &properties);

    QString filter() const;
    void setFilter(const QString &filter);

    Q_SLOT void search(const QString &query);
    QString searchQuery();
    void setSearchQuery(const QString &query);

    void sort(int column, Qt::SortOrder order) override;
    int sortColumn() const;
    Qt::SortOrder sortOrder() const;
    void setSort(int column, Qt::SortOrder order);

    Q_SLOT bool select();

    QByteArrayList linkedClassNames() const;
    template<typename T> void linkTo();
    template<typename T> void linkTo(const QString &foreignProperty);
    template<typename T> void linkTo(const QString &foreignProperty, const QString &indexProperty);
    void linkTo(const QByteArray &className);
    void linkTo(const QByteArray &className, const QString &foreignProperty);
    void linkTo(const QByteArray &className, const QString &foreignProperty, const QString &indexProperty);

protected:
    Data *itemPointer(int row) const;

    virtual QString selectStement() const;

private:
    QScopedPointer<DataTableModelPrivate> d_ptr;
};

template<typename T>
T DataTableModel::item() const
{ return item().to<T>(); }

template<typename T>
T DataTableModel::item(int row) const
{ return item(row).to<T>(); }

template<typename T>
void DataTableModel::setClass()
{ setClass(DataInfo::fromType<T>()); }

template<typename T>
inline void DataTableModel::linkTo()
{ linkTo(T::staticMetaObject.className()); }

template<typename T>
inline void DataTableModel::linkTo(const QString &foreignProperty)
{ linkTo(T::staticMetaObject.className(), foreignProperty); }

template<typename T>
inline void DataTableModel::linkTo(const QString &foreignProperty, const QString &indexProperty)
{ linkTo(T::staticMetaObject.className(), foreignProperty, indexProperty); }

}

#endif // SYSTEMUS_DATATABLEMODEL_H
