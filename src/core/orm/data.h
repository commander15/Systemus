#include <SystemusCore/global.h>

#include <QtSql/qsqlrecord.h>
#include <QtSql/qsqlfield.h>
#include <QtSql/qsqlerror.h>

#include <QtCore/QVariant.h>

#define SYSTEMUS_DATA(table)  SYSTEMUS_DATA1(table)

#define SYSTEMUS_DATA1(table) SYSTEMUS_DATA2(table, id, int)

#define SYSTEMUS_DATA2(table, id, type) \
    protected:
        void initData(QSqlRecord *record) override { \
            QSqlField field(#id, QMetaType::fromType<type>(), #table); \
            record->append(field); \
        } \
    private:
    
#define SYSTEMUS_PROPERTY SYSTEMUS_PROPERTY1

#define SYSTEMUS_PROPERTY2(type, name, value) \
    public: \
        inline void init##SYSTEMUS_CAPITALIZE_FIRST_LETTER(name)() \
        { initField(#name, QMetaType::fromType<type>(), value); } \
        inline type ##SYSTEMUS_CAPITALIZE_FIRST_LETTER(name)() const \
        { return m_record.value(#name).value<type>(); } \
        inline void set##SYSTEMUS_CAPITALIZE_FIRST_LETTER(name)(const type &##name) \
        { m_record.setValue(#name, name); } \
    private:
    
#define SYSTEMUS_FOREIGN SYSTEMUS_FOREIGN1

#define SYSTEMUS_FOREIGN1(name) SYSTEMUS_FOREIGN2(name, int)
    
#define SYSTEMUS_FOREIGN2(name, type) \
    protected: \
        inline void init##SYSTEMUS_CAPITALIZE_FIRST_LETTER(name)() \
        { initField(#name, QMetaType::fromType<type>(), QVariant()); } \
        inline type ##SYSTEMUS_CAPITALIZE_FIRST_LETTER(name)() const \
        { return m_record.value(#name).value<type>(); } \
        inline void set##SYSTEMUS_CAPITALIZE_FIRST_LETTER(name)(const type &##name) \
        { m_record.setValue(#name, name); } \
    private:

namespace Systemus {

class DataPrivate;
class SYSTEMUS_CORE_EXPORT Data
{
public:
    Data();
    Data(const Data &other);
    Data(Data &&other);
    virtual ~Data();
    
    Data &operator=(const Data &other);
    Data &operator=(Data &&other);
    
    QString table() const;
    
    template<typename T>
    T value(int field) const
    { return value(field).value<T>(); }
    QVariant value(int field) const;
    void setValue(int field, const QVariant &value);
    
    template<typename T>
    T value(const QString &field) const
    { return value(field).value<T>(); }
    QVariant value(const QString &field) const;
    void setValue(const QString &field, const QVariant &value);
    
    virtual void fill(const Data &other);
    virtual void clear();
    
    bool get();
    bool get(const QVariant &id);
    virtual bool get(const QString &filter);
    
    bool save();
    virtual bool insert();
    virtual bool update();
    
    virtual bool deleteData();
    
    QSqlError lastError() const;
    
protected:
    Data(DataPrivate *data);
    Data(const QSharedDataPointer<DataPrivate> &other);
    
    virtual void initData(QSqlRecord *record);
    
    bool isFieldInitialized(const QString &name) const;
    void initField(const QString &name, const QMetaType &type, const QVariant &value);

    virtual void setTable(const QString &table);
    
    void registerField(const QString name, const QMetaType &type);
    virtual void registerField(const QSqlField &field);

    QSqlRecord m_record;
    
private:
    QSharedDataPointer<DataPrivate> d;
};

}
