namespace Systemus {

class DataPrivate : public QSharedData
{
public:
    enum DataType {
        DefaultData,
        SimpleData,
        SecureData,
        CommonData
    };

    QSqlRecord record() const;
    void setRecord(const QSqlRecord &record);

protected:
    QString selectStatement() const;
    QString insertStatement() const;
    QString updateStatement() const;
    QString deleteStatement() const;
    
    virtual QString tableName() const = 0;
    virtual QSqlRecord createRecord() const = 0;
    virtual void fillRecord(QSqlRecord *record) = 0;
    virtual void extractRecord(const QSqlRecord &record) = 0;
    
    virtual DataType dataType() const
    { return DefaultData; }
};

class SimpleDataPrivate : public DataPrivate
{
public:
    inline QString tableName() const override
    { return m_tableName; }
    
    inline QSqlRecord createRecord() const override
    { return m_tableRecord; }
    
    inline void fillRecord(QSqlRecord *record) override
    { *record = m_record; }
    
    inline void extractRecord(const QSqlRecord &record) override
    { m_record = record; }
    
    inline DataType dataType() const override
    { return SimpleData; }
    
    QSqlRecord m_record;
    
    QString m_tableName;
    QSqlRecord m_tableRecord;
};

}
