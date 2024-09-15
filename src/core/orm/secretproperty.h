#ifndef SECRETPROPERTY_H
#define SECRETPROPERTY_H

#include <SystemusCore/global.h>

#include <QtCore/qshareddata.h>

class QMetaProperty;

namespace Systemus {
namespace Orm {

class SecretPropertyData;
class SYSTEMUS_CORE_EXPORT SecretProperty
{
public:
    SecretProperty();
    SecretProperty(const QMetaProperty &property);
    SecretProperty(const SecretProperty &other);
    ~SecretProperty();

    const char *name() const;
    QMetaType metaType() const;
    bool isRequired() const;

private:
    SecretProperty(SecretPropertyData *d);

    QExplicitlySharedDataPointer<SecretPropertyData> d_ptr;

    friend class Backend;
};

class SYSTEMUS_CORE_EXPORT SecretPropertyData : public QSharedData
{
public:
    virtual ~SecretPropertyData();

    virtual const char *name() const = 0;
    virtual int metaType() const = 0;
    virtual bool isRequired() const = 0;

    virtual const QMetaProperty *metaProperty() const = 0;
};

}
}

#endif // SECRETPROPERTY_H
