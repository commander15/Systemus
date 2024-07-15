#ifndef INFOSCOMMAND_H
#define INFOSCOMMAND_H

#include "command.h"

class InfosCommand : public Command
{
protected:
    void prepareCommand(QCommandLineParser *parser);
    int runCommand(const QCommandLineParser &parser);
};

#endif // INFOSCOMMAND_H
