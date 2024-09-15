#ifndef DIRECTION_H
#define DIRECTION_H

#include "data.h"

class Direction : public Data
{
    Q_GADGET
    Q_PROPERTY(QString label MEMBER label)
    Q_CLASSINFO("properties", "parentId(int)")
    Q_CLASSINFO("table", "ADirections")

public:
    QString label;
    OneToOneRelation<Direction> parent;

    Direction &operator=(const Direction &other);

    bool postGet() override;

    QJsonObject toJsonObject() const override;

private:
    S_DATA_INIT(Direction)
    S_DATA_METHODS(Direction)
};

#endif // DIRECTION_H
