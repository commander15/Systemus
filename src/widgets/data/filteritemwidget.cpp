#include "filteritemwidget.h"
#include "filteritemwidget_p.h"

#include <SystemusCore/namespace.h>

#include <QtWidgets/qcheckbox.h>
#include <QtWidgets/qcombobox.h>

#include <QtCore/qvariant.h>

namespace Systemus {

FilterItemWidget::FilterItemWidget(QWidget *parent) :
    QWidget(parent),
    m_title(new QCheckBox("Item", this))
{
}

FilterItemWidget::~FilterItemWidget()
{
}

QString FilterItemWidget::title() const
{
    return m_title->text();
}

void FilterItemWidget::setTitle(const QString &title)
{
    m_title->setText(title);
    emit titleChanged(title);
}

QString FilterItemWidget::property() const
{
    return m_property;
}

void FilterItemWidget::setProperty(const QString &property)
{
    if (m_property != property) {
        m_property = property;
        emit propertyChanged(property);
    }
}

QVariant FilterItemWidget::value() const
{
    return readValue();
}

void FilterItemWidget::setValue(const QVariant &value)
{
    writeValue(value);
}

QVariant FilterItemWidget::minimum() const
{
    return readMinimum();
}

void FilterItemWidget::setMinimum(const QVariant &val)
{
    writeMinimum(val);
}

QVariant FilterItemWidget::maximum() const
{
    return readMaximum();
}

void FilterItemWidget::setMaximum(const QVariant &val)
{
    writeMaximum(val);
}

QPair<QVariant, QVariant> FilterItemWidget::range() const
{
    return QPair<QVariant, QVariant>(readMinimum(), readMaximum());
}

void FilterItemWidget::setRange(const QVariant &min, const QVariant &max)
{
    writeMinimum(min);
    writeMaximum(max);
}

bool FilterItemWidget::isActive() const
{
    return m_title->isChecked();
}

void FilterItemWidget::setActive(bool active)
{
    m_title->setChecked(active);
}

QString FilterItemWidget::filter() const
{
    return (m_title->isChecked() ? generateFilter() : QString());
}

FilterItemWidget *FilterItemWidget::createItem(int type, const QVariantHash &options, QWidget *parent)
{
    switch (type) {
    case 1:
        return DefaultFilterItemWidgetPrivate::createComboBoxItem(options, parent);

    default:
        return nullptr;
    }
}

DefaultFilterItemWidget::DefaultFilterItemWidget(const QString &name, const QVariantHash &options, QWidget *parent) :
    FilterItemWidget(parent)
{
    if (name == QStringLiteral("QString")) {
    }
}

DefaultFilterItemWidget::DefaultFilterItemWidget(std::function<QVariant (int)> getter, std::function<void (int, QVariant)> setter, std::function<QString ()> generator, const QVariantHash &options, QWidget *parent) :
    FilterItemWidget(parent),
    m_getter(getter),
    m_setter(setter),
    m_generator(generator)
{
}

QVariant DefaultFilterItemWidget::readValue() const
{
    return (m_getter ? m_getter(DefaultFilterItemWidgetPrivate::ValueProperty) : QVariant());
}

void DefaultFilterItemWidget::writeValue(const QVariant &value)
{
    if (m_setter)
        m_setter(DefaultFilterItemWidgetPrivate::ValueProperty, value);
}

QVariant DefaultFilterItemWidget::readMinimum() const
{
    return (m_getter ? m_getter(DefaultFilterItemWidgetPrivate::MinimumProperty) : QVariant());
}

void DefaultFilterItemWidget::writeMinimum(const QVariant &value)
{
    if (m_setter)
        m_setter(DefaultFilterItemWidgetPrivate::MinimumProperty, value);
}

QVariant DefaultFilterItemWidget::readMaximum() const
{
    return (m_getter ? m_getter(DefaultFilterItemWidgetPrivate::MaximumProperty) : QVariant());
}

void DefaultFilterItemWidget::writeMaximum(const QVariant &value)
{
    if (m_setter)
        m_setter(DefaultFilterItemWidgetPrivate::MaximumProperty, value);
}

QString DefaultFilterItemWidget::generateFilter() const
{
    return (m_generator ? m_generator() : QString());
}

DefaultFilterItemWidget *Systemus::DefaultFilterItemWidgetPrivate::createComboBoxItem(const QVariantHash &options, QWidget *parent)
{
    std::function<QVariant(int)> getter;
    std::function<void(int, QVariant)> setter;
    std::function<QString()> generator;

    DefaultFilterItemWidget *item = new DefaultFilterItemWidget("", options, parent);

    QComboBox *box = new QComboBox(parent);

    const QStringList labels = options.value("labels").toStringList();
    const QVariantList values = options.value("values").toList();
    for (int i(0); i < labels.size(); ++i)
        box->addItem(labels.at(i), (i < values.size() ? values.at(i) : labels.at(i)));

    item->m_getter = [item, box](int property) {
        switch (property) {
        case ValueProperty:
            return box->currentData();

        case FilterProperty:
            return QVariant(item->property() + " = " + Systemus::formatValue(box->currentData()));

        default:
            return QVariant();
        }
    };

    return new DefaultFilterItemWidget(getter, nullptr, nullptr, options, parent);
}

}
