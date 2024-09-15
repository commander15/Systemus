#ifndef SYSTEMUS_SECRETPROPERTY_P_H
#define SYSTEMUS_SECRETPROPERTY_P_H

#include "secretproperty.h"

#include <QtCore/qmetaobject.h>

namespace Systemus {
namespace Orm {

class SecretPropertyPrivate : public SecretPropertyData
{
public:
    const char *name() const override
    { return m_name; }

    int metaType() const override
    { return m_metaType; }

    bool isRequired() const override
    { return m_required; }

    const QMetaProperty *metaProperty() const override
    { return nullptr; }

    char *m_name = nullptr;
    int m_metaType = 0;
    bool m_required = false;
};

class MetaSecretPropertyPrivate : public SecretPropertyData
{
public:
    MetaSecretPropertyPrivate(const QMetaProperty &property) :
        m_property(property)
    {}

    MetaSecretPropertyPrivate(const MetaSecretPropertyPrivate &other) = default;

    const char *name() const override
    { return m_property.name(); }

    int metaType() const override
    { return m_property.metaType().id(); }

    bool isRequired() const override
    { return m_property.isRequired(); }

    const QMetaProperty *metaProperty() const override
    { return &m_property; }

    const QMetaProperty m_property;
};

}
}

#endif // SYSTEMUS_SECRETPROPERTY_P_H
