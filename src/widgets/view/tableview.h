#ifndef SYSTEMUS_TABLEVIEW_H
#define SYSTEMUS_TABLEVIEW_H

#include <SystemusWidgets/global.h>

#include <SystemusCore/data.h>

#include <QtWidgets/qtableview.h>

namespace Systemus {

namespace Orm {
class DataTableModel;
}

class SYSTEMUS_WIDGETS_EXPORT TableView : public QTableView
{
    Q_OBJECT

public:
    explicit TableView(QWidget *parent = nullptr);
    ~TableView();

    template<typename T> T currentData() const;
    Data currentData() const;

    template<typename T> QList<T> selectedData() const;
    QList<Data> selectedData() const;

    QList<int> selectedRows() const;

    template<typename T> T dataAt(const QPoint &pos) const;
    Data dataAt(const QPoint &pos) const;

    Q_SIGNAL void dataDoubleClicked(const Data &data);

    Orm::DataTableModel *model() const;
    void setModel(QAbstractItemModel *model);

private:
    Q_SLOT void processDoubleClick(const QModelIndex &index);
};

template<typename T>
T TableView::currentData() const
{ return currentData().to<T>(); }

template<typename T>
QList<T> TableView::selectedData() const
{
    QList<T> data;
    for (const Data &d : selectedData())
        data.append(d.to<T>());
    return data;
}

template<typename T>
T TableView::dataAt(const QPoint &pos) const
{ return dataAt(pos).to<T>(); }

}

#endif // SYSTEMUS_TABLEVIEW_H
