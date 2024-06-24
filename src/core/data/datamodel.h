#ifndef SYSTEMUS_DATAMODEL_H
#define SYSTEMUS_DATAMODEL_H

#include <SystemusCore/global.h>

#include <QtSql/qsqlquerymodel.h>

namespace Systemus {

class Data;

class DataModelPrivate;
class SYSTEMUS_CORE_EXPORT DataModel : public QSqlQueryModel
{
public:
    explicit DataModel(QObject *parent = nullptr);
    virtual ~DataModel();

    Data model() const;
    void setModel(const Data &data);

    QString filter() const;
    void setFilter(const QString &filter);

    void sort(int column, Qt::SortOrder order) override;
    void setSort(int column, Qt::SortOrder order);

    virtual bool select();

protected:
    virtual QString selectStatement() const;

private:
    QScopedPointer<DataModelPrivate> d_ptr;
};

}

#endif // SYSTEMUS_DATAMODEL_H
