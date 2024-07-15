#ifndef COMMAND_H
#define COMMAND_H

#include <QtCore/qcommandlineparser.h>
#include <QtCore/qtextstream.h>

#define UNKNWON_COMMAND_ERROR     1
#define DATABASE_CONNECTION_ERROR 10

class QJsonObject;

typedef void(FileLineReadCallback)(const QByteArray &line);
typedef void(FileCharReadCallback)(const char &c);

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

    static QJsonObject readJsonObject(const QString &fileName);
    static QJsonArray readJsonArray(const QString &fileName);
    static QByteArray readFile(const QString &fileName);
    static bool readFileLineByLine(const QString &fileName, const std::function<FileLineReadCallback> &callback);
    static bool readFileCharByChar(const QString &fileName, const std::function<FileCharReadCallback> &callback);

    QTextStream out, in;

private:
    int _id;
    static int _idCounter;
};

#endif // COMMAND_H
