#ifndef SYSTEMUS_DATAWINDOW_P_H
#define SYSTEMUS_DATAWINDOW_P_H

#include "datawindow.h"

namespace Systemus {

class DataWindowPrivate
{
public:
    DataWindowPrivate(DataWindow *q);
    virtual ~DataWindowPrivate();

    DataWindow *q_ptr;
    DataEdit *edit;
};

}

#endif // SYSTEMUS_DATAWINDOW_P_H
