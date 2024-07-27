#ifndef INSTALLCOMMAND_H
#define INSTALLCOMMAND_H

#include "command.h"

class InstallCommand : public Command
{
protected:
    void prepareCommand(QCommandLineParser *parser);
    int runCommand(const QCommandLineParser &parser);
};

#endif // INSTALLCOMMAND_H
