#include "user.h"
#include "user_p.h"

#include <QtCore/qcryptographichash.h>
#include <QtCore/qrandom.h>

namespace Systemus {

UserProfile::UserProfile() :
    Data(new UserProfilePrivate)
{
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
}

User::User(const User &other) :
    PrivilegedData(other)
{
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
    return d->profile->name();
}

UserProfile User::profile() const
{
    S_D(const User);
    return d->profile;
}

bool User::hasRole(const QString &name) const
{
    S_D(const User);
    return d->role->name() == name;
}

Role User::role() const
{
    S_D(const User);
    return d->role;
}

QJsonObject User::toJsonObject() const
{
    S_D(const User);

    QJsonObject object = AuthorizationData::toJsonObject();
    object.insert("role", d->role->toJsonObject());
    return object;
}

bool User::getExtras()
{
    S_D(User);
    return d->role.get(this) && PrivilegedData::getExtras();
}

bool User::insert()
{
    S_D(User);
    d->creationDate = QDate::currentDate();
    d->creationTime = QTime::currentTime();
    return Data::insert();
}

UserPrivate::UserPrivate() :
    PrivilegedDataPrivate("User", true)
{
}

QString UserPrivate::encryptPassword(const QString &password)
{
    QByteArray hash = password.toUtf8();
    for (int i = 0; i < 1000; ++i)
        hash = QCryptographicHash::hash(hash, QCryptographicHash::Sha256);
    return hash.toHex();
}

bool UserPrivate::isPrivilegeActive(const QString &name) const
{ return role->isPrivilegeActive(name) || PrivilegedDataPrivate::isPrivilegeActive(name); }

bool UserPrivate::isPermissionActive(const QString &name) const
{ return role->isPermissionActive(name) || PrivilegedDataPrivate::isPermissionActive(name); }

bool UserPrivate::equalsTo(const DataPrivate *o) const
{
    const UserPrivate *other = static_cast<const UserPrivate *>(o);
    return password == other->password
           && active == other->active
           && profile == other->profile
           && role == other->role
           && PrivilegedDataPrivate::equalsTo(o);
}

void UserPrivate::clear()
{
    password.clear();
    active = false;
    role.clear();
    PrivilegedDataPrivate::clear();
}

bool UserProfilePrivate::equalsTo(const DataPrivate *o) const
{
    const UserProfilePrivate *other = static_cast<const UserProfilePrivate *>(o);
    return name == other->name
           && DataPrivate::equalsTo(o);
}

void UserProfilePrivate::clear()
{
    name.clear();
    DataPrivate::clear();
}

}
