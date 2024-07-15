#include "infoscommand.h"

#include <SystemusCore/system.h>
#include <SystemusCore/user.h>

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

    const QList<Systemus::User> users = Systemus::User::all();
    for (const Systemus::User &user : users) {
        out << user.name() << Qt::endl;
        Systemus::Data data = user;
        data.dumpInfos();
    }

    return 0;
}
