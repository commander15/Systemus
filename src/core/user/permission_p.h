namespace Systemus {

class PermissionPrivate : public CommonDataPrivate
{
public:
    int id;
};

class IssuedPermissionPrivate : public PermissionPrivate
{
public:
    bool active;
    QDate issueDate;
    QTime issueTime;
    
    int permissionId;
};

}
