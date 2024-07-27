#include "user.h"
#include "user_p.h"

#include <SystemusCore/system.h>

#include <QtCore/qcryptographichash.h>
#include <QtCore/qrandom.h>

#include <bcrypt.h>

namespace Systemus {

User::User() :
    PrivilegedData(new UserPrivate)
{
    init();
}

User::User(const User &other) :
    PrivilegedData(other, false)
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

void User::setProfile(const UserProfile &profile)
{
    S_D(User);
    d->profile = profile;
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

void User::setRole(const Role &role)
{
    S_D(User);
    d->role = role;
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

bool User::getExtras(ExtraType type)
{
    if (type == PostExtra) {
        S_D(User);
        return PrivilegedData::getExtras(type) && d->profile.get(this) && d->role.get(this) && d->groups.get(this);
    } else
        return PrivilegedData::getExtras(type);
}

bool User::insertExtras(ExtraType type)
{
    S_D(User);
    if (type == PreExtra) {
        if (PrivilegedData::insertExtras(type) && d->profile.insert(this)) {
            d->setProperty("profileId", d->profile->id());
            d->setProperty("roleId", d->role->id());
            return PrivilegedData::insertExtras(type);
        } else
            return false;
    } else if (type == PostExtra) {
        const QDateTime now = System::instance()->now();
        for (int i(0); i < d->groups.size(); ++i) {
            d->groups.setJunctionData("issueDate", i, now.date());
            d->groups.setJunctionData("issueTime", i, now.time());
        }
        return PrivilegedData::insertExtras(type) && d->groups.insert(this);
    }

    return PrivilegedData::insertExtras(type);
}

bool User::updateExtras(ExtraType type)
{
    S_D(User);
    if (type == PreExtra)
        return PrivilegedData::updateExtras(type) && d->profile.update(this);

    return PrivilegedData::updateExtras(type);
}

bool User::saveReadOnlyProperty(const QString &name, const QVariant &value)
{
    S_D(User);
    if (name == "lastLogDate")
        d->lastLogDate = value.toDate();
    else if (name == "lastLogTime")
        d->lastLogTime = value.toTime();
    else
        return PrivilegedData::saveReadOnlyProperty(name, value);

    return true;
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
    PrivilegedDataPrivate("User"),
    groups("UserGroups", { "add_date", "add_time" })
{
}

bool UserPrivate::checkPassword(const QString &input, const QString &password)
{
    return bcrypt::validatePassword(input.toStdString(), password.toStdString());
}

QString UserPrivate::encryptPassword(const QString &password)
{
    return QString::fromStdString(bcrypt::generateHash(password.toStdString(), 12));
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

bool UserPrivate::equals(const DataPrivate *o) const
{
    const UserPrivate *other = static_cast<const UserPrivate *>(o);
    return password == other->password
           && active == other->active
           && profile == other->profile
           && role == other->role
           && groups == other->groups
           && PrivilegedDataPrivate::equals(o);
}

void UserPrivate::clear()
{
    password.clear();
    active = false;
    role.clear();
    groups.clear();
    PrivilegedDataPrivate::clear();
}

UserPrivate *UserPrivate::clone() const
{
    return new UserPrivate(*this);
}

bool UserProfilePrivate::equals(const DataPrivate *o) const
{
    const UserProfilePrivate *other = static_cast<const UserProfilePrivate *>(o);
    return name == other->name
           && firstName == other->firstName
           && DefaultDataPrivate::equals(o);
}

void UserProfilePrivate::clear()
{
    name.clear();
    firstName.clear();
    DefaultDataPrivate::clear();
}

UserProfilePrivate *UserProfilePrivate::clone() const
{
    return new UserProfilePrivate(*this);
}

}
