namespace Systemus {

class SecureDataPrivate : public DataPrivate
{
public:
    QDate creationDate;
    QTime creationTime;
    
    QVector<Privileges> privileges;
    QVector<Permission> permissions;
};

class CommonDataPrivate : public SecureDataPrivate
{
public:
    QString name;
    QString description;
};

}
