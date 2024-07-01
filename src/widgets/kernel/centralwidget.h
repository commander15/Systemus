#ifndef SYSTEMUS_CENTRALWIDGET_H
#define SYSTEMUS_CENTRALWIDGET_H

#include <SystemusWidgets/global.h>
#include <SystemusWidgets/interfaceholder.h>

#include <QtWidgets/qwidget.h>

namespace Systemus {

namespace Ui {
class CentralWidget;
}

class UserInterface;

class SYSTEMUS_WIDGETS_EXPORT CentralWidget : public QWidget, public InterfaceHolder
{
    Q_OBJECT

public:
    explicit CentralWidget(QWidget *parent = nullptr);
    ~CentralWidget();

private:
    Ui::CentralWidget *ui;
};

}

#endif // SYSTEMUS_CENTRALWIDGET_H
