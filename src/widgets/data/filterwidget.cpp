#include "filterwidget.h"

#include <SystemusCore/namespace.h>

namespace Systemus {

FilterWidget::FilterWidget(QWidget *parent) :
    QWidget(parent)
{
}

FilterWidget::~FilterWidget()
{
}

QVariant FilterWidget::parameter(const QString &name) const
{
    return parameterValues(parameterIndex(name)).constFirst();
}

void FilterWidget::setParameter(const QString &name, const QVariant &value)
{
    setParameterValues(parameterIndex(name), { value });
}

void FilterWidget::setParameter(const QString &name, const QVariant &value1, const QVariant &value2)
{
    setParameterValues(parameterIndex(name), { value1, value2 });
}

QString FilterWidget::parameterProperty(int index) const
{
    return parameterName(index);
}

void FilterWidget::clearParameterValues(int index)
{
    setParameterValues(index, {});
}

QString FilterWidget::parameterFilter(int index) const
{
    const QVariantList values = parameterValues(index);
    QString expression;

    if (values.isEmpty()) {
        // Nothing to do
    } else if (values.size() == 1)
        expression.append(" = " + formatValue(values.constFirst()));
    else if (values.size() == 2) {
        const QVariant v1 = values.constFirst();
        const QVariant v2 = values.constLast();

        switch (v1.metaType().id()) {
        case QMetaType::QDate:
        case QMetaType::QTime:
            expression.append("BETWEEN " + formatValue(v1) + " AND " + formatValue(v2));
            break;

        default:
            expression.append("IN (" + formatValue(v1) + ", " + formatValue(v2) + ')');
            break;
        }
    } else {
        QStringList strings;
        strings.resize(values.size());
        std::transform(values.begin(), values.end(), strings.begin(), [](const QVariant &val) {
            return formatValue(val);
        });

        expression.append("IN (" + strings.join(", ") + ')');
    }

    if (!expression.isEmpty())
        return parameterProperty(index) + ' ' + expression;
    else
        return QString();
}

int FilterWidget::parameterIndex(const QString &parameter) const
{
    for (int i(0); i < parameterCount(); ++i)
        if (parameterName(i) == parameter)
            return i;
    return -1;
}

QString FilterWidget::filter() const
{
    QStringList filters;
    for (int i(0); i < parameterCount(); ++i) {
        const QString filter = parameterFilter(i);
        if (!filter.isEmpty())
            filters.append(filter);
    }
    return filters.join(" AND ");
}

}
