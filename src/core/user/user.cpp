#include "user.h"
#include "user_p.h"

#include <QtCore/qcryptographichash.h>
#include <QtCore/qrandom.h>

namespace Systemus {

UserProfile::UserProfile() :
    Data(new UserProfilePrivate)
{
    init<UserProfile>();
}

UserProfile::UserProfile(const UserProfile &other) :
    Data(other)
{
}

QString UserProfile::name() const
{
    S_D(const UserProfile);
    return d->name;
}

void UserProfile::setName(const QString &name)
{
    S_D(UserProfile);
    d->name = name;
}

User::User() :
    PrivilegedData(new UserPrivate)
{
    init<User>();
}

User::User(const User &other) :
    PrivilegedData(other)
{
    init<User>();
}

QString User::password() const
{
    S_D(const User);
    return d->password;
}

void User::setPassword(const QString &password, bool encrypt)
{
    S_D(User);
    if (encrypt)
        d->password = d->encryptPassword(password);
    else
        d->password = password;
}

bool User::isActive() const
{
    S_D(const User);
    return d->active;
}

void User::setActive(bool active)
{
    S_D(User);
    d->active = active;
}

QString User::profileName() const
{
    S_D(const User);
    return d->profile.object().name();
}

UserProfile User::profile() const
{
    S_D(const User);
    return d->profile;
}

bool User::hasRole(const QString &name) const
{
    S_D(const User);
    return d->role.object().name() == name;
}

Role User::role() const
{
    S_D(const User);
    return d->role;
}

bool User::insert()
{
    S_D(User);
    d->creationDate = QDate::currentDate();
    d->creationTime = QTime::currentTime();
    return Data::insert();
}

QString UserPrivate::encryptPassword(const QString &password)
{
    QByteArray hash = password.toUtf8();
    for (int i = 0; i < 1000; ++i)
        hash = QCryptographicHash::hash(hash, QCryptographicHash::Sha256);
    return hash.toHex();
}

}
