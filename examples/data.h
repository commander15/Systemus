#ifndef DATA_H
#define DATA_H

#include <SystemusCore/data.h>
#include <SystemusCore/datarelation.h>

class Data : public Systemus::Orm::Data
{
    Q_GADGET
    Q_PROPERTY(int id MEMBER id)

public:
    int id;

protected:
    bool isPropertyNull(const QString &name) const override;
    QVariant readProperty(const QString &name) const override;
    bool writeProperty(const QString &name, const QVariant &value) override;

private:
    S_DATA(Data)
    S_DATA_BASE
};

template<typename T>
using OneToOneRelation = Systemus::Orm::OneToOneRelation<T>;

template<typename T>
using OneToManyRelation = Systemus::Orm::OneToManyRelation<T>;

template<typename T>
using ManyToOneRelation = Systemus::Orm::ManyToManyRelation<T>;

template<typename T>
using ManyToManyRelation = Systemus::Orm::ManyToManyRelation<T>;

#endif // DATA_H
