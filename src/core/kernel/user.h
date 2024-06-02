#include <SystemusCore/global.h>
#include <SystemusCore/profile.h>
#include <SystemusCore/role.h>
#include <SystemusCore/group.h>
#include <SystemusCore/privilege.h>
#include <SystemusCore/permission.h>

class UserPrivate;
class SYSTEMUS_CORE_EXPORT User : public Systemus::Data
{
    SYSTEMUS_DATA(Users)
    SYSTEMUS_PROPERTY(QString, login)
    SYSTEMUS_PROPERTY(QString, password)
    SYSTEMUS_PROPERTY(bool, active)
    SYSTEMUS_FOREIGN(profile_id)
    SYSTEMUS_FOREIGN(role_id)
    
public:
    User();
    ~User();
    
    QString name() const;
    void setName(const QString &name);
    
    QString firstName() const;
    void setFirstName(const QString &name);
    
    Profile::Sex sex() const;
    void setSex(Profile::Sex sex);
    
    QDate birthdate() const;
    void setBirthdate(const QDate &date);
    
    QString phone() const;
    void setPhone(const QString &phone);
    
    QString email() const;
    void setEmail(const QString &email);
    
    Profile profile() const;
    void setProfile(const Profile &profile);
    
    QString roleName() const;
    Role role() const;
    void setRole(const Role &role);
    
    bool inGroup(const QString &group) const;
    QList<Group> groups() const;
    void setGroups(const QList<Group> &groups);
       
    bool hasPrivilege(const QString &name) const;
    QList<Privilege> privileges() const;
    
    bool hasPermission(const QString &name) const;
    QList<Permission> permissions() const;
    
    void fill(const User &user);
    void clear() override;
    
    bool get(const QString &filter) override;
    bool insert() override;
    bool update() override;
    bool deleteData() override;
};
