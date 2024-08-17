#ifndef SYSTEMUS_METATABLE_H
#define SYSTEMUS_METATABLE_H

#include <SystemusCore/global.h>

#include <QtCore/qshareddata.h>
#include <QtCore/qmetaobject.h>

class QMetaProperty;
class QMetaObject;

class QSqlField;
class QSqlRecord;

namespace Systemus {
namespace Orm {

class SYSTEMUS_CORE_EXPORT MetaTableData
{
public:
    int primaryPropertyIndex = 0;
    QList<int> metaPropertyIndexes;
    const QMetaObject *metaObject = nullptr;
};

class SYSTEMUS_CORE_EXPORT MetaTable
{
public:
    MetaTable();
    MetaTable(const MetaTable &other);
    ~MetaTable();

    MetaTable &operator=(const MetaTable &other);

    QString className() const;
    QString tableName() const;

    QString primaryPropertyName() const;
    QMetaProperty primaryProperty() const;
    QString primaryFieldName() const;
    QSqlField primaryField() const;

    QString userPropertyName() const;
    QMetaProperty userProperty() const;
    QString userFieldName() const;
    QSqlField userField() const;

    QStringList metaPropertyNames() const;
    QString metaPropertyName(int index) const;
    QMetaProperty metaProperty(int index) const;
    QStringList metaFieldNames() const;
    QString metaFieldName(int index) const;
    QSqlField metaField(int index) const;
    int metaCount() const;
    QSqlRecord metaRecord() const;

    QStringList secretPropertyNames() const;
    QString secretPropertyName(int index) const;
    QMetaProperty secretProperty(int index) const;
    QStringList secretFieldNames() const;
    QString secretFieldName(int index) const;
    QSqlField secretField(int index) const;
    int secretCount() const;
    QSqlRecord secretRecord() const;

    QStringList propertyNames() const;
    QString propertyName(int index) const;
    QMetaProperty property(int index) const;
    QStringList fieldNames() const;
    QString fieldName(int index) const;
    QSqlField field(int index) const;
    int count() const;
    QSqlRecord record() const;

    static MetaTable fromName(const QString &className);
    template<typename T> static inline MetaTable fromType()
    { return fromName(T::staticMetaObject.className()); }

    static void registerType(const QString &className, const QMetaObject *metaObject, const QMetaType &metaType);

private:
    MetaTable(MetaTableData *d);

    QSharedDataPointer<MetaTableData> d_ptr;

    static QHash<QString, MetaTable> s_records;
};

}
}
template<typename T>
void sRegisterType()
{
    const QMetaObject *object(&T::staticMetaObject);
    const QMetaType type(qRegisterMetaType<T>());
    Systemus::Orm::MetaTable::registerType(object->className(), object, type);
}

#endif // SYSTEMUS_METATABLE_H
