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
    Data data(extractFields());
    data.setId(d->dataId);
    return data;
}

void DataEdit::setData(const Data &data)
{
    S_D(DataEdit);

    if (true) {
        d->dataId = data.id();
        fillFields(data);
        emit dataChanged();
    }
}

void DataEdit::clear()
{
    S_D(DataEdit);
    d->dataId = 0;
    clearFields();

    emit dataChanged();
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

Data DataEdit::extractFields() const
{
    return Data();
}

void DataEdit::fillFields(const Data &data)
{
    Q_UNUSED(data);
}

void DataEdit::clearFields()
{
}

void DataEdit::makeFieldsWritable(bool writable)
{
    Q_UNUSED(writable);
}

DataEditDialog::DataEditDialog(QWidget *parent) :
    DataEditDialog(new DefaultDataEditDialogPrivate(this), parent)
{
}

DataEditDialog::DataEditDialog(DataEdit *edit, QWidget *parent) :
    DataEditDialog(new ForwardedDataEditDialogPrivate(edit, this), parent)
{
    setWindowTitle(edit->windowTitle());
    connect(edit, &QWidget::windowTitleChanged, this, &QWidget::setWindowTitle);

    setFocusProxy(edit);

    const QSize size = ui->verticalLayout->minimumSize();
    setMinimumSize(size);
    setMaximumSize(size);
}

DataEditDialog::DataEditDialog(DataEditDialogPrivate *d, QWidget *parent) :
    QDialog(parent),
    d(d),
    ui(new Ui::DataEditDialog)
{
    ui->setupUi(this);
    contentWidget = ui->widget;
    d->init();
}

DataEditDialog::~DataEditDialog()
{
    delete ui;
}

Data DataEditDialog::data() const
{
    return d->data();
}

void DataEditDialog::setData(const Data &data)
{
    d->setData(data);
}

void DataEditDialog::clear()
{
    d->clear();
}

bool DataEditDialog::isReadOnly() const
{
    return d->isReadOnly();
}

void DataEditDialog::setReadOnly(bool r)
{
    d->setReadOnly(r);

    if (!r)
        ui->buttonBox->setStandardButtons(QDialogButtonBox::Save|QDialogButtonBox::Cancel);
    else
        ui->buttonBox->setStandardButtons(QDialogButtonBox::Close);
}

int DataEditDialog::exec()
{
    setFocus();
    return QDialog::exec();
}

DataEditDialog *DataEditDialog::fromEdit(DataEdit *edit, QWidget *parent)
{
    return new DataEditDialog(edit, parent);
}

Data DataEditDialog::extractFields() const
{
    return Data();
}

void DataEditDialog::fillFields(const Data &data)
{
    Q_UNUSED(data);
}

void DataEditDialog::clearFields()
{
}

void DataEditDialog::makeFieldsWritable(bool writable)
{
    Q_UNUSED(writable);
}

Systemus::DataEditPrivate::DataEditPrivate(DataEdit *q) :
    q_ptr(q),
    dataId(0),
    readOnly(false)
{
}

Systemus::DataEditPrivate::~DataEditPrivate()
{
}

DataEditDialogPrivate::DataEditDialogPrivate(DataEditDialog *q) :
    q(q)
{
}

DataEditDialogPrivate::~DataEditDialogPrivate()
{
}

void DataEditDialogPrivate::init()
{
}

DefaultDataEditDialogPrivate::DefaultDataEditDialogPrivate(DataEditDialog *q) :
    DataEditDialogPrivate(q),
    dataId(0)
{
}

Data DefaultDataEditDialogPrivate::data() const
{
    Data data(q->extractFields(), false);
    data.setId(dataId);
    return data;
}

void DefaultDataEditDialogPrivate::setData(const Data &data)
{
    dataId = data.id();
    q->fillFields(data);
}

void DefaultDataEditDialogPrivate::clear()
{
    q->clearFields();
}

bool DefaultDataEditDialogPrivate::isReadOnly() const
{
    return readOnly;
}

void DefaultDataEditDialogPrivate::setReadOnly(bool r)
{
    if (readOnly != r) {
        q->makeFieldsWritable(!r);
        readOnly = r;
    }
}

ForwardedDataEditDialogPrivate::ForwardedDataEditDialogPrivate(DataEdit *edit, DataEditDialog *q) :
    DataEditDialogPrivate(q),
    edit(edit)
{
}

void ForwardedDataEditDialogPrivate::init()
{
    QLayout *layout;

    layout = edit->layout();
    if (layout)
        layout->setContentsMargins(0, 0, 0, 0);

    layout = new QHBoxLayout(q->contentWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(edit);
}

Data ForwardedDataEditDialogPrivate::data() const
{
    return edit->data();
}

void ForwardedDataEditDialogPrivate::setData(const Data &data)
{
    edit->setData(data);
}

void ForwardedDataEditDialogPrivate::clear()
{
    edit->clear();
}

bool ForwardedDataEditDialogPrivate::isReadOnly() const
{
    return edit->isReadOnly();
}

void ForwardedDataEditDialogPrivate::setReadOnly(bool r)
{
    edit->setReadOnly(r);
}

}
