namespace Systemus {

class ProfilePrivate : public DataPrivate
{
public:
    int id;
    QString name;
    QString firstName;
    Profile::Sex sex;
    QDate birthdate;
    QString phone;
    QString email;
};

}
