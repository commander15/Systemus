#ifndef SYSTEMUS_DATAINTERFACE_H
#define SYSTEMUS_DATAINTERFACE_H

#include <SystemusWidgets/global.h>
#include <SystemusWidgets/userinterface.h>

#include <QAbstractTableModel>

class QTableView;
class QAbstractButton;

class QSqlError;

class QModelIndex;
class QItemSelection;

namespace Systemus {

namespace Ui {
class DataInterface;
}

namespace Orm {
class DataTableModel;
}

class FilterWidget;

class DataEditDialog;

class Data;

class DataInterfacePrivate;
class SYSTEMUS_WIDGETS_EXPORT DataInterface : public UserInterface
{
    Q_OBJECT

public:
    enum InterfaceAction {
        SearchAction,
        ToggleFiltersAction,
        RefreshAction,
        ShowAction,
        EditAction,
        AddAction,
        DeleteAction,
        UserAction = 255
    };

    explicit DataInterface(QWidget *parent = nullptr);
    virtual ~DataInterface();

    Q_SLOT bool search();
    Q_SLOT bool search(const QString &query);
    bool searchWithLabel(const QString &query, const QString &label);

    Q_SLOT bool refresh();

    Q_SLOT void showCurrentData();
    virtual void showData(const Data &data);

    Q_SLOT bool editCurrentData();
    virtual bool editData(const Data &data);

    Q_SLOT QVariant addNewData();
    virtual QVariant addData(const Data &data);

    Q_SLOT int deleteSelectedData();
    virtual int deleteData(const QList<Data> &data);

    QTableView *tableView() const;
    QMenu *contextMenu() const;

    FilterWidget *filterWidget() const;
    void setFilterWidget(FilterWidget *widget);

    DataEditDialog *editDialog() const;
    void setEditDialog(DataEditDialog *dialog);

    Orm::DataTableModel *model() const;
    void setModel(Orm::DataTableModel *model);

    bool supportAction(int action) const override;
    bool canHandleAction(int action) const override;

protected:
    void initUi() override;
    void cleanupUi() override;
    void translateUi(bool full = true) override;

    virtual void showErrorMessage(const QString &title, const QString &text, const QSqlError &error);

    virtual bool showDataContextMenu(const QList<Data> &data, QMenu *menu);
    void contextMenuEvent(QContextMenuEvent *event) override;

    QVariant processAction(int action, const QVariantList &data) override;

    QAbstractButton *addButton(const QIcon &icon, const QString &text);
    void addButton(QAbstractButton *button);

    QAction *showAction() const;
    QAction *editAction() const;
    QAction *deleteAction() const;

    virtual QString searchFilter(const QString &query) const;
    virtual QString userFilter() const;
    virtual QString extraFilter() const;

private:
    Q_SLOT void configureTableHeaders();
    Q_SLOT void processSelectionChange(const QItemSelection &current, const QItemSelection &last);

    Ui::DataInterface *ui;
};

}

#endif // SYSTEMUS_DATAINTERFACE_H
