#include "command.h"

#include <QtSql/qsqldatabase.h>
#include <QtSql/qsqlerror.h>

#include <QtCore/qfile.h>
#include <QtCore/qjsondocument.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>

Command::Command() :
    out(stdout),
    in(stdin),
    _id(_idCounter++)
{
}

Command::~Command()
{
}

int Command::id() const
{
    return _id;
}

int Command::run(const QStringList &arguments)
{
    QCommandLineParser parser;
    parser.setApplicationDescription("Systemus Commandline Tool");
    parser.addHelpOption();
    parser.addVersionOption();

    {
        QCommandLineOption system({ "system", "s" }, "The system configuration file to use", "system.json");
        parser.addOption(system);
    }

    prepareCommand(&parser);
    parser.process(arguments);

    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    {
        const QJsonObject system = readJsonObject(parser.isSet("system") ? parser.value("system") : ":/configs/system.json");
        if (system.contains("database")) {
            const QJsonObject database = system.value("database").toObject();
            db.setDatabaseName(database.value("name").toString());
            db.setUserName(database.value("user").toString());
            db.setPassword(database.value("password").toString());
            db.setHostName(database.value("host").toString());
            db.setPort(database.value("port").toInt());
        }
    }

    if (!db.open()) {
        out << db.lastError().databaseText() << Qt::endl;
        return DATABASE_CONNECTION_ERROR;
    }

    int exitCode = runCommand(parser);
    if (exitCode != 0)
        return (_id * 100) + exitCode;
    else
        return 0;
}

QJsonObject Command::readJsonObject(const QString &fileName)
{
    QJsonDocument doc = QJsonDocument::fromJson(readFile(fileName));
    return doc.object();
}

QJsonArray Command::readJsonArray(const QString &fileName)
{
    QJsonDocument doc = QJsonDocument::fromJson(readFile(fileName));
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
