#include "secretproperty.h"
#include "secretproperty_p.h"

#include <QtCore/qmetaobject.h>
#include <QtCore/qmetatype.h>

namespace Systemus {
namespace Orm {

SecretProperty::SecretProperty() :
    d_ptr(new SecretPropertyPrivate())
{
}

SecretProperty::SecretProperty(const QMetaProperty &property) :
    d_ptr(new MetaSecretPropertyPrivate(property))
{
}

SecretProperty::SecretProperty(const SecretProperty &other) :
    d_ptr(other.d_ptr)
{
}

SecretProperty::SecretProperty(SecretPropertyData *d) :
    d_ptr(d)
{
}

SecretProperty::~SecretProperty()
{
}

const char *SecretProperty::name() const
{
    return d_ptr->name();
}

QMetaType SecretProperty::metaType() const
{
    return QMetaType(d_ptr->metaType());
}

bool SecretProperty::isRequired() const
{
    return d_ptr->isRequired();
}

SecretPropertyData::~SecretPropertyData()
{
}

}
}
