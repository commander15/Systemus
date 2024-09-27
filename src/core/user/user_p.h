#ifndef SYSTEMUS_USER_P_H
#define SYSTEMUS_USER_P_H

#include "user.h"

#include <SystemusCore/private/internaldata_p.h>

namespace Systemus {

class UserPrivate : public InternalDataPrivate
{
public:
    static bool checkPassword(const QString &input, const QString &password);
    static QString encryptPassword(const QString &password);

    UserPrivate *clone() const override;

    int dataType() const override
    { return UserDataType; }

    QString login;
    QString password;
    int status = 0;

    UserProfile profile;
    UserRole role;
};

class UserProfilePrivate : public InternalDataPrivate
{
public:
    QString name;
    QString firstName;
};

class DescriptiveUserDataPrivate : public InternalDataPrivate
{
public:
    QString name;
    QString description;
};

}

#endif // SYSTEMUS_USER_P_H
