#include "installcommand.h"

#include <QtSql/qsqlquery.h>
#include <QtSql/qsqlerror.h>

#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>

void InstallCommand::prepareCommand(QCommandLineParser *parser)
{
}

int InstallCommand::runCommand(const QCommandLineParser &parser)
{
    // ToDo: must get system general infos, users, roles, groups, privileges, permissions and notifications !

    if (runScripts() != 0)
        return DATABASE_CONNECTION_ERROR;

    return true;
}

int InstallCommand::runScripts()
{
    const int version = 0; // Must be deduced from the database if the Systems table exists and has at least one record !
    int errors = 0;

    for (int i(version); i < version; ++i) {
        const QString fileName(":/scripts/Systemus" + QString::number(i) + ".sql");

        QByteArray request;
        readFileCharByChar(fileName, [&request, &errors, this](const char &c) {
            if (c == ' ');
            else if (c != ';')
                request.append(c);
            else {
                QSqlQuery query;
                if (!query.exec(request)) {
                    out << query.lastError().databaseText() << Qt::endl;
                    ++errors;
                }
            }
        });
    }

    return errors;
}
