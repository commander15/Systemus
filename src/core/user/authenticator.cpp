#include "authenticator.h"
#include "authenticator_p.h"

#include <SystemusCore/private/user_p.h>

namespace Systemus {

AuthenticationError::AuthenticationError(ErrorType error) :
    _error(error)
{
}

AuthenticationError::ErrorType AuthenticationError::error() const
{
    return _error;
}

QString AuthenticationError::errorString() const
{
    switch (_error) {
    case NoError:
        return QString();

    case BadCredentials:
        return tr("Incorrect user name / password");

    case UserNotFound:
        return tr("User not found");

    case DisabledAccount:
        return tr("User disabled");

    case UnknownError:
    default:
        return tr("Unknown error");
    }
}

Authenticator::Authenticator() :
    d_ptr(new AuthenticatorPrivate)
{
}

Authenticator::~Authenticator()
{
}

bool Authenticator::isLoggedIn() const
{
    S_D(const Authenticator);
    return d->user.id() > 0;
}

User Authenticator::loggedUser() const
{
    S_D(const Authenticator);
    return d->user;
}

bool Authenticator::logIn(const QString &name, const QString &password)
{
    if (isLoggedIn())
        logOut();

    S_D(Authenticator);

    User user;

    if (user.get("name = '" + name + '\'', false) && user.password() == UserPrivate::encryptPassword(password)) {
        user.getExtras();
        d->user = user;
        emit loggedIn(user);

        d->error = AuthenticationError::NoError;
        return true;
    } else {
        AuthenticationError::ErrorType error = AuthenticationError::UnknownError;

        if (!user.isValid())
            error = AuthenticationError::UserNotFound;
        else if (user.password() != UserPrivate::encryptPassword(password))
            error = AuthenticationError::BadCredentials;
        else if (!user.isActive())
            error = AuthenticationError::DisabledAccount;
        else
            error = AuthenticationError::UnknownError;

        d->error = error;
        emit logInError(error);
        return false;
    }
}

void Authenticator::logOut()
{
    if (!isLoggedIn())
        return;

    S_D(Authenticator);

    d->user.clear();
    emit loggedOut();
}

Authenticator *Authenticator::instance()
{
    if (!_instance)
        _instance.reset(new Authenticator());
    return _instance.get();
}

QScopedPointer<Authenticator> Authenticator::_instance;

}
