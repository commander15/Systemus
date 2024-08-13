#include "datatablemodel.h"
#include "datatablemodel_p.h"

#include <QtCore/qmetaobject.h>

namespace Systemus {

DataTableModel::DataTableModel(QObject *parent) :
    AbstractDataModel(new DataTableModelPrivate(this), parent)
{
}

DataTableModel::~DataTableModel()
{
}

QStringList DataTableModel::propertyNames() const
{
    S_D(const DataTableModel);
    QStringList properties;
    for (const DataTableProperty &property : d->properties)
        properties.append(property.name());
    return properties;
}

void DataTableModel::setProperties(const QStringList &names)
{
    S_D(DataTableModel);
    beginResetModel();

    d->properties.clear();
    for (const QString &name : names) {
        const int index = d->info.fieldPropertyIndex(name);
        if (index >= 0)
            d->properties.append(DataTableProperty(index, d));
    }

    endResetModel();
}

bool DataTableModel::addProperty(const QString &name)
{
    S_D(DataTableModel);
    const int index = d->info.fieldPropertyIndex(name);

    if (index >= 0) {
        beginResetModel();
        d->properties.append(DataTableProperty(index, d));
        endResetModel();
        return true;
    } else
        return false;
}

bool DataTableModel::addProperty(const QString &name, std::function<QVariant (const Data &)> getter, std::function<void (Data *, const QVariant &)> setter)
{
    S_D(DataTableModel);
    d->properties.append(DataTableProperty(name, d, getter, setter));
    return true;
}

QVariant DataTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    S_D(const DataTableModel);

    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
        if (orientation == Qt::Horizontal)
            return d->columnName(section);
        else
            return d->itemNumber(section);

    default:
        return AbstractDataModel::headerData(section, orientation, role);
    }
}

bool DataTableModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    S_D(DataTableModel);

    if (orientation == Qt::Vertical)
        return setHeaderData(section, orientation, value, role);

    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
        d->headerData.insert(section, value);
        return false;

    default:
        return AbstractDataModel::setHeaderData(section, orientation, role);
    }
}

QVariant DataTableModel::data(const QModelIndex &index, int role) const
{
    S_D(DataTableModel);
    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
        return d->itemData(index.row(), index.column());

    default:
        return QVariant();
    }
}

bool DataTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    S_D(DataTableModel);
    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
        return d->setItemData(index.row(), index.column(), value);

    default:
        return AbstractDataModel::setData(index, value, role);
    }
}

int DataTableModel::columnCount(const QModelIndex &parent) const
{
    S_D(const DataTableModel);
    return (!parent.isValid() ? d->columnCount() : 0);
}

bool DataTableModel::isCustomized() const
{
    S_D(const DataTableModel);
    return !d->properties.isEmpty();
}

DataTableModelPrivate::DataTableModelPrivate(DataTableModel *q) :
    AbstractDataModelPrivate(q)
{
}

QString DataTableModelPrivate::columnName(int index) const
{
    if (headerData.contains(index))
        return headerData.value(index).toString();
    else if (properties.size() > index)
        return properties.at(index).name();
    else
        return info.fieldPropertyName(index);
}

int DataTableModelPrivate::columnCount() const
{
    return (!properties.isEmpty() ? properties.size() : info.count());
}
int DataTableModelPrivate::itemNumber(int index) const
{
    return (_page > 1 ? ((_page - 1) * _itemsPerPage) : 1) + index;
}

QVariant DataTableModelPrivate::itemData(int row, int propertyIndex) const
{
    if (properties.size() > propertyIndex)
        return properties.at(propertyIndex).value(row);
    else
        return items.at(row).property(propertyName(propertyIndex));
}

bool DataTableModelPrivate::setItemData(int row, int propertyIndex, const QVariant &data)
{
    if (properties.size() > row) {
        properties[row].setValue(row, data);
        return true;
    }

    this->items[row].setProperty(propertyName(propertyIndex), data);
    return true;
}

QString DataTableModelPrivate::propertyName(int index) const
{
    return info.fieldPropertyName(index);
}

DataTableProperty::DataTableProperty(const QString &name, DataTableModelPrivate *model, std::function<QVariant (const Data &)> getter) :
    Systemus::DataTableProperty(name, model, getter, nullptr)
{
}

DataTableProperty::DataTableProperty(const QString &name, DataTableModelPrivate *model, std::function<QVariant (const Data &)> getter, std::function<void (Data *, const QVariant &)> setter) :
    _name(new QString(name)),
    _getter(getter),
    _setter(setter),
    _index(0),
    _model(model)
{
}

DataTableProperty::DataTableProperty(int index, DataTableModelPrivate *model) :
    _name(nullptr),
    _getter(nullptr),
    _index(index),
    _model(model)
{
}

QString DataTableProperty::name() const
{
    return (_name ? *_name : _model->propertyName(_index));
}

QVariant DataTableProperty::value(int index) const
{
    const Data data = _model->items.at(index);
    return (_getter ? _getter(data) : data.property(_model->propertyName(_index)));
}

void DataTableProperty::setValue(int index, const QVariant &value)
{
    Data *data = &_model->items[index];

    if (_setter)
        _setter(data, value);
}

}
