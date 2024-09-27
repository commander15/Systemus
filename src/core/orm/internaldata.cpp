#include "internaldata.h"
#include "internaldata_p.h"

namespace Systemus {

InternalData::InternalData() :
    Orm::Data(new InternalDataPrivate)
{
}

InternalData::InternalData(InternalDataPrivate *d) :
    Orm::Data(d)
{
}

InternalData::InternalData(const InternalData &other) :
    Orm::Data(other)
{
}

InternalData::~InternalData()
{
}

int InternalData::id() const
{
    S_D(const InternalData);
    return d->id;
}

void InternalData::setId(int id)
{
    S_D(InternalData);
    d->id = id;
}

QString InternalData::defaultFilter() const
{
    const auto table = metaTable();
    return table.foreignFieldName() + " = " + formatValue(primaryValue());
}

}
