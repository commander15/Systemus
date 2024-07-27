#include "installcommand.h"

#include <SystemusCore/global.h>

#include <QtSql/qsqldatabase.h>
#include <QtSql/qsqlquery.h>
#include <QtSql/qsqldriver.h>
#include <QtSql/qsqlrecord.h>

#include <QtCore/qdatetime.h>

void InstallCommand::prepareCommand(QCommandLineParser *parser)
{
    Q_UNUSED(parser);
}

int InstallCommand::runCommand(const QCommandLineParser &parser)
{
    const QString fileName = QStringLiteral(":/scripts/%1/Systemus%2.sql");
    const QString database = QSqlDatabase::database().driverName().mid(1, -1);

    const QStringList versionNames = { "1.0.0" };
    const int installedVersion = 0;
    const int latestVersion = 0;

    QSqlDriver *driver = QSqlDatabase::database().driver();
    QSqlQuery query;

    for (int i(0); i <= latestVersion; ++i) {
        QByteArray request;
        readFileLineByLine(fileName.arg(database).arg(i), [&request](const QByteArray &line) {
            request.append(line);

            if (line.endsWith(';')) {
                QSqlQuery q;
                if (!q.exec(request)) {
                    //...
                }
                    //out << q.lastError() << ;
                request.clear();
            }
        });

        // Updating system data...
        if (i == 0) {
            QSqlRecord record;
            record.setValue(0, "Systemus");
            record.setValue(1, SYSTEMUS_VERSION_STR);

            QString statement = driver->sqlStatement(QSqlDriver::InsertStatement, "Systems", record, false);
            query.exec(statement);
        }

        {
            QSqlRecord record;
            record.setValue(0, i);
            record.setValue(1, versionNames.at(i));
            record.setValue(2, QDate::currentDate());
            record.setValue(3, QTime::currentTime());

            QString statement = driver->sqlStatement(QSqlDriver::InsertStatement, "SystemInstallations", record, false);
            query.exec(statement);
        }

        if (i == latestVersion) {
            QSqlRecord record;
            record.setValue(0, "Updated to version " + versionNames.at(i) + " !");
            record.setValue(1, "System has been updated to version " + versionNames.at(i) + " !");
            record.setValue(2, "{}");
            record.setValue(3, "Update");
            record.setValue(4, QDate::currentDate());
            record.setValue(5, QTime::currentTime());

            QString statement = driver->sqlStatement(QSqlDriver::InsertStatement, "SystemNotifications", record, false);
            query.exec(statement);
        }
    }

    return 0;
}
