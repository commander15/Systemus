namespace Systemus {

class UserPrivate : public SecureDataPrivate
{
public:
    int id;
    QString login;
    QString password;
    bool active;
    
    QDate loggedDate;
    QTime loggedTime;
    
    Profile profile;
    Role role;
    QVector<Group> groups;
};

}
