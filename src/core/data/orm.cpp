#include "orm.h"
#include "orm_p.h"

#include <SystemusCore/system.h>
#include <SystemusCore/user.h>
#include <SystemusCore/role.h>
#include <SystemusCore/group.h>
#include <SystemusCore/privilege.h>
#include <SystemusCore/permission.h>

#include <QtSql/qsqlquery.h>
#include <QtSql/qsqlrecord.h>
#include <QtSql/qsqlfield.h>
#include <QtSql/qsqlerror.h>
#include <QtSql/qsqldriver.h>

#include <QtCore/qmetaobject.h>
#include <QtCore/qregularexpression.h>

namespace Systemus {

namespace Orm {

void init(int argc, char *argv[])
{
    sRegisterType<User>();
    sRegisterType<UserProfile>();
    sRegisterType<Role>();
    sRegisterType<Group>();
    sRegisterType<Privilege>();
    sRegisterType<Permission>();

    Q_UNUSED(argc);
    Q_UNUSED(argv);
}

QString tableNameFromMetaObject(const QMetaObject *object)
{
    const int index = object->indexOfClassInfo("table");
    return (index >= 0 ? object->classInfo(index).value() : tableNameFromClassName(object->className()));
}

QSqlRecord recordFromMetaObject(const QMetaObject *object)
{
    QSqlRecord record;

    for (int i(0); i < object->propertyCount(); ++i) {
        const QMetaProperty property = object->property(i);
        if (property.isStored())
            record.append(fieldFromMetaProperty(property));
    }

    const QMetaObject *metaObject = object;
    while (metaObject) {
        const int index = metaObject->indexOfClassInfo("fields");
        if (index >= 0) {
            const QList<QSqlField> fields = fieldsFromString(metaObject->classInfo(index).value());
            for (const QSqlField &field : fields)
                record.append(field);
        }

        metaObject = metaObject->superClass();
    }

    return record;
}

QSqlField fieldFromMetaProperty(const QMetaProperty &property)
{
    QSqlField field;
    field.setName(fieldNameFromPropertyName(property.name()));
    field.setMetaType(property.metaType());
    field.setReadOnly(property.isReadable() && !property.isWritable());
    field.setRequired(property.isRequired());
    //field.setGenerated(property.isWritable());
    field.setTableName(tableNameFromMetaObject(property.enclosingMetaObject()));
    return field;
}

QString classNameFromTableName(const QString &tableName)
{
    QString className;
    if (OrmPrivate::ormOptions.testFlag(OrmPrivate::UseNamespaceAsDatabase)) {
        className = tableName.chopped(1);
        className.replace(".", "::");
    } else
        className = tableName.section('.', -1);
    return className;
}

QString tableNameFromClassName(const QString &className)
{
    QString tableName;
    if (OrmPrivate::ormOptions.testFlag(OrmPrivate::UseNamespaceAsDatabase)) {
        tableName = className;
        tableName.replace("::", ".");
    } else
        tableName = className.section("::", -1) + 's';
    return tableName;
}

QString fieldNameFromPropertyName(const QString &propertyName)
{
    QString property = propertyName;
    property.replace(".", "s.");
    return property;
}

QString propertyNameFromFieldName(const QString &fieldName)
{
    QString field = fieldName;
    field.replace("s.", ".");
    return field;
}

QList<QSqlField> fieldsFromString(const QString &str)
{
    QList<QSqlField> fields;

    const QStringList strings = str.split(',');
    for (const QString &string : strings) {
        const QSqlField field = fieldFromString(string);
        if (field.isValid())
            fields.append(field);
    }

    return fields;
}

QSqlField fieldFromString(const QString &str)
{
    static QRegularExpression regExp("^(?<name>[a-zA-Z]+)\\((?<type>[A-Za-z]+),?(?<options>[a-z, ]*)\\)$");

    const QRegularExpressionMatch match = regExp.match(str.trimmed().replace(" ", ""));
    if (match.hasMatch()) {
        QSqlField field;
        field.setName(fieldNameFromPropertyName(match.captured("name")));
        field.setMetaType(QMetaType::fromName(match.captured("type").toLatin1()));

        if (match.hasCaptured("options")) {
            const QStringList options = match.captured("options").split(',', Qt::SkipEmptyParts);
            for (const QString &option : options) {
                if (option == "required")
                    field.setRequired(true);
            }
        }

        return field;
    } else {
        return QSqlField();
    }
}

QSqlField fieldFromVariant(const QVariant &value)
{
    QSqlField field(QString(), value.metaType());
    field.setValue(value);
    return field;
}

QString formatValue(const QVariant &value)
{
    QSqlDriver *driver = QSqlDatabase::database(SystemusPrivate::dbConnection).driver();
    return driver->formatValue(fieldFromVariant(value), true);
}

}

}
