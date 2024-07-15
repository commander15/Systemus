#include "user.h"
#include "user_p.h"

#include <QtCore/qcryptographichash.h>
#include <QtCore/qrandom.h>

namespace Systemus {

User::User() :
    PrivilegedData(new UserPrivate)
{
    init();
}

User::User(const User &other) :
    PrivilegedData(other, true)
{
}

User &User::operator=(const User &other)
{
    PrivilegedData::operator=(other);
    return *this;
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

bool User::inGroup(const QString &name) const
{
    S_D(const User);
    for (const Group &group : d->groups)
        if (group.name() == name)
            return true;
    return false;
}

QList<Group> User::groups() const
{
    S_D(const User);
    return d->groups;
}

bool User::getExtras()
{
    S_D(User);
    return PrivilegedData::getExtras() && d->profile.get(this) && d->role.get(this) && d->groups.get(this);
}

UserProfile::UserProfile() :
    Data(new UserProfilePrivate)
{
    init();
}

UserProfile::UserProfile(const UserProfile &other) :
    Data(other, false)
{
}

UserProfile &UserProfile::operator=(const UserProfile &other)
{
    Data::operator=(other);
    return *this;
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

QString UserProfile::firstName() const
{
    S_D(const UserProfile);
    return d->firstName;
}

void UserProfile::setFirstName(const QString &name)
{
    S_D(UserProfile);
    d->firstName = name;
}

QString UserProfile::fullName() const
{
    S_D(const UserProfile);
    return d->name + (!d->firstName.isEmpty() ? ' ' + d->firstName : QString());
}

UserPrivate::UserPrivate() :
    PrivilegedDataPrivate("User", true),
    groups("UserGroups", { "add_date", "add_time" })
{
}

QString UserPrivate::encryptPassword(const QString &password)
{
    QByteArray hash = password.toUtf8();
    for (int i = 0; i < 1000; ++i)
        hash = QCryptographicHash::hash(hash, QCryptographicHash::Sha256);
    return hash.toHex();
}

bool UserPrivate::hasPrivilege(const QString &name) const
{
    if (PrivilegedDataPrivate::hasPrivilege(name))
        return true;
    else if (role->hasPrivilege(name))
        return true;
    else {
        for (const Group &group : groups)
            if (group.hasPrivilege(name))
                return true;
        return false;
    }
}

bool UserPrivate::hasPermission(const QString &name) const
{
    if (PrivilegedDataPrivate::hasPermission(name))
        return true;
    else if (role->hasPermission(name))
        return true;
    else {
        for (const Group &group : groups)
            if (group.hasPermission(name))
                return true;
        return false;
    }
}

QVariant UserPrivate::property(const QString &name) const
{
    if (name == "password")
        return password;
    else if (name == "active")
        return active;
    else
        return PrivilegedDataPrivate::property(name);
}

void UserPrivate::setProperty(const QString &name, const QVariant &value)
{
    if (name == "password")
        password = value.toString();
    else if (name == "active")
        active = value.toBool();
    else
        PrivilegedDataPrivate::setProperty(name, value);
}

bool UserPrivate::equalsTo(const DataPrivate *o) const
{
    const UserPrivate *other = static_cast<const UserPrivate *>(o);
    return password == other->password
           && active == other->active
           && profile == other->profile
           && role == other->role
           && groups == other->groups
           && PrivilegedDataPrivate::equalsTo(o);
}

void UserPrivate::clear()
{
    password.clear();
    active = false;
    role.clear();
    groups.clear();
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
