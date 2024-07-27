#ifndef SYSTEMUS_FILTERWIDGETIMPL_P_H
#define SYSTEMUS_FILTERWIDGETIMPL_P_H

#include <SystemusWidgets/filterwidget.h>

namespace Systemus {

class FilterWidgetUtils
{
public:
    static FilterWidgetItem *createBoolItem();

    typedef QVariant(ValueGetter)(const QWidget *);
    typedef void(ValueSetter)(QWidget *, const QVariant &);

    typedef QVariantPair(RangeGetter)(const QWidget *);
    typedef void(RangeSetter)(QWidget *, const QVariant &, const QVariant &);

    typedef void(Translator)(const QList<QWidget *> &);
};

class FilterWidgetImplPrivate : public FilterWidgetItem
{
public:
    FilterWidgetImplPrivate( int type, FilterWidget *filter);
    ~FilterWidgetImplPrivate();

    bool hasValue() const override;
    QVariant value() const override;
    void setValue(const QVariant &value) override;

    bool hasRange() const override;
    QVariantPair range() const override;
    void setRange(const QVariantPair &range) override;

    int itemType() const override;

protected:
    void translateItem() override;

private:
    int _type;
};

}

#endif // FILTERWIDGETIMPL_P_H
