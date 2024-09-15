#ifndef SYSTEMUS_ACTIONWIDGET_H
#define SYSTEMUS_ACTIONWIDGET_H

#include <SystemusWidgets/global.h>
#include <SystemusWidgets/interfaceholder.h>

#include <QtWidgets/qwidget.h>

class QBoxLayout;
class QAbstractButton;

namespace Systemus {

class SYSTEMUS_WIDGETS_EXPORT ActionWidget : public QWidget, public SimpleInterfaceHolder
{
    Q_OBJECT

public:
    explicit ActionWidget(QWidget *parent = nullptr);
    ActionWidget(Qt::Orientation orientation, QWidget *parent = nullptr);
    ~ActionWidget();

protected:
    void processInterfaceChange(UserInterface *from, UserInterface *to) override;

private:
    Q_SLOT void updateUi(int action, bool supported);
    void retranslateUi();

    Q_SLOT void refreshData();
    Q_SLOT void addData();
    Q_SLOT void editData();
    Q_SLOT void deleteData();
    Q_SLOT void printData();

    QBoxLayout *_layout;
    QAbstractButton *_refreshButton;
    QAbstractButton *_addButton;
    QAbstractButton *_editButton;
    QAbstractButton *_deleteButton;
    QAbstractButton *_printButton;
};

}

#endif // SYSTEMUS_ACTIONWIDGET_H
