#ifndef SYSTEMUS_FILTERWIDGET_P_H
#define SYSTEMUS_FILTERWIDGET_P_H

#include <SystemusWidgets/filterwidget.h>

namespace Systemus {

class FilterWidgetPrivate
{
public:
    FilterWidgetPrivate(FilterWidget *q);

    FilterWidget *q;

    QList<FilterWidgetItem *> items;
};

}

#endif // SYSTEMUS_FILTERWIDGET_P_H
