#ifndef SYSTEMUS_DATAWINDOWMANAGER_P_H
#define SYSTEMUS_DATAWINDOWMANAGER_P_H

#include "datawindowmanager.h"

#include <QtCore/qmap.h>
#include <QtCore/qvariant.h>

namespace Systemus {

class DataWindowManagerPrivate
{
public:
    DataWindowManagerPrivate(DataWindowManager *q);

    DataWindowManager *q_ptr;

    QMap<QVariant, DataWindow*> windows;
    int maxCount;
};

}

bool operator<(const QVariant &v1, const QVariant &v2)
{
    return QVariant::compare(v1, v2) == QPartialOrdering::Less;
}

#endif // SYSTEMUS_DATAWINDOWMANAGER_P_H
