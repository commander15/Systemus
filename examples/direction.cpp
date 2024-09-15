#include "direction.h"

#include <QtCore/qjsonobject.h>

Direction &Direction::operator=(const Direction &other)
{
    Data::operator=(other);
    label = other.label;
    if (other.parent)
        this->parent = other.parent;
    return *this;
}

bool Direction::postGet()
{
    if (property("parentId").toInt() > 0)
        return parent.get(this, "parentId", "id");
    else
        return true;
}

QJsonObject Direction::toJsonObject() const
{
    QJsonObject object = Data::toJsonObject();
    if (parent)
        object.insert("parent", parent.toJson());
    //object.remove("parentId");
    return object;
}
