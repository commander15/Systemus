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

DescriptiveData::DescriptiveData() :
    InternalData(new DescriptiveDataPrivate)
{
}

QString DescriptiveData::name() const
{
    S_D(const DescriptiveData);
    return d->name;
}

void DescriptiveData::setName(const QString &name)
{
    S_D(DescriptiveData);
    d->name = name;
}

QString DescriptiveData::description() const
{
    S_D(const DescriptiveData);
    return d->description;
}

void DescriptiveData::setDescription(const QString &desc)
{
    S_D(DescriptiveData);
    d->description = desc;
}

QDate DescriptiveData::creationDate() const
{
    S_D(const DescriptiveData);
    return d->creationDate;
}

QTime DescriptiveData::creationTime() const
{
    S_D(const DescriptiveData);
    return d->creationTime;
}

bool DescriptiveData::writeProperty(const QString &name, const QVariant &value)
{
    S_D(DescriptiveData);

    if (name == "creationDate")
        d->creationDate = value.toDate();
    else if (name == "creationTime")
        d->creationTime = value.toTime();
    else
        return InternalData::writeProperty(name, value);

    return true;
}

}
