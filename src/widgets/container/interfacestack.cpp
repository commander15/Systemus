#include "interfacestack.h"
#include "ui_interfacestack.h"

#include <SystemusWidgets/userinterface.h>

#include <SystemusCore/system.h>

namespace Systemus {

InterfaceStack::InterfaceStack(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InterfaceStack)
{
    ui->setupUi(this);

    connect(ui->stackedWidget, &QStackedWidget::currentChanged, this, &InterfaceStack::processWidgetChange);

    const QImage iconImage = QImage::fromData(System::instance()->logoData());
    if (!iconImage.isNull())
        ui->systemLogoOutput->setPixmap(QPixmap::fromImage(iconImage));
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
    return ui->stackedWidget->currentIndex() + 1;
}

void InterfaceStack::setCurrentIndex(int index)
{
    ui->stackedWidget->setCurrentIndex(index + 1);
}

UserInterface *InterfaceStack::interface(int index) const
{
    return static_cast<UserInterface *>(ui->stackedWidget->widget(index + 1));
}

int InterfaceStack::count() const
{
    return ui->stackedWidget->count() - 1;
}

int InterfaceStack::addInterface(UserInterface *interface)
{
    connect(interface->interfaceAction(), &QAction::triggered, this, &InterfaceStack::showInterface);
    return ui->stackedWidget->addWidget(interface);
}

int InterfaceStack::insertInterface(int index, UserInterface *interface)
{
    connect(interface->interfaceAction(), &QAction::triggered, this, &InterfaceStack::showInterface);
    return ui->stackedWidget->insertWidget(index + 1, interface);
}

void InterfaceStack::removeInterface(UserInterface *interface)
{
    disconnect(interface->interfaceAction(), &QAction::triggered, this, &InterfaceStack::showInterface);
    ui->stackedWidget->removeWidget(interface);
}

int InterfaceStack::indexOf(UserInterface *interface) const
{
    return ui->stackedWidget->indexOf(interface) - 1;
}

void InterfaceStack::showInterface()
{
    const QByteArray interfaceId = sender()->property("interfaceId").toByteArray();
    for (int i(0); i < ui->stackedWidget->count(); ++i) {
        if (ui->stackedWidget->widget(i)->property("interfaceId") == interfaceId) {
            ui->stackedWidget->setCurrentIndex(i);
            break;
        }
    }
}

void InterfaceStack::processWidgetChange(int index)
{
    index -= 1;
    emit currentIndexChanged(index);
}

void InterfaceStack::translateUi()
{
    //
}

}
