#ifndef SYSTEMUS_TABLEVIEW_H
#define SYSTEMUS_TABLEVIEW_H

#include <SystemusWidgets/global.h>

#include <SystemusCore/data.h>

#include <QtWidgets/qtableview.h>

class QSqlRecord;
class QSqlQueryModel;

namespace Systemus {

class SYSTEMUS_WIDGETS_EXPORT TableView : public QTableView
{
    Q_OBJECT

public:
    explicit TableView(QWidget *parent = nullptr);
    ~TableView();

    template<typename T>
    T currentData() const
    { return Data::fromSqlRecord<T>(currentRecord()); }
    QSqlRecord currentRecord() const;

    template<typename T>
    QList<T> selectedData() const
    { return Data::fromSqlRecords<T>(selectedRecords()); }
    QList<QSqlRecord> selectedRecords() const;

    template<typename T>
    T dataAt(const QPoint &pos) const
    { return Data::fromSqlRecord<T>(recordAt(pos)); }
    QSqlRecord recordAt(const QPoint &pos) const;

    Q_SIGNAL void recordDoubleClicked(const QSqlRecord &record);

    QSqlQueryModel *model() const;
    void setModel(QSqlQueryModel *model);

protected:
    Q_SLOT virtual void prepare();

private:
    Q_SLOT void processDoubleClick(const QModelIndex &index);
};

}

#endif // SYSTEMUS_TABLEVIEW_H
