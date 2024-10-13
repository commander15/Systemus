#include "orm.h"
#include "orm_p.h"

#include <SystemusCore/system.h>
#include <SystemusCore/user.h>
#include <SystemusCore/ormbackend.h>
#include <SystemusCore/metamapper.h>
#include <SystemusCore/metatable.h>
#include <SystemusCore/private/namespace_p.h>
#include <SystemusCore/private/debug_p.h>

#include <QtSql/qsqlrecord.h>
#include <QtSql/qsqlfield.h>
#include <QtSql/qsqldriver.h>

#include <QtCore/qregularexpression.h>
#include <QtCore/qfile.h>

namespace Systemus {

namespace Orm {

void init(int convention)
{
    switch (convention) {
    case LaravelConvention:
        setBackend(new LaravelBackend());
        break;

    default:
        setBackend(new SystemusBackend());
        break;
    }
}

void init(Backend *backend)
{
    setBackend(backend);
}

void registerClasses()
{
    {
        const QMetaObject *metaObject = &Systemus::System::staticMetaObject;
        MetaTable::registerClass(metaObject->className(), metaObject, QMetaType());
    }

    MetaTable::registerClass<User>();
    MetaTable::registerClass<UserProfile>();
    MetaTable::registerClass<UserRole>();
    MetaTable::registerClass<UserGroup>();
    //MetaTable::registerClass<Privilege>();
    //MetaTable::registerClass<Permission>();
}

Backend *backend()
{
    return Backend::instance();
}

void setBackend(Backend *backend)
{
    MetaTable::clearTables();
    Backend::setInstance(backend);
    registerClasses();
}

QString formatExpression(const QString &expression)
{
    return formatExpression(expression, QString());
}

QString formatExpression(const QString &expression, const QString &contextClassName)
{
    MetaTable contextTable = (!contextClassName.isEmpty() ? MetaTable::fromClassName(contextClassName) : MetaTable());

    static const QRegularExpression regExp("({{\\s*[A-Za-z_][A-Za-z0-9_\\:\\.]*\\s*}})");
    const int matchIndex = 1;

    QRegularExpressionMatchIterator it = regExp.globalMatch(expression, 0, QRegularExpression::NormalMatch);

    struct Replacement {
        QString match;
        int pos;
        int size;
        QString value;
    };
    QList<Replacement> replacements;

    while (it.hasNext()) {
        const QRegularExpressionMatch match = it.next();

        if (!match.hasMatch() || !match.hasCaptured(matchIndex))
            continue;

        Replacement replacement;
        replacement.match = match.captured(matchIndex);
        replacement.match = replacement.match.sliced(2, replacement.match.size() - 4);
        replacement.match = replacement.match.trimmed();
        replacement.pos = match.capturedStart(matchIndex);
        replacement.size = match.capturedLength(matchIndex);

        if (replacement.match.contains('.')) {
            const QStringList items = replacement.match.split('.', Qt::SkipEmptyParts);
            replacement.value = MetaMapper::fieldName(items.last(), items.first(), MetaMapper::IncludeTableName);
        } else if (replacement.match.at(0).isUpper()) {
            replacement.value = MetaMapper::tableName(replacement.match);

            if (!contextTable.isValid())
                contextTable = MetaTable::fromClassName(replacement.match);
        } else if (contextTable.isValid()) {
            replacement.value = MetaMapper::fieldName(replacement.match, contextTable);
        }

        replacements.append(replacement);
    }

    QString result;
    int lastPos = 0;

    for (const Replacement &rep : replacements) {
        // Append the part of the string before the current match
        result.append(expression.mid(lastPos, rep.pos - lastPos));

        // Append the replacement string
        if (!rep.value.isEmpty())
            result.append(rep.value);
        else
            result.append(MetaMapper::fieldName(rep.match, contextTable));

        // Update the last position after the match
        lastPos = rep.pos + rep.size;
    }

    // Append the remaining part of the original string
    result.append(expression.mid(lastPos));

    return result;
}


QString formatValue(const QVariant &value)
{
    QSqlDriver *driver = QSqlDatabase::database(SystemusPrivate::dbConnection).driver();
    QSqlField field(QString(), value.metaType());
    field.setValue(value);
    return driver->formatValue(field);
}

QString selectStatement(const MetaTable &table)
{
    return Systemus::sqlStatement(QSqlDriver::SelectStatement, table.tableName(), table.record(), false);
}

QString updateStatement(const MetaTable &table)
{
    return Systemus::sqlStatement(QSqlDriver::UpdateStatement, table.tableName(), table.record(), false);
}

QString insertStatement(const MetaTable &table)
{
    return Systemus::sqlStatement(QSqlDriver::InsertStatement, table.tableName(), table.record(), false);
}

QString deleteStatement(const MetaTable &table)
{
    return Systemus::sqlStatement(QSqlDriver::DeleteStatement, table.tableName(), table.record(), false);
}

}

QStringList OrmPrivate::sqlKeywords()
{
    //ToDo: remove this function and the resource file linked, no longer used

    QStringList keywords;

    QFile file(":/systemus/sql_keywords.txt");
    if (file.open(QIODevice::QIODevice::ReadOnly|QIODevice::Text)) {
        QByteArray line;
        while (!file.atEnd()) {
            char c;
            file.getChar(&c);

            if (c != '\n') {
                line.append(c);
            } else {
                line = line.trimmed();
                if (!line.isEmpty()) {
                    if (!line.startsWith('#'))
                        keywords.append(line);
                    line.clear();
                }
            }
        }
        file.close();
    }

    if (keywords.isEmpty())
        systemusWarning() << "Systemus::Orm: SQL keyword database load failed !";

    return keywords;
}

}
