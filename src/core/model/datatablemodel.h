#ifndef SYSTEMUS_DATATABLEMODEL_H
#define SYSTEMUS_DATATABLEMODEL_H

#include <SystemusCore/global.h>
#include <SystemusCore/data.h>
#include <SystemusCore/sqlquerymodel.h>

namespace Systemus {
namespace Orm {

class DataTableModelPrivate;
class SYSTEMUS_CORE_EXPORT DataTableModel : public SqlQueryModel
{
    Q_OBJECT

public:
    enum LinkMode {
        InnerLink,
        LeftLink,
        RightLink
    };

    explicit DataTableModel(QObject *parent = nullptr);
    virtual ~DataTableModel();

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    template<typename T> T inline item() const;
    Systemus::Data item() const;

    template<typename T> T inline item(int row) const;
    Systemus::Data item(int row) const;

    QByteArray className() const;
    const QMetaObject *classMetaObject() const;
    QMetaType classMetaType() const;
    QString classTableName() const;
    QSqlRecord classRecord() const;
    MetaTable classTable() const;
    template<typename T> void setClass();
    void setClass(const Data &data);
    void setClass(const QByteArray &className);
    void setClass(const MetaTable &table);

    QStringList properties() const;
    void setProperties(const QStringList &properties);

    QString filter() const;
    void setFilter(const QString &filter);

    void sort(int column, Qt::SortOrder order) override;
    int sortColumn() const;
    Qt::SortOrder sortOrder() const;
    void setSort(int column, Qt::SortOrder order);

    int page() const;
    void setPage(int page);

    int pageCount() const;

    int itemsPerPage() const;
    void setItemsPerPage(int items);

    Q_SLOT bool select();

    void clear() override;

    QStringList linkedClassNames() const;
    template<typename T> inline void linkTo();
    template<typename T> inline void linkTo(const QString &foreignProperty);
    template<typename T> inline void linkTo(const QString &foreignProperty, const QString &indexProperty, LinkMode mode = InnerLink);
    void linkTo(const QString &className, LinkMode mode = InnerLink);
    void linkTo(const QString &className, const QString &foreignProperty, LinkMode mode = InnerLink);
    void linkTo(const QString &className, const QString &foreignProperty, const QString &indexProperty, LinkMode mode = InnerLink);

protected:
    enum StatementType {
        DataStatement,
        CountStatement
    };

    virtual QString selectStatement(StatementType type) const;

    void queryChange() override;

private:
    QScopedPointer<DataTableModelPrivate> d_ptr;
};

template<typename T>
inline T DataTableModel::item() const
{ return item().to<T>(); }

template<typename T>
inline T DataTableModel::item(int row) const
{ return item(row).to<T>(); }

template<typename T>
inline void DataTableModel::setClass()
{ setClass(MetaTable::fromType<T>()); }

template<typename T>
inline void DataTableModel::linkTo()
{ linkTo(MetaTable::fromType<T>().className()); }

template<typename T>
inline void DataTableModel::linkTo(const QString &foreignProperty)
{ linkTo(MetaTable::fromType<T>().className(), foreignProperty); }

template<typename T>
inline void DataTableModel::linkTo(const QString &foreignProperty, const QString &indexProperty, LinkMode mode)
{ linkTo(MetaTable::fromType<T>().className(), foreignProperty, indexProperty, mode); }

}
}

#endif // SYSTEMUS_DATATABLEMODEL_H
