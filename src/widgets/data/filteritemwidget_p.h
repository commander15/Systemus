#ifndef SYSTEMUS_FILTERITEMWIDGET_P_H
#define SYSTEMUS_FILTERITEMWIDGET_P_H

#include <SystemusWidgets/filteritemwidget.h>

namespace Systemus {

class DefaultFilterItemWidgetPrivate {
public:
    enum ItemProperty {
        ValueProperty,
        MinimumProperty,
        MaximumProperty,
        FilterProperty
    };

    static DefaultFilterItemWidget *createComboBoxItem(const QVariantHash &options, QWidget *parent);
};

}

#endif // SYSTEMUS_FILTERITEMWIDGET_P_H
