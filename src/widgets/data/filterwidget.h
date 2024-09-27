#ifndef FILTERWIDGET_H
#define FILTERWIDGET_H

#include <SystemusWidgets/global.h>

#include <SystemusCore/secretproperty.h>

#include <QtWidgets/qwidget.h>

namespace Systemus {

class SYSTEMUS_WIDGETS_EXPORT FilterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FilterWidget(QWidget *parent = nullptr);
    virtual ~FilterWidget();

    QVariant parameter(const QString &name) const;
    void setParameter(const QString &name, const QVariant &value);
    void setParameter(const QString &name, const QVariant &value1, const QVariant &value2);

    virtual QString parameterName(int index) const = 0;
    virtual QString parameterProperty(int index) const;

    virtual QVariantList parameterValues(int index) const = 0;
    virtual void setParameterValues(int index, const QVariantList &values) = 0;
    virtual void clearParameterValues(int index);

    virtual QString parameterFilter(int index) const;

    virtual int parameterIndex(const QString &parameter) const;
    virtual int parameterCount() const = 0;

    QString filter() const;
};

}

#endif // FILTERWIDGET_H
