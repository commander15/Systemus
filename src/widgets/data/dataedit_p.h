#ifndef SYSTEMUS_DATAEDIT_P_H
#define SYSTEMUS_DATAEDIT_P_H

#include <SystemusWidgets/dataedit.h>

namespace Systemus {

class DataEditPrivate
{
public:
    DataEditPrivate(DataEdit *q);
    virtual ~DataEditPrivate();

    DataEdit *q_ptr;

    Data data;
    bool readOnly;
};

class DataEditDialogPrivate
{
public:
    DataEditDialogPrivate(DataEditDialog *q, DataEdit *editWidget);
    virtual ~DataEditDialogPrivate();

    DataEdit *editWidget;
    DataEditPrivate *edit;

    DataEditDialog *q_ptr;
};

}

#endif // SYSTEMUS_DATAEDIT_P_H
