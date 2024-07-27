#ifndef SYSTEMUS_TABLEVIEW_H
#define SYSTEMUS_TABLEVIEW_H

#include <SystemusWidgets/global.h>

#include <SystemusCore/data.h>

#include <QtWidgets/qtableview.h>

namespace Systemus {

class AbstractDataModel;

class SYSTEMUS_WIDGETS_EXPORT TableView : public QTableView
{
    Q_OBJECT

public:
    explicit TableView(QWidget *parent = nullptr);
    ~TableView();

    template<typename T>
    T currentData() const
    { return currentData().to<T>(); }
    Data currentData() const;

    template<typename T>
    QList<T> selectedData() const
    {
        QList<T> data;
        for (const Data &d : selectedData())
            data.append(d.to<T>());
        return data;
    }
    QList<Data> selectedData() const;

    template<typename T>
    T dataAt(const QPoint &pos) const
    { return dataAt(pos).to<T>(); }
    Data dataAt(const QPoint &pos) const;

    Q_SIGNAL void dataDoubleClicked(const Data &data);

    AbstractDataModel *model() const;
    void setModel(AbstractDataModel *model);

private:
    Q_SLOT void configureHeaders();

    Q_SLOT void processDoubleClick(const QModelIndex &index);
};

}

#endif // SYSTEMUS_TABLEVIEW_H
