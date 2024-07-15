#include <QtCore/qcoreapplication.h>

#include "command.h"
#include "infoscommand.h"
#include "installcommand.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationVersion("1.0.0");

    const QString mode = (argc > 1 ? argv[1] : QString());
    Command *command = nullptr;

    if (mode.isEmpty()) {
        command = new InfosCommand();
    } else if (mode == "install") {
        command = new InstallCommand();
    } else if (mode == "update") {
    } else if (mode == "fill") {
    } else if (mode == "generate") {
    } else if (mode == "uninstall") {
    }
    
    if (command) {
        QScopedPointer commandPointer(command);

        QStringList arguments = app.arguments();
        if (arguments.size() > 1)
            arguments.remove(1);
        return command->run(arguments);
    } else {
        QTextStream() << "error: unknown command" << Qt::endl;
        return UNKNWON_COMMAND_ERROR;
    }
}
