#ifndef SAMPLE_H
#define SAMPLE_H

#include <SystemusCore/data.h>

class BaseData : public Systemus::Orm::Data
{
    Q_GADGET
    Q_PROPERTY(int id MEMBER id)

public:
    int id = 0;
};

class SimpleSample : public BaseData
{
    Q_GADGET
    Q_PROPERTY(QString name MEMBER name USER true)
    Q_PROPERTY(int index MEMBER index STORED false)
    Q_CLASSINFO("properties", "secretLevel(int, required)")

public:
    QString name;

    int index = 1;
};

#endif // SAMPLE_H
