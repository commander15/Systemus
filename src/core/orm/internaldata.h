#ifndef SYSTEMUS_INTERNALDATA_H
#define SYSTEMUS_INTERNALDATA_H

#include <SystemusCore/global.h>
#include <SystemusCore/data.h>

namespace Systemus {

class InternalDataPrivate;
class SYSTEMUS_CORE_EXPORT InternalData : public Systemus::Orm::Data
{
    Q_GADGET
    Q_PROPERTY(int id READ id WRITE setId)

public:
    InternalData();
    InternalData(const InternalData &other);
    ~InternalData();

    int id() const;
    void setId(int id);

    QString defaultFilter() const;

protected:
    InternalData(InternalDataPrivate *d);
};

}

#endif // SYSTEMUS_INTERNALDATA_H
