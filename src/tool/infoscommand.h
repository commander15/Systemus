#ifndef INFOSCOMMAND_H
#define INFOSCOMMAND_H

#include "command.h"

namespace Systemus {
class Data;
}

class InfosCommand : public Command
{
protected:
    void prepareCommand(QCommandLineParser *parser);
    int runCommand(const QCommandLineParser &parser);

private:
    void dumpData(const Systemus::Data &data);
};

#endif // INFOSCOMMAND_H
