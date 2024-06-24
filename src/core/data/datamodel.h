#ifndef SYSTEMUS_DATAMODEL_H
#define SYSTEMUS_DATAMODEL_H

#include <SystemusCore/global.h>

#include <QtSql/qsqlquerymodel.h>

class QSqlField;
class QSqlRelation;

namespace Systemus {

class Data;

class DataModelPrivate;
class SYSTEMUS_CORE_EXPORT DataModel : public QSqlQueryModel
{
public:
    explicit DataModel(QObject *parent = nullptr);
    virtual ~DataModel();

    QString tableName() const;
    void setTableName(const QString &name);

    //QSqlField tableField(int column) const;
    //int tableFieldCount() const;
    int addField(const QString &name, QMetaType::Type type, const QString &table = QString());
    void insertField(int pos, const QString &name, QMetaType::Type type, const QString &table = QString());
    void replaceField(int pos, const QString &name, QMetaType::Type type, const QString &table = QString());
    //void removeField(const QString &name);

    //QSqlRelation tableRelation(int index) const;
    //int tableRelationCount() const;
    int addRelation(const QString &table);
    int addRelation(const QString &table, const QString &foreignIndex);
    int addRelation(const QString &table, const QString &index, const QString &foreignIndex);
    //void removeRelation(const QString &table);

    void setTable(const QString &table);

    QString filter() const;
    void setFilter(const QString &filter);

    void sort(int column, Qt::SortOrder order) override;
    void setSort(int column, Qt::SortOrder order);

    virtual bool select();

    void clear() override;

protected:
    virtual QString selectStatement() const;

private:
    QScopedPointer<DataModelPrivate> d_ptr;
};

}

#endif // SYSTEMUS_DATAMODEL_H
