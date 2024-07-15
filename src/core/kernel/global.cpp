#include "global.h"

#include <SystemusCore/user.h>
#include <SystemusCore/private/user_p.h>

#include <SystemusCore/role.h>
#include <SystemusCore/group.h>
#include <SystemusCore/privilege.h>
#include <SystemusCore/permission.h>

#include <QtCore/qjsonobject.h>

void _s_transfer_privilege(const Systemus::Data *from, Systemus::Data *to, int type);
void _s_generate_json_privilege(const Systemus::Data &data, QJsonObject *object);

void _s_register_internal_types()
{
    static bool registered(false);
    if (!registered)
        registered = true;
    else
        return;

    sRegisterType<Systemus::User>()
        .init([] {
            sRegisterType<Systemus::UserProfile>();
        })
        /*.transfer([](const Systemus::Data *from, Systemus::Data *to, int type) {
            if (type == 0) {
                const Systemus::UserPrivate *user = static_cast<const Systemus::UserPrivate *>(from->internalData());
                to->setProperty("profile", QVariant::fromValue(user->profile));
                auto t = to->property("profile").value<Systemus::OneToOneRelation<Systemus::UserProfile>>();
            } else if (type == 1) {
                Systemus::UserPrivate *user = static_cast<Systemus::UserPrivate *>(to->internalData());
                user->profile = from->property("profile").value<Systemus::OneToOneRelation<Systemus::UserProfile>>();
            }
            _s_transfer_privilege(from, to, type);
        })
        .jsonGenerator([](const Systemus::Data &d, QJsonObject *object) {
            const Systemus::DataPrivate *data = static_cast<const Systemus::DataPrivate *>(d.internalData());

            if (data->dataType() == Systemus::DataPrivate::DefaultDataType) {
                object->insert("profile", data->properties.value("profile").value<Systemus::OneToOneRelation<Systemus::UserProfile>>().toJson());
                object->insert("role", data->properties.value("role").value<Systemus::OneToManyRelation<Systemus::Role>>().toJson());
                object->insert("groups", data->properties.value("groups").value<Systemus::ManyToManyRelation<Systemus::Group>>().toJson());
            } else if (data->dataType() == Systemus::DataPrivate::UserDataType) {
                const Systemus::UserPrivate *user = static_cast<const Systemus::UserPrivate *>(data);
                object->insert("profile", user->profile.toJson());
                object->insert("role", user->role.toJson());
                object->insert("groups", user->groups.toJson());
            }
            _s_generate_json_privilege(d, object);
        })*/;

    sRegisterType<Systemus::Role>();
    sRegisterType<Systemus::Group>();
    sRegisterType<Systemus::Privilege>();
    sRegisterType<Systemus::Permission>();
}

void _s_transfer_privilege(const Systemus::Data *from, Systemus::Data *to, int type)
{
    if (type == 0) {
        auto *data = static_cast<const Systemus::PrivilegedDataPrivate *>(from->internalData());
        to->setProperty("privileges", QVariant::fromValue(data->privileges));
        to->setProperty("permissions", QVariant::fromValue(data->permissions));
    } else if (type == 1) {
        auto *data = static_cast<Systemus::PrivilegedDataPrivate *>(to->internalData());
        data->privileges = from->property("privileges").value<Systemus::ManyToManyRelation<Systemus::Privilege>>();
        data->permissions = from->property("permissions").value<Systemus::ManyToManyRelation<Systemus::Permission>>();
    }
}

void _s_generate_json_privilege(const Systemus::Data &d, QJsonObject *object)
{
    const Systemus::DataPrivate *data = static_cast<const Systemus::DataPrivate *>(d.internalData());

    if (data->dataType() >= Systemus::DataPrivate::PrivilegeDataType || data->dataType() <= Systemus::DataPrivate::GroupDataType) {
        auto data = static_cast<const Systemus::PrivilegedDataPrivate *>(d.internalData());
    }
}
