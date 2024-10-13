#ifndef SYSTEMUS_INTERNALDATA_P_H
#define SYSTEMUS_INTERNALDATA_P_H

#include "internaldata.h"

#include <SystemusCore/private/ormdata_p.h>

namespace Systemus {

class InternalDataPrivate : public Orm::DataPrivate
{
public:
    int id = 0;
};

class DescriptiveDataPrivate : public InternalDataPrivate
{
public:
    QString name;
    QString description;

    QDate creationDate;
    QTime creationTime;
};

}

#endif // SYSTEMUS_INTERNALDATA_P_H
