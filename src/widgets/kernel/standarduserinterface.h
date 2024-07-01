#ifndef SYSTEMUS_STANDARDUSERINTERFACE_H
#define SYSTEMUS_STANDARDUSERINTERFACE_H

#include <SystemusWidgets/global.h>
#include <SystemusWidgets/userinterface.h>

class QMenu;

class QTextDocument;

class QSqlRecord;

class QPagedPaintDevice;

namespace Systemus {

namespace Ui {
class StandardUserInterface;
}

class TableView;
class DataModel;
class Data;

class StandardUserInterfacePrivate;
class SYSTEMUS_WIDGETS_EXPORT StandardUserInterface : public UserInterface
{
    Q_OBJECT

public:
    explicit StandardUserInterface(const QByteArray &id, QWidget *parent = nullptr);
    virtual ~StandardUserInterface();

    Q_SLOT void search();
    Q_SLOT bool refresh();
    Q_SLOT void showData();
    Q_SLOT void addData();
    Q_SLOT void editData();
    Q_SLOT void deleteData();
    Q_SLOT void printData();

    TableView *tableView() const;
    QMenu *contextMenu() const;
    DataModel *dataModel() const;

    QAction *printAction() const;

    bool supportAction(int action) const override;

    InterfaceType interfaceType() const override
    { return StandardInterface; }

protected:
    Q_SLOT virtual void showRecord(const QSqlRecord &record);
    virtual bool addRecord(const QSqlRecord &record);
    virtual bool editRecord(const QSqlRecord &record);
    virtual bool deleteRecords(const QList<QSqlRecord> &records);
    virtual void fillDocumentForPrinting(QTextDocument *document, const QSqlRecord &record);

    virtual void showRecordsContextMenu(const QList<QSqlRecord> &records, const QPoint &pos);

    void initUi() override;
    void translateUi() override;
    QVariant processAction(int action, const QVariant &data) override;

private:
    Q_SLOT void showContextMenu(const QPoint &pos);

    Ui::StandardUserInterface *ui;

    friend class StandardUserInterfacePrivate;
};

}

#endif // SYSTEMUS_STANDARDUSERINTERFACE_H
