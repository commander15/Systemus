#ifndef SYSTEMUS_DATA_P_H
#define SYSTEMUS_DATA_P_H

#include "data.h"

namespace Systemus {

class DataPrivate : public DataContent
{
public:
    DataPrivate() = default;
    DataPrivate(Orm::Data *data);
    DataPrivate(const DataPrivate &other);
    DataPrivate(const Orm::Data &data);
    ~DataPrivate();

    void changeData(const Orm::Data &other);

    static Orm::Data *copy(const Orm::Data *other);

    QVariantHash properties;
};

}

#endif // SYSTEMUS_DATA_P_H
