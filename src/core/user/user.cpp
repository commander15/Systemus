#include "user.h"
#include "user_p.h"

#include <SystemusCore/system.h>

#include <QtCore/qcryptographichash.h>
#include <QtCore/qrandom.h>

#include <bcrypt.h>

namespace Systemus {

User::User() :
    InternalData(new UserPrivate)
{
}

QString User::login() const
{
    S_D(const User);
    return d->login;
}

void User::setLogin(const QString &login)
{
    S_D(User);
    d->login = login;
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

int User::status() const
{
    S_D(const User);
    return d->status;
}

void User::setStatus(int status)
{
    S_D(User);
    d->status = status;
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
    return d->role.name() == name;
}

UserRole User::role() const
{
    S_D(const User);
    return d->role;
}

void User::setRole(const UserRole &role)
{
    S_D(User);
    d->role = role;
}

bool User::postGet()
{
    S_D(User);
    d->profile.getByFilter(defaultFilter());
    return d->role.getByPrimary(readProperty("roleId"));
}

User User::fromLogin(const QString &login)
{
    return fromFilter("{{ login }} = " + formatValue(login));
}

UserProfile::UserProfile() :
    InternalData(new UserProfilePrivate)
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

DescriptiveUserData::DescriptiveUserData() :
    InternalData(new DescriptiveUserDataPrivate)
{
}

QString DescriptiveUserData::name() const
{
    S_D(const DescriptiveUserData);
    return d->name;
}

void DescriptiveUserData::setName(const QString &name)
{
    S_D(DescriptiveUserData);
    d->name = name;
}

QString DescriptiveUserData::description() const
{
    S_D(const DescriptiveUserData);
    return d->description;
}

void DescriptiveUserData::setDescription(const QString &desc)
{
    S_D(DescriptiveUserData);
    d->description = desc;
}

bool UserPrivate::checkPassword(const QString &input, const QString &password)
{
    return bcrypt::validatePassword(input.toStdString(), password.toStdString());
}

QString UserPrivate::encryptPassword(const QString &password)
{
    return QString::fromStdString(bcrypt::generateHash(password.toStdString(), 12));
}

UserPrivate *UserPrivate::clone() const
{
    return new UserPrivate(*this);
}

}
