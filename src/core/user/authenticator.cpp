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
        return tr("Incorrect user login / password");

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

bool Authenticator::logIn(const QString &login, const QString &password)
{
    if (isLoggedIn())
        logOut();

    S_D(Authenticator);

    bool ok;
    QSqlQuery query = exec(selectStatement(login), &ok);

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

    if (query.value(1).toInt() != 1) {
        d->error = AuthenticationError::DisabledAccount;
        emit logInError(d->error);
        return false;
    }

    if (!UserPrivate::checkPassword(password, query.value(2).toString())) {
        d->error = AuthenticationError::BadCredentials;
        emit logInError(d->error);
        return false;
    }

    d->user.getByPrimary(query.value(0));
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
    if (!s_instance)
        s_instance.reset(new Authenticator());
    return s_instance.get();
}

QString Authenticator::selectStatement(const QString &login)
{
    const QString statement = "SELECT {{id}}, {{status}}, {{password}} FROM {{Systemus::User}} WHERE {{login}} = '%1'";
    return Orm::formatExpression(statement.arg(login), "Systemus::User");
}

QScopedPointer<Authenticator> Authenticator::s_instance;

}
