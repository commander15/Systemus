#include "orm.h"
#include "orm_p.h"

//#include <SystemusCore/user.h>
//#include <SystemusCore/role.h>
//#include <SystemusCore/group.h>
//#include <SystemusCore/privilege.h>
//#include <SystemusCore/permission.h>
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
    //sRegisterType<User>();
    //sRegisterType<UserProfile>();
    //sRegisterType<Role>();
    //sRegisterType<Group>();
    //sRegisterType<Privilege>();
    //sRegisterType<Permission>();
}

QString formatExpression(const QString &expression, const QString &contextClassName)
{
    static const QRegularExpression regExp("(?!\\b(?:" + OrmPrivate::sqlKeywords().join('|') + ")\\b|\\b'[^\\']*'\\b)(?<!'\\b)(\\b[A-Za-z_][A-Za-z0-9_]*\\b)(?:\\.(\\b[a-z_][A-Za-z0-9_]*\\b))?");
    QRegularExpressionMatchIterator it = regExp.globalMatch(expression, 0, QRegularExpression::NormalMatch);

    struct Replacement {
        int pos;
        int size;
        QString match;
        QString replacement;
    };
    QList<Replacement> replacements;

    MetaMapper::MapOptions replacementOptions = MetaMapper::EscapeIdentifiers;

    while (it.hasNext()) {
        const QRegularExpressionMatch match = it.next();

        const QString first = match.captured(1);
        const QString second = match.captured(2);

        Replacement r;

        if (!first.isEmpty() && !second.isEmpty()) {
            const MetaTable table = MetaTable::fromClassName(first);

            r.pos = match.capturedStart(1);
            r.size = match.capturedLength(1);
            r.match = first;
            r.replacement = MetaMapper::tableName(table, replacementOptions);
            replacements.append(r);

            r.pos = match.capturedStart(2);
            r.size = match.capturedLength(2);
            r.match = second;
            r.replacement = MetaMapper::fieldName(second, table, replacementOptions);
            replacements.append(r);
        } else if (!first.isEmpty()) {
            r.pos = match.capturedStart(1);
            r.size = match.capturedLength(1);
            r.match = first;

            if (first.at(0).isUpper()) {
                r.replacement = MetaMapper::tableName(first, replacementOptions);
            } else {
                const MetaTable table = MetaTable::fromClassName(contextClassName);
                //replacementOptions |= MetaMapper::IncludeTableName;
                r.replacement = MetaMapper::fieldName(first, table, replacementOptions);
            }

            replacements.append(r);
        }
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
