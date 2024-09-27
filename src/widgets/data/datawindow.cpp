#include "datawindow.h"
#include "datawindow_p.h"

#include <SystemusWidgets/dataedit.h>

#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qtabwidget.h>

namespace Systemus {

DataWindow::DataWindow(QWidget *parent) :
    DataWindow(DefaultWindow, parent)
{
}

DataWindow::DataWindow(int type, QWidget *parent) :
    DataWindow(DataWindowPrivate::create(type, this), parent)
{
}

DataWindow::DataWindow(DataWindowPrivate *d, QWidget *parent) :
    UserInterface(parent),
    d_ptr(d)
{
}

DataWindow::~DataWindow()
{
}

Data DataWindow::data() const
{
    return (d_ptr->edit ? d_ptr->edit->data() : Data());
}

void DataWindow::setData(const Data &data)
{
    if (d_ptr->edit)
        d_ptr->edit->setData(data);
}

DataEdit *DataWindow::edit() const
{
    return d_ptr->edit;
}

int DataWindow::windowType() const
{
    return d_ptr->windowType();
}

void DataWindow::initUi(DataEdit *edit)
{
    UserInterface::initUi();
    d_ptr->init(edit);
}

DataWindowPrivate::DataWindowPrivate(int type, DataWindow *q) :
    q_ptr(q),
    edit(nullptr)
{
}

DataWindowPrivate::~DataWindowPrivate()
{
}

void DataWindowPrivate::init(DataEdit *edit)
{
    if (!this->edit) {
        QWidget *container = createContainer(q_ptr);
        if (container) {
            QBoxLayout *l = new QVBoxLayout(q_ptr);
            l->addWidget(container);
        }

        UserInterface *interface = new UserInterface(q_ptr);
        interface->setInterfaceIcon(q_ptr->interfaceIcon());
        interface->setInterfaceTitle(q_ptr->tr("Informations"));

        QBoxLayout *layout = new QVBoxLayout(interface);
        layout->addWidget(edit);

        addInterface(interface);

        this->edit = edit;
    }
}

DataWindowPrivate *DataWindowPrivate::create(int type, DataWindow *q)
{
    switch (type) {
    case DataWindow::DefaultWindow:
        return new DefaultDataWindowPrivate(q);

    case DataWindow::TabWindow:
        return new DataTabWindowPrivate(q);

    default:
        return nullptr;
    }
}

DefaultDataWindowPrivate::DefaultDataWindowPrivate(DataWindow *q) :
    DataWindowPrivate(DataWindow::DefaultWindow, q),
    m_interface(nullptr)
{
}

DataTabWindowPrivate::DataTabWindowPrivate(DataWindow *q) :
    DataWindowPrivate(DataWindow::TabWindow, q),
    tab(nullptr)
{
}

QWidget *DataTabWindowPrivate::createContainer(QWidget *parent)
{
    tab = new QTabWidget(parent);
    return tab;
}

int DataTabWindowPrivate::currentIndex() const
{
    return tab->currentIndex();
}

void DataTabWindowPrivate::setCurrentIndex(int index)
{
    tab->setCurrentIndex(index);
}

UserInterface *DataTabWindowPrivate::interface(int index) const
{
    return static_cast<UserInterface *>(tab->widget(index));
}

int DataTabWindowPrivate::count() const
{
    return tab->count();
}

int DataTabWindowPrivate::insertInterface(int index, UserInterface *interface)
{
    return tab->insertTab(index, interface, interface->interfaceIcon(), interface->interfaceTitle());
}

void DataTabWindowPrivate::removeInterface(UserInterface *interface)
{
    tab->removeTab(tab->indexOf(interface));
}

}
