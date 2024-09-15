#ifndef SYSTEMUS_FILTERITEMWIDGET_H
#define SYSTEMUS_FILTERITEMWIDGET_H

#include <SystemusWidgets/global.h>

#include <QtWidgets/qwidget.h>

class QCheckBox;

namespace Systemus {

class SYSTEMUS_WIDGETS_EXPORT FilterItemWidget : public QWidget
{
    Q_OBJECT

public:
    FilterItemWidget(QWidget *parent = nullptr);
    virtual ~FilterItemWidget();

    QString title() const;
    Q_SLOT void setTitle(const QString &title);
    Q_SIGNAL void titleChanged(const QString &title);

    QString property() const;
    Q_SLOT void setProperty(const QString &property);
    Q_SIGNAL void propertyChanged(const QString &property);

    QVariant value() const;
    Q_SLOT void setValue(const QVariant &value);
    Q_SIGNAL void valueChanged();

    QVariant minimum() const;
    void setMinimum(const QVariant &val);

    QVariant maximum() const;
    void setMaximum(const QVariant &val);

    QPair<QVariant, QVariant> range() const;
    Q_SLOT void setRange(const QVariant &min, const QVariant &max);
    Q_SIGNAL void rangeChanged(const QVariant &min, const QVariant &max);

    bool isActive() const;
    void setActive(bool active);

    QString filter() const;

    static FilterItemWidget *createItem(int type, const QVariantHash &options, QWidget *parent = nullptr);

protected:
    virtual QVariant readValue() const = 0;
    virtual void writeValue(const QVariant &value) = 0;

    virtual QVariant readMinimum() const = 0;
    virtual void writeMinimum(const QVariant &value) = 0;

    virtual QVariant readMaximum() const = 0;
    virtual void writeMaximum(const QVariant &value) = 0;

    virtual QString generateFilter() const = 0;

private:
    QCheckBox *m_title;
    QString m_property;
};

class SYSTEMUS_WIDGETS_EXPORT DefaultFilterItemWidget : public FilterItemWidget
{
    Q_OBJECT

public:
    DefaultFilterItemWidget(const QString &name, const QVariantHash &options, QWidget *parent = nullptr);
    DefaultFilterItemWidget(std::function<QVariant(int)> getter, std::function<void(int, QVariant)> setter, std::function<QString()> generator, const QVariantHash &options, QWidget *parent = nullptr);

protected:
    QVariant readValue() const override;
    void writeValue(const QVariant &value) override;

    QVariant readMinimum() const override;
    void writeMinimum(const QVariant &value) override;

    QVariant readMaximum() const override;
    void writeMaximum(const QVariant &value) override;

    QString generateFilter() const override;

private:
    S_DECLARE_PRIVATE(DefaultFilterItemWidget)

    std::function<QVariant(int)> m_getter;
    std::function<void(int, QVariant)> m_setter;
    std::function<QString()> m_generator;
};

}
#endif // SYSTEMUS_FILTERITEMWIDGET_H
