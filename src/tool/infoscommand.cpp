#include "infoscommand.h"

#include <SystemusCore/system.h>
#include <SystemusCore/user.h>
#include <SystemusCore/role.h>
#include <SystemusCore/group.h>
#include <SystemusCore/privilege.h>
#include <SystemusCore/permission.h>

#include <QtCore/qversionnumber.h>

void InfosCommand::prepareCommand(QCommandLineParser *parser)
{
    Q_UNUSED(parser);
}

int InfosCommand::runCommand(const QCommandLineParser &parser)
{
    Q_UNUSED(parser);

    Systemus::System *system = Systemus::System::instance();
    out << system->name() << Qt::endl;
    out << system->version().toString() << Qt::endl;

    if (parser.positionalArguments().size() > 0)
        qInfo() << "";
    else
        return 0;

    const QString entities = parser.positionalArguments().constFirst();
    int count = 0;

    if (entities == "users") {
        const QList<Systemus::User> users = Systemus::User::all();
        count = users.count();
        for (const Systemus::User &user : users) {
            qInfo().noquote().nospace() << user.profile().fullName();
            dumpData(user);
        }
    } else if (entities == "roles") {
        const QList<Systemus::Role> roles = Systemus::Role::all();
        count = roles.count();
        for (const Systemus::Role &role : roles)
            dumpData(role);
    } else if (entities == "groups") {
        const QList<Systemus::Group> groups = Systemus::Group::all();
        count = groups.count();
        for (const Systemus::Group &group : groups)
            dumpData(group);
    } else if (entities == "privileges") {
        const QList<Systemus::Privilege> privileges = Systemus::Privilege::all();
        count = privileges.count();
        for (const Systemus::Privilege &privilege : privileges)
            dumpData(privilege);
    } else if (entities == "permissions") {
        const QList<Systemus::Permission> permissions = Systemus::Permission::all();
        count = permissions.count();
        for (const Systemus::Permission &permission : permissions)
            dumpData(permission);
    }

    qInfo().nospace().noquote() << count << ' ' << entities.chopped(1) << "(s) found !";

    return 0;
}

void InfosCommand::dumpData(const Systemus::Data &data)
{
    qInfo().noquote().nospace() << data.property(data.dataInfo().userPropertyName()).toString();
    data.dumpInfos();
}
