#include "interfacestack.h"
#include "ui_interfacestack.h"

#include <SystemusWidgets/userinterface.h>

namespace Systemus {

InterfaceStack::InterfaceStack(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InterfaceStack)
{
    ui->setupUi(this);

    connect(ui->stackedWidget, &QStackedWidget::currentChanged, this, &InterfaceStack::currentIndexChanged);
}

InterfaceStack::~InterfaceStack()
{
    delete ui;
}

UserInterface *InterfaceStack::currentInterface() const
{
    return static_cast<UserInterface *>(ui->stackedWidget->currentWidget());
}

int InterfaceStack::currentIndex() const
{
    return ui->stackedWidget->currentIndex();
}

void InterfaceStack::setCurrentIndex(int index)
{
    ui->stackedWidget->setCurrentIndex(index);
}

UserInterface *InterfaceStack::interface(int index) const
{
    return static_cast<UserInterface *>(ui->stackedWidget->widget(index));
}

int InterfaceStack::count() const
{
    return ui->stackedWidget->count();
}

int InterfaceStack::addInterface(UserInterface *interface)
{
    connect(interface->interfaceAction(), &QAction::triggered, this, &InterfaceStack::showInterface);
    return ui->stackedWidget->addWidget(interface);
}

int InterfaceStack::insertInterface(int index, UserInterface *interface)
{
    connect(interface->interfaceAction(), &QAction::triggered, this, &InterfaceStack::showInterface);
    return ui->stackedWidget->insertWidget(index, interface);
}

void InterfaceStack::removeInterface(UserInterface *interface)
{
    disconnect(interface->interfaceAction(), &QAction::triggered, this, &InterfaceStack::showInterface);
    ui->stackedWidget->removeWidget(interface);
}

int InterfaceStack::indexOf(UserInterface *interface) const
{
    return ui->stackedWidget->indexOf(interface);
}

void InterfaceStack::showInterface()
{
    QAction *action = qobject_cast<QAction *>(sender());
    for (int i = 0; i < count(); ++i) {
        if (interface(i)->interfaceTitle() == action->text()) {
            setCurrentIndex(i);
            break;
        }
    }
}

}
