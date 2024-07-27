#ifndef SYSTEMUS_DATATABLEMODEL_P_H
#define SYSTEMUS_DATATABLEMODEL_P_H

#include <SystemusCore/datatablemodel.h>

#include <SystemusCore/private/abstractdatamodel_p.h>

namespace Systemus {

class DataTableProperty;

class DataTableModelPrivate : public AbstractDataModelPrivate
{
public:
    DataTableModelPrivate(DataTableModel *q);

    QString columnName(int index) const;
    int columnCount() const;

    int itemNumber(int index) const;
    QVariant itemData(int row, int propertyIndex) const;
    bool setItemData(int row, int propertyIndex, const QVariant &data);

    QString propertyName(int index) const;

    QHash<int, QVariant> headerData;
    QList<DataTableProperty> properties;
};

class DataTableProperty
{
public:
    DataTableProperty(const QString &name, DataTableModelPrivate *model, std::function<QVariant (const Data &)> getter);
    DataTableProperty(const QString &name, DataTableModelPrivate *model, std::function<QVariant (const Data &)> getter, std::function<void(Data *, const QVariant &)> setter);
    DataTableProperty(int index, DataTableModelPrivate *model);

    QString name() const;
    QVariant value(int index) const;
    void setValue(int index, const QVariant &value);

private:
    QString *_name;
    std::function<QVariant(const Data &)> _getter;
    std::function<void(Data *, const QVariant &)> _setter;

    int _index;
    DataTableModelPrivate *_model;

};

}

#endif // SYSTEMUS_DATATABLEMODEL_P_H
