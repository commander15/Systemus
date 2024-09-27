#ifndef SYSTEMUS_METATABLE_H
#define SYSTEMUS_METATABLE_H

#include <SystemusCore/global.h>
#include <SystemusCore/secretproperty.h>

#include <QtCore/qshareddata.h>
#include <QtCore/qmetaobject.h>

class QMetaProperty;
class QMetaObject;

class QSqlField;
class QSqlRecord;

namespace Systemus {
namespace Orm {

class SecretProperty;

class SYSTEMUS_CORE_EXPORT MetaTableData : public QSharedData
{
public:
    int primaryPropertyIndex = 0;

    QList<int> searchPropertyIndexes;

    QList<int> metaPropertyIndexes;
    QMap<int, int> metaPropertyFieldIndexes;

    QList<int> secretPropertyIndexes;
    QMap<int, int> secretPropertyFieldIndexes;
    QVector<SecretProperty> secretProperties;

    const QMetaObject *metaObject = nullptr;
    int metaTypeId = -1;
};

class SYSTEMUS_CORE_EXPORT MetaTable
{
public:
    MetaTable();
    MetaTable(const MetaTable &other);
    MetaTable(MetaTable &&other);
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

    QStringList searchPropertyNames() const;
    QString searchPropertyName(int index) const;
    QMetaProperty searchProperty(int index) const;
    int searchCount() const;

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
    SecretProperty secretProperty(int index) const;
    QStringList secretFieldNames() const;
    QString secretFieldName(int index) const;
    QSqlField secretField(int index) const;
    int secretCount() const;
    QSqlRecord secretRecord() const;

    QStringList propertyNames() const;
    QString propertyName(int index) const;
    SecretProperty property(int index) const;
    int indexOfProperty(const QString &propertyName) const;
    QStringList fieldNames() const;
    QString fieldName(int index) const;
    QSqlField field(int index) const;
    int indexOfField(const QString &fieldName) const;
    int count() const;
    QSqlRecord record() const;

    QString foreignPropertyName() const;
    QString foreignFieldName() const;
    QSqlField foreignField() const;

    void *newData(const void *copy = nullptr) const;
    void assignData(const void *from, void *to) const;

    const QMetaObject *metaObject() const;
    QMetaType metaType() const;

    bool isValid() const;

    static bool isRegistered(const QString &name);
    template<typename T> inline static bool isRegistered();
    static MetaTable fromClassName(const QString &name);
    template<typename T> inline static MetaTable fromType();

    template<typename T> static inline void registerClass();
    static void registerClass(const QString &name, const QMetaObject *metaObject, const QMetaType &metaType);

    static void registerClassAssignmentFunction(const QString &className, const std::function<void(const void *, void *)> &function);

private:
    MetaTable(MetaTableData *d);

    QExplicitlySharedDataPointer<MetaTableData> d_ptr;

    static QHash<QString, MetaTable> s_tables;
};

template<typename T>
inline bool MetaTable::isRegistered()
{ return isRegistered(T::staticMetaObject.className()); }

template<typename T>
inline MetaTable MetaTable::fromType()
{
    const QString className = T::staticMetaObject.className();
    if (!isRegistered(className))
        registerClass<T>();
    return fromClassName(className);
}

template<typename T>
inline void MetaTable::registerClass()
{
    const QMetaObject *object(&T::staticMetaObject);
    const QMetaType type(qRegisterMetaType<T>());

    const QString className = object->className();
    registerClass(className, object, type);
    registerClassAssignmentFunction(className, [](const void *from, void *to) {
        *static_cast<T *>(to) = *static_cast<const T *>(to);
    });
}

}
}

template<typename T>
inline void sRegisterClass()
{ Systemus::Orm::MetaTable::registerClass<T>(); }

#endif // SYSTEMUS_METATABLE_H
