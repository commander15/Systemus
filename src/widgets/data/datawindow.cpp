#include "datawindow.h"
#include "datawindow_p.h"

#include <SystemusWidgets/dataedit.h>

#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qtabwidget.h>

namespace Systemus {

DataWindow::DataWindow(QWidget *parent) :
    DataWindow(new DataWindowPrivate(this), parent)
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
    if (d_ptr->edit) {
        d_ptr->edit->setData(data);
        display(data);
    }
}

DataEdit *DataWindow::edit() const
{
    return d_ptr->edit;
}

QWidget *DataWindow::container() const
{
    return d_ptr->q_ptr;
}

int DataWindow::windowType() const
{
    return DefaultWindow;
}

void DataWindow::initWindow(DataEdit *edit)
{
    if (!d_ptr->edit)
        d_ptr->edit = edit;
}

void DataWindow::display(const Data &data)
{
    Q_UNUSED(data);
}

DataWindowPrivate::DataWindowPrivate(DataWindow *q) :
    q_ptr(q),
    edit(nullptr)
{
}

DataWindowPrivate::~DataWindowPrivate()
{
}

}
