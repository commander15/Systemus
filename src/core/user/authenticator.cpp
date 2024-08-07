#include "authenticator.h"
#include "authenticator_p.h"

#include <SystemusCore/system.h>
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
    System *system = System::instance();
    connect(this, &Authenticator::loggedIn, system, &System::setUser);
    connect(this, &Authenticator::loggedOut, system, [system] {
        system->setUser(User());
    });
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

    bool ok;
    QSqlQuery query = Data::execQuery("SELECT id, active, password FROM Users WHERE name = '" + name + "'", &ok);

    if (!ok) {
        d->error = AuthenticationError::UnknownError;
        emit logInError(d->error);
        return false;
    }

    if (!query.next()) {
        d->error = AuthenticationError::UserNotFound;
        emit logInError(d->error);
        return false;
    }

    if (!query.value(1).toBool()) {
        d->error = AuthenticationError::DisabledAccount;
        emit logInError(d->error);
        return false;
    }

    if (!UserPrivate::checkPassword(password, query.value(2).toString())) {
        d->error = AuthenticationError::BadCredentials;
        emit logInError(d->error);
        return false;
    }

    d->user.get(query.value(0).toInt());
    emit loggedIn(d->user);
    return true;
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
