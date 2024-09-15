#include "dataedit.h"
#include "dataedit_p.h"
#include "ui_dataeditdialog.h"

#include <SystemusCore/data.h>

namespace Systemus {

DataEdit::DataEdit(QWidget *parent) :
    DataEdit(new DataEditPrivate(this), parent)
{
}

DataEdit::DataEdit(DataEditPrivate *d, QWidget *parent) :
    QWidget(parent),
    d_ptr(d)
{
}

DataEdit::~DataEdit()
{
}

Data DataEdit::data() const
{
    S_D(const DataEdit);
    return fieldsData(d->data);
}

void DataEdit::setData(const Data &data)
{
    S_D(DataEdit);
    if (fillFields(data)) {
        d->data = data;
        emit dataChanged();
    }
}

void DataEdit::clear()
{
    S_D(DataEdit);
    if (clearFields()) {
        d->data.clear();
        emit dataChanged();
    }
}

bool DataEdit::isReadOnly() const
{
    S_D(const DataEdit);
    return d->readOnly;
}

void DataEdit::setReadOnly(bool r)
{
    S_D(DataEdit);
    if (d->readOnly != r) {
        makeFieldsWritable(!r);
        d->readOnly = r;
    }
}

Data DataEdit::fieldsData(const Data &data) const
{
    return data;
}

bool DataEdit::fillFields(const Data &data)
{
    Q_UNUSED(data);
    return true;
}

bool DataEdit::clearFields()
{
    return true;
}

void DataEdit::makeFieldsWritable(bool writable)
{
    Q_UNUSED(writable);
}

DataEditDialog::DataEditDialog(QWidget *parent) :
    DataEditDialog(new DataEditDialogPrivate(this, nullptr), parent)
{
}

DataEditDialog::DataEditDialog(DataEdit *edit, QWidget *parent) :
    DataEditDialog(new DataEditDialogPrivate(this, edit), parent)
{
    setWindowIcon(edit->windowIcon());
    connect(edit, &QWidget::windowIconChanged, this, &QWidget::setWindowIcon);

    setWindowTitle(edit->windowTitle());
    connect(edit, &QWidget::windowTitleChanged, this, &QWidget::setWindowTitle);
}

DataEditDialog::DataEditDialog(DataEditDialogPrivate *d, QWidget *parent) :
    QDialog(parent),
    d(d),
    ui(new Ui::DataEditDialog)
{
    ui->setupUi(this);

    if (d->editWidget) {
        ui->verticalLayout->insertWidget(0, d->editWidget, 1);
        contentWidget = d->editWidget;
    } else {
        contentWidget = new QWidget(this);
        ui->verticalLayout->insertWidget(0, contentWidget, 1);
    }
}

DataEditDialog::~DataEditDialog()
{
    delete ui;
}

Data DataEditDialog::data() const
{
    return fieldsData(d->edit->data);
}

void DataEditDialog::setData(const Data &data)
{
    if (fillFields(data)) {
        d->edit->data = data;
        emit dataChanged();
    }
}

void DataEditDialog::clear()
{
    if (clearFields()) {
        d->edit->data.clear();
        emit dataChanged();
    }
}

bool DataEditDialog::isReadOnly() const
{
    return d->edit->readOnly;
}

void DataEditDialog::setReadOnly(bool r)
{
    if (d->edit->readOnly == r)
        return;

    makeFieldsWritable(!r);

    if (!r)
        ui->buttonBox->setStandardButtons(QDialogButtonBox::Save|QDialogButtonBox::Cancel);
    else
        ui->buttonBox->setStandardButtons(QDialogButtonBox::Close);

    d->edit->readOnly = r;
}

int DataEditDialog::exec()
{
    setFocus();
    return QDialog::exec();
}

DataEdit *DataEditDialog::editWidget() const
{
    return d->editWidget;
}

DataEditDialog *DataEditDialog::fromEdit(DataEdit *edit, QWidget *parent)
{
    return new DataEditDialog(edit, parent);
}

Data DataEditDialog::fieldsData(const Data &data) const
{
    if (d->editWidget)
        return d->editWidget->fieldsData(data);
    else
        return data;
}

bool DataEditDialog::fillFields(const Data &data)
{
    if (d->editWidget)
        return d->editWidget->fillFields(data);
    else
        return false;
}

bool DataEditDialog::clearFields()
{
    if (d->editWidget)
        return d->editWidget->clearFields();
    else
        return false;
}

void DataEditDialog::makeFieldsWritable(bool writable)
{
    if (d->editWidget)
        d->editWidget->makeFieldsWritable(writable);
}

Systemus::DataEditPrivate::DataEditPrivate(DataEdit *q) :
    q_ptr(q),
    readOnly(false)
{
}

Systemus::DataEditPrivate::~DataEditPrivate()
{
}

DataEditDialogPrivate::DataEditDialogPrivate(DataEditDialog *q, DataEdit *editWidget) :
    q_ptr(q),
    editWidget(editWidget),
    edit(editWidget ? editWidget->d_ptr.get() : new DataEditPrivate(nullptr))
{
}

DataEditDialogPrivate::~DataEditDialogPrivate()
{
    if (edit && !editWidget)
        delete edit;
}

}
