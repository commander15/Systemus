#include "filterwidget.h"
#include "filterwidget_p.h"
#include "ui_filterwidget.h"

#include <SystemusWidgets/private/filterwidgetimpl_p.h>

#include <QtWidgets/qcheckbox.h>
#include <QtWidgets/qgroupbox.h>

#include <QtCore/qcoreapplication.h>

namespace Systemus {

FilterWidget::FilterWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FilterWidget),
    d(new FilterWidgetPrivate(this))
{
    ui->setupUi(this);

    connect(ui->searchInput, &QLineEdit::textChanged, this, &FilterWidget::searchQueryChanged);
}

FilterWidget::~FilterWidget()
{
    delete ui;
}

QString FilterWidget::searchQuery() const
{
    return ui->searchInput->text();
}

void FilterWidget::setSearchQuery(const QString &query)
{
    ui->searchInput->setText(query);
}

FilterWidgetItem *FilterWidget::item(int index) const
{
    return (d->items.size() > index ? d->items.at(index) : nullptr);
}

int FilterWidget::itemCount() const
{
    return d->items.size();
}

FilterWidgetItem *FilterWidget::addString(const QString &name, const QString &property)
{
    return addString(name, property, 30);
}

FilterWidgetItem *FilterWidget::addString(const QString &name, const QString &property, int maxLength)
{
    QLineEdit *edit = new QLineEdit(this);
    edit->setMaxLength(maxLength);
    new FilterWidgetItem(edit, this);
}

FilterWidgetItem *FilterWidget::addString(const QString &name, const QString &property, const QStringList &values)
{

}

FilterWidgetItem::FilterWidgetItem(QWidget *widget, FilterWidget *filter) :
    FilterWidgetItem(QList<QWidget *>() << widget, filter)
{
}

FilterWidgetItem::FilterWidgetItem(const QList<QWidget *> &widgets, FilterWidget *filter) :
    _title(new QCheckBox(filter)),
    _layout(nullptr),
    _widgets(widgets)
{
    QFormLayout *layout = filter->ui->formLayout;
    if (widgets.size() == 1) {
        layout->addRow(_title, widgets.at(0));
        _layout = layout;
    } else if (widgets.size() > 1) {
        layout->addRow(_title);

        QWidget *container = new QWidget(filter);
        {
            _layout = new QFormLayout(container);
            _layout->addRow(QCoreApplication::translate("FilterWidgetItem", "From"), widgets.at(0));
            _layout->addRow(QCoreApplication::translate("FilterWidgetItem", "To"), widgets.at(1));
            for (int i(2); i < widgets.size(); ++i)
                _layout->addRow(widgets.at(i));
            container->setLayout(_layout);
        }
        layout->addRow(container);
    }
}

FilterWidgetItem::~FilterWidgetItem()
{
}

QString FilterWidgetItem::title() const
{
    return _title->text();
}

void FilterWidgetItem::setTitle(const QString &title)
{
    _title->setText(title);
}

bool FilterWidgetItem::isActive() const
{
    return _title->isChecked();
}

void FilterWidgetItem::setActive(bool active)
{
    _title->setChecked(active);
}

QString FilterWidgetItem::propertyName() const
{
    return _propertyName;
}

void FilterWidgetItem::setPropertyName(const QString &name)
{
    _propertyName = name;
}

bool FilterWidgetItem::hasValue() const
{
    return value().isValid();
}

void FilterWidgetItem::translateItem()
{
}

}
