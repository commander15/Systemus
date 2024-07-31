#ifndef SYSTEMUS_STANDARDUSERINTERFACE_H
#define SYSTEMUS_STANDARDUSERINTERFACE_H

#include <SystemusWidgets/global.h>
#include <SystemusWidgets/userinterface.h>

class QMenu;

class QTextDocument;

class QPagedPaintDevice;
class QItemSelection;

namespace Systemus {

namespace Ui {
class StandardUserInterface;
}

class TableView;
class DataTableModel;
class DataEditDialog;
class Data;

class StandardUserInterfacePrivate;
class SYSTEMUS_WIDGETS_EXPORT StandardUserInterface : public UserInterface
{
    Q_OBJECT

public:
    enum InterfaceAction {
        SearchAction,
        RefreshAction,
        ShowAction,
        AddAction,
        EditAction,
        DeleteAction,
        PrintAction,
        UserAction = 255
    };

    StandardUserInterface(const QByteArray &id, QWidget *parent = nullptr);
    virtual ~StandardUserInterface();

    Q_SLOT void search();
    Q_SLOT bool refresh();
    Q_SLOT void showData();
    Q_SLOT void addData();
    Q_SLOT void editData();
    Q_SLOT void deleteData();
    Q_SLOT void printData();

    TableView *tableView() const;

    DataTableModel *dataModel() const;
    void setDataModel(const QByteArray &className);
    void setDataModel(DataTableModel *model);

    QMenu *contextMenu() const;

    DataEditDialog *dataEditDialog() const;
    void setDataEditDialog(DataEditDialog *dialog);

    QAction *printAction() const;

    bool canHandleAction(int action) const override;
    bool supportAction(int action) const override;

    InterfaceType interfaceType() const override
    { return StandardInterface; }

protected:
    virtual bool showDataContextMenu(const QList<Data> &data, QMenu *menu);

    virtual QPageLayout documentPageLayout() const;
    virtual void fillDocumentForPrinting(QTextDocument *document, const Data &data);

    void initUi() override;
    void translateUi() override;
    QVariant processAction(int action, const QVariantList &data) override;

    void contextMenuEvent(QContextMenuEvent *event) override;

private:
    Q_SLOT void configureTableHeaders();
    Q_SLOT void processSelectionChange(const QItemSelection &current, const QItemSelection &last);

    Ui::StandardUserInterface *ui;

    friend class StandardUserInterfacePrivate;
};

}

#endif // SYSTEMUS_STANDARDUSERINTERFACE_H
