#include <QtCore/qcoreapplication.h>

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
    
    QCommandlineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    
    QCommandlineOption hostOption, portOption;
    parser.addOptions({hostOption, portOption});
    
    QCommandlineOption userOption, passwordOption;
    parser.addOptions({userOption, passwordOption});
    
    QCommandlineOption databaseOption;
    parser.addOption(databaseOption);
    
    QCommandlineOption systemOption;
    parser.addOption(databaseOption);
    
    QCommandlineOption usersOption;
    parser.addOption(usersOption);
    
    QCommandlineOption rolesOption;
    parser.addOption(rolesOption);
    
    QCommandlineOption groupsOption;
    parser.addOption(groupsOption);
    
    QCommandlineOption privilegesOption;
    parser.addOption(privilegesOption);
    
    QCommandlineOption permissionsOption;
    parser.addOption(permissionsOption);
    
    parser.process(app);
    
    const QString mode = (parser.positionalArguments().isEmpty() ? "" : parser.positionalArguments().constFirst());
    bool fillData = true;
    
    if (mode.isEmpty()) {
    } else if (mode == "install") {
    } else if (mode == "update") {
    } else if (mode == "fill") {
    ] else if (mode == "generate") {
        fillData = false;
    } else if (mode == "uninstall") {
        fillData = false;
    } else {
        fillData = false;
    }
    
    if (fillData) {
        QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    }
    
    return 0;
}
