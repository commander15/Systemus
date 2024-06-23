#include <QtCore/qcoreapplication.h>
#include <QtCore/qtextstream.h>
#include <QtCore/qcommandlineparser.h>
#include <QtCore/qcommandlineoption.h>

#include <QtSql/qsqldatabase.h>
#include <QtSql/qsqlerror.h>

#include <QMetaProperty>

#include <SystemusCore/user.h>
#include <SystemusCore/role.h>
#include <SystemusCore/privilege.h>
#include <SystemusCore/permission.h>

#include "system.h"

#define ERROR_DB_CONNECTION 1

bool initDatabase();
bool updateDatabase();
bool destroyDatabase();

bool fillSystem(const QJsonObject &system);
bool fillUsers(const QList<QJsonObject> &users);
bool fillRoles(const QList<QJsonObject> &roles);
bool fillGroups(const QList<QJsonObject> &groups);
bool fillPrivileges(const QList<QJsonObject> &privileges);
bool fillPermissions(const QList<QJsonObject> &permissions);

bool generateConfigFile(const QString &fileName);

QJsonObject readJsonObject(const QString &fileName);
QList<QJsonObject> readJsonObjects(const QString &fileName);

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationVersion("1.0.0");

    QTextStream out(stdout), err(stderr);
    
    QCommandLineParser parser;
    parser.setApplicationDescription("Systemus commandline tool");
    parser.addHelpOption();
    parser.addVersionOption();
    
    QCommandLineOption hostOption("host", "Databse host", "host", "127.0.0.1"), portOption("port", "Database host port", "port", "3306");
    parser.addOptions({hostOption, portOption});
    
    QCommandLineOption userOption("user", "Database user", "user", "root"), passwordOption("password", "Database password", "password");
    parser.addOptions({userOption, passwordOption});
    
    QCommandLineOption databaseOption("database", "Database name", "database", "Systemus");
    parser.addOption(databaseOption);
    
    QCommandLineOption systemOption("system", "Load system configuration from <file>", "file");
    parser.addOption(systemOption);
    
    /*QCommandLineOption usersOption;
    parser.addOption(usersOption);
    
    QCommandLineOption rolesOption;
    parser.addOption(rolesOption);
    
    QCommandLineOption groupsOption;
    parser.addOption(groupsOption);
    
    QCommandLineOption privilegesOption;
    parser.addOption(privilegesOption);
    
    QCommandLineOption permissionsOption;
    parser.addOption(permissionsOption);*/
    
    parser.process(app);

    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setDatabaseName(parser.value(databaseOption));
    db.setHostName(parser.value(hostOption));
    db.setPort(parser.value(portOption).toInt());
    db.setUserName(parser.value(userOption));
    db.setPassword(parser.value(passwordOption));
    if (!db.open()) {
        err << db.lastError().databaseText() << Qt::endl;
        return ERROR_DB_CONNECTION;
    }
    
    const QString mode = (parser.positionalArguments().isEmpty() ? "" : parser.positionalArguments().constFirst());
    bool fillData = true;
    
    if (mode.isEmpty()) {
        Systemus::User user;
        user.get(1);
        user.dumpInfos();

        if (user.hasPermission("destinations.list"))
            out << "Access granted !" << Qt::endl;
        else
            out << "Accesss Denied !" << Qt::endl;
    } else if (mode == "install") {
    } else if (mode == "update") {
    } else if (mode == "fill") {
    } else if (mode == "generate") {
        fillData = false;
    } else if (mode == "uninstall") {
        fillData = false;
    } else {
        fillData = false;
    }
    
    if (fillData) {
    }
    
    return 0;
}
