#include "data.h"

bool Data::isPropertyNull(const QString &name) const
{
    if (name == QStringLiteral("id"))
        return id <= 0;
    else
        return Systemus::Orm::Data::isPropertyNull(name);
}

QVariant Data::readProperty(const QString &name) const
{
    if (name == QStringLiteral("id"))
        return id;
    else
        return Systemus::Orm::Data::readProperty(name);
}

bool Data::writeProperty(const QString &name, const QVariant &value)
{
    if (name == QStringLiteral("id")) {
        id = value.toInt();
        return true;
    } else
        return Systemus::Orm::Data::writeProperty(name, value);
}
