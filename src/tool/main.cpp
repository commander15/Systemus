#include <QtCore/qcoreapplication.h>

#include "command.h"
#include "infoscommand.h"
#include "installcommand.h"

bool infoMode(const QString &mode);

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationVersion("1.0.0");

    const QString mode = (argc > 1 ? argv[1] : QString());
    Command *command = nullptr;
    QStringList arguments;

    if (mode.isEmpty() || infoMode(mode)) {
        command = new InfosCommand();
        arguments.append(mode);
    } else if (mode == "install") {
        command = new InstallCommand();
    } else if (mode == "update") {
    } else if (mode == "fill") {
    } else if (mode == "generate") {
    } else if (mode == "uninstall") {
    }
    
    if (command) {
        QScopedPointer commandPointer(command);

        QStringList runArguments;
        {
            const QStringList appArguments = app.arguments();

            runArguments.append(appArguments.at(0));
            if (appArguments.size() >= 2 && appArguments.at(1).startsWith('-'))
                runArguments.append(appArguments.at(1));

            runArguments.append(arguments);

            for (int i(2); i < appArguments.size(); ++i)
                runArguments.append(appArguments.at(i));
        }

        return command->run(runArguments);
    } else {
        QTextStream() << "error: unknown command" << Qt::endl;
        return COMMAND_UNKNOWN_ERROR;
    }
}

bool infoMode(const QString &mode)
{
    QStringList modes;
    modes << "users" << "roles" << "groups" << "privileges" << "permissions";
    return modes.contains(mode);
}
