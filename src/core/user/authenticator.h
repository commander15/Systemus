#ifndef SYSTEMUS_AUTHENTICATOR_H
#define SYSTEMUS_AUTHENTICATOR_H

#include <SystemusCore/global.h>

#include <QtCore/qobject.h>
#include <QtCore/qcoreapplication.h>

namespace Systemus {

class User;

class SYSTEMUS_CORE_EXPORT AuthenticationError
{
    Q_DECLARE_TR_FUNCTIONS(Authenticator)

public:
    enum ErrorType {
        NoError,
        BadCredentials,
        UserNotFound,
        DisabledAccount,
        UnknownError
    };

    AuthenticationError(ErrorType error = NoError);

    ErrorType error() const;
    QString errorString() const;

private:
    ErrorType _error;
};

class AuthenticatorPrivate;
class SYSTEMUS_CORE_EXPORT Authenticator : public QObject
{
    Q_OBJECT

public:
    ~Authenticator();

    bool isLoggedIn() const;
    User loggedUser() const;
    Q_SLOT void logIn(const QString &name, const QString &password);
    Q_SLOT void logOut();
    Q_SIGNAL void loggedIn(const User &user);
    Q_SIGNAL void loggedOut();
    Q_SIGNAL void logInError(const AuthenticationError &error);

    static Authenticator *instance();

private:
    Authenticator();

    QScopedPointer<AuthenticatorPrivate> d_ptr;

    static QScopedPointer<Authenticator> _instance;
};

}

#endif // SYSTEMUS_AUTHENTICATOR_H
