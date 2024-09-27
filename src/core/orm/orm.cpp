#include "orm.h"
#include "orm_p.h"

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

void init()
{
    MetaTable::registerClass<User>();
    MetaTable::registerClass<UserProfile>();
    MetaTable::registerClass<UserRole>();
    //MetaTable::registerClass<UserGroup>();
    //MetaTable::registerClass<Privilege>();
    //MetaTable::registerClass<Permission>();
}

QString formatExpression(const QString &expression, const QString &contextClassName)
{
    const MetaTable contextTable = MetaTable::fromClassName(contextClassName);

    static const QRegularExpression regExp("({{\\s*[A-Za-z_][A-Za-z0-9_\\.]*\\s*}})");
    const int matchIndex = 1;

    QRegularExpressionMatchIterator it = regExp.globalMatch(expression, 0, QRegularExpression::NormalMatch);

    struct Replacement {
        int pos;
        int size;
        QString match;
        QString replacement;
    };
    QList<Replacement> replacements;

    while (it.hasNext()) {
        const QRegularExpressionMatch match = it.next();

        if (!match.hasMatch() || !match.hasCaptured(matchIndex))
            continue;

        Replacement r;
        r.pos = match.capturedStart(matchIndex);
        r.size = match.capturedLength(matchIndex);

        r.match = match.captured(matchIndex);
        r.replacement = r.match;
        r.replacement.remove(' ');
        r.replacement.remove("{{");
        r.replacement.remove("}}");

        if (r.replacement.contains('.')) {
            const QStringList items = r.replacement.split('.', Qt::SkipEmptyParts);
            r.replacement = MetaMapper::fieldName(items.last(), items.first(), MetaMapper::IncludeTableName);
        } else if (r.replacement.at(0).isUpper()) {
            r.replacement = MetaMapper::tableName(r.replacement);
        } else {
            r.replacement = MetaMapper::fieldName(r.replacement, contextTable);
        }

        replacements.append(r);
    }

    QString result;
    int lastPos = 0;

    for (const Replacement &rep : replacements) {
        // Append the part of the string before the current match
        result.append(expression.mid(lastPos, rep.pos - lastPos));
        // Append the replacement string
        result.append(rep.replacement);
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
