#ifndef COMMAND_H
#define COMMAND_H

#include <QtCore/qjsonobject.h>
#include <QtCore/qcommandlineparser.h>
#include <QtCore/qtextstream.h>

#define COMMAND_UNKNOWN_ERROR       10

#define SYSTEM_FILE_NOT_FOUND_ERROR 20
#define SYSTEM_FILE_INVALID_ERROR   22

#define DATABASE_INVALID_OPTIONS    30
#define DATABASE_CONNECTION_ERROR   32
#define DATABASE_QUERY_ERROR        34

class QJsonObject;
class QJsonArray;
class QJsonParseError;

typedef void(FileLineReadCallback)(const QByteArray &line);
typedef void(FileCharReadCallback)(const char &c);

class System;

class Command
{
public:
    Command();
    virtual ~Command();

    int id() const;

    int run(const QStringList &arguments);

protected:
    virtual void prepareCommand(QCommandLineParser *parser) = 0;
    virtual int runCommand(const QCommandLineParser &parser) = 0;

    static bool isFileExists(const QString &fileName);

    static QJsonObject readJsonObject(const QString &fileName, QJsonParseError *error = nullptr);
    static QJsonArray readJsonArray(const QString &fileName, QJsonParseError *error = nullptr);
    static QByteArray readFile(const QString &fileName);
    static bool readFileLineByLine(const QString &fileName, const std::function<FileLineReadCallback> &callback);
    static bool readFileCharByChar(const QString &fileName, const std::function<FileCharReadCallback> &callback);

    System *system;

    QTextStream out, err, in;

private:
    int _id;
    static int _idCounter;
};

class System
{
public:
    System();

    QString name() const;
    QString version() const;
    int versionCode() const;

    QJsonObject object;

private:
    int _versionCode;

    friend class Command;
};

#endif // COMMAND_H
