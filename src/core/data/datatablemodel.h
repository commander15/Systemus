#ifndef SYSTEMUS_DATATABLEMODEL_H
#define SYSTEMUS_DATATABLEMODEL_H

#include <SystemusCore/global.h>
#include <SystemusCore/abstractdatamodel.h>

namespace Systemus {

class DataTableModelPrivate;
class SYSTEMUS_CORE_EXPORT DataTableModel : public AbstractDataModel
{
    Q_OBJECT

public:
    explicit DataTableModel(QObject *parent = nullptr);
    ~DataTableModel();

    QStringList propertyNames() const;
    void setProperties(const QStringList &names);
    bool addProperty(const QString &name);
    bool addProperty(const QString &name, std::function<QVariant (const Data &)> getter, std::function<void(Data *, const QVariant &)> setter = nullptr);
    template<typename T>
    bool addProperty(const QString &name, std::function<QVariant (const T &)> getter, std::function<void(T *, const QVariant &)> setter = nullptr);

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

protected:
    bool isCustomized() const override;
};

template<typename T>
bool DataTableModel::addProperty(const QString &name, std::function<QVariant (const T &)> getter, std::function<void (T *, const QVariant &)> setter)
{
    return addProperty(name,
        [&getter](const Data &data) {
            return getter(data.to<T>());
        },
        [&setter](Data *data, const QVariant &value) {
            T d = data->to<T>();
            setter(&d, value);
            *data = d;
        });
}

}

#endif // SYSTEMUS_DATATABLEMODEL_H
