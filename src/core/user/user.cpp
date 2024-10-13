#include "user.h"
#include "user_p.h"

#include <SystemusCore/system.h>

#include <QtCore/qregularexpression.h>

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

void User::setPassword(const QString &password, PasswordType type)
{
    S_D(User);

    switch (type) {
    case Clear:
        d->password = d->encryptPassword(password);
        break;

    case Hashed:
        d->password = password;
        break;

    default:
        setPassword(password, d->isEncryptedPassword(password) ? Hashed : Clear);
        break;
    }
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

bool User::postUpdate()
{
    S_D(User);
    return d->profile.update();
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

bool UserPrivate::isEncryptedPassword(const QString &password)
{
    static QRegularExpression bcryptRegex(R"(^\$2[aby]\$\d{2}\$[./A-Za-z0-9]{53}$)");
    return bcryptRegex.match(password).hasMatch();
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
