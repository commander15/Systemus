#include "command.h"

#include <QtSql/qsqldatabase.h>
#include <QtSql/qsqlquery.h>
#include <QtSql/qsqlerror.h>

#include <QtCore/qfile.h>
#include <QtCore/qjsondocument.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>

Command::Command() :
    system(new System),
    out(stdout),
    in(stdin),
    _id(_idCounter++)
{
}

Command::~Command()
{
    delete system;
}

int Command::id() const
{
    return _id;
}

int Command::run(const QStringList &arguments)
{
    QCommandLineParser parser;
    parser.setApplicationDescription("Systemus Commandline Tool");

    QCommandLineOption helpOption("help", "");
    parser.addOption(helpOption);

    parser.addVersionOption();

    QCommandLineOption systemOption(QStringList() << "s" << "system", "System configuration file (e.g system.json).", "system");
    parser.addOption(systemOption);

    // Database options
    QCommandLineOption hostOption(QStringList() << "h" << "host", "Database host address.", "host");
    parser.addOption(hostOption);

    QCommandLineOption portOption(QStringList() << "p" << "port", "Database port.", "port");
    parser.addOption(portOption);

    QCommandLineOption userOption(QStringList() << "u" << "user", "Database user.", "user");
    parser.addOption(userOption);

    QCommandLineOption passwordOption(QStringList() << "w" << "password", "Database password.", "password");
    parser.addOption(passwordOption);

    QCommandLineOption databaseOption(QStringList() << "d" << "database", "Database name.", "database");
    parser.addOption(databaseOption);

    QCommandLineOption typeOption(QStringList() << "t" << "type", "Database type (e.g MYSQL, SQLITE).", "type");
    parser.addOption(typeOption);

    prepareCommand(&parser);
    parser.process(arguments);

    if (parser.isSet(helpOption))
        parser.showHelp();

    QJsonObject system;
    if (parser.isSet(systemOption)) {
        const QString file = parser.value(systemOption);
        if (isFileExists(file)) {
            QJsonParseError error;
            system = readJsonObject(file, &error);

            if (error.error == QJsonParseError::NoError) {
                this->system->object = system;
                if (this->system->object.contains("database"))
                    this->system->object.remove("database");
            } else {
                err << "error: invalid system file provided" << Qt::endl;
                return SYSTEM_FILE_INVALID_ERROR;
            }
        } else {
            err << "error: system file provided doesn't exists" << Qt::endl;
            return SYSTEM_FILE_NOT_FOUND_ERROR;
        }
    }

    QString host;
    int port = 0;
    QString user;
    QString password;
    QString database;
    QString driver;

    if (parser.isSet(typeOption)) {
        driver = "Q" + parser.value(typeOption).toUpper();
    } else if (!system.isEmpty() && system.contains("database") && system["database"].isObject()) {
        QJsonObject dbObject = system["database"].toObject();
        if (dbObject.contains("type")) {
            driver = "Q" + dbObject["type"].toString().toUpper();
        }
    }

    if (driver.isEmpty()) {
        err << "error: database type not specified" << Qt::endl;
        return DATABASE_INVALID_OPTIONS;
    }

    if (driver == "QSQLITE") {
        if (parser.isSet(databaseOption)) {
            database = parser.value(databaseOption);
        } else if (!system.isEmpty() && system.contains("database") && system["database"].isObject()) {
            QJsonObject dbObject = system["database"].toObject();
            if (dbObject.contains("name")) {
                database = dbObject["name"].toString();
            }
        }

        if (database.isEmpty()) {
            err << "error: not enough database options for SQLITE" << Qt::endl;
            return DATABASE_INVALID_OPTIONS;
        }
    } else {
        if (parser.isSet(hostOption)) {
            host = parser.value(hostOption);
        } else if (!system.isEmpty() && system.contains("database") && system["database"].isObject()) {
            QJsonObject dbObject = system["database"].toObject();
            if (dbObject.contains("host")) {
                host = dbObject["host"].toString();
            }
        }

        if (parser.isSet(portOption)) {
            port = parser.value(portOption).toInt();
        } else if (!system.isEmpty() && system.contains("database") && system["database"].isObject()) {
            QJsonObject dbObject = system["database"].toObject();
            if (dbObject.contains("port")) {
                port = dbObject["port"].toInt();
            }
        }

        if (parser.isSet(userOption)) {
            user = parser.value(userOption);
        } else if (!system.isEmpty() && system.contains("database") && system["database"].isObject()) {
            QJsonObject dbObject = system["database"].toObject();
            if (dbObject.contains("user")) {
                user = dbObject["user"].toString();
            }
        }

        if (parser.isSet(passwordOption)) {
            password = parser.value(passwordOption);
        } else if (!system.isEmpty() && system.contains("database") && system["database"].isObject()) {
            QJsonObject dbObject = system["database"].toObject();
            if (dbObject.contains("password")) {
                password = dbObject["password"].toString();
            }
        }

        if (parser.isSet(databaseOption)) {
            database = parser.value(databaseOption);
        } else if (!system.isEmpty() && system.contains("database") && system["database"].isObject()) {
            QJsonObject dbObject = system["database"].toObject();
            if (dbObject.contains("name")) {
                database = dbObject["name"].toString();
            }
        }

        if (host.isEmpty() || user.isEmpty() || database.isEmpty()) {
            err << "error: not enough database options" << Qt::endl;
            return DATABASE_INVALID_OPTIONS;
        }
    }

    QSqlDatabase db = QSqlDatabase::addDatabase(driver);
    db.setHostName(host);
    if (port > 0)
        db.setPort(port);
    db.setUserName(user);
    db.setPassword(password);
    db.setDatabaseName(database);

    if (db.open()) {
        if (db.tables().contains("SystemInstallations")) {
            QSqlQuery query;
            if (query.exec("SELECT id FROM SystemInstallations ORDER BY id ASC LIMIT 1") && query.next())
                this->system->_versionCode = query.value(0).toInt();
        }
    } else {
        err << db.lastError().databaseText() << Qt::endl;
        return DATABASE_CONNECTION_ERROR;
    }

    int exitCode = runCommand(parser);
    if (exitCode != 0)
        return (_id * 100) + exitCode;
    else
        return 0;
}

bool Command::isFileExists(const QString &fileName)
{
    return QFile::exists(fileName);
}

QJsonObject Command::readJsonObject(const QString &fileName, QJsonParseError *error)
{
    QJsonDocument doc = QJsonDocument::fromJson(readFile(fileName), error);
    return doc.object();
}

QJsonArray Command::readJsonArray(const QString &fileName, QJsonParseError *error)
{
    QJsonDocument doc = QJsonDocument::fromJson(readFile(fileName), error);
    return doc.array();
}

QByteArray Command::readFile(const QString &fileName)
{
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly))
        return file.readAll();
    else
        return QByteArray();
}

bool Command::readFileLineByLine(const QString &fileName, const std::function<FileLineReadCallback> &callback)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    while (!file.atEnd())
        callback(file.readLine());

    return true;
}

bool Command::readFileCharByChar(const QString &fileName, const std::function<FileCharReadCallback> &callback)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    while (!file.atEnd()) {
        char c;
        file.getChar(&c);
        callback(c);
    }

    return true;
}

int Command::_idCounter(1);

System::System() :
    _versionCode(0)
{
}

QString System::name() const
{
    return object.value("name").toString();
}

QString System::version() const
{
    return object.value("version").toString();
}

int System::versionCode() const
{
    return _versionCode;
}
