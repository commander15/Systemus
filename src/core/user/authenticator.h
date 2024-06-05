#include <SystemusCore/global.h>
#include <SystemusCore/user.h>

namespace Systemus {

class SYSTEMUS_CORE_EXPORT AuthenticationError
{
public:
    enum Error {
        BadCredentials,
        UserDisabled,
        UnknownError
    };
    
    AuthenticationError(Error error);
    AuthenticationError(const AuthenticationError &other);
    
    AuthenticationError &operator=(const AuthenticationError &other);
    
    Error error() const;
    QString errorString() const;
    
private:
    Error m_error;
};

class AuthenticatorPrivate;
class SYSTEMUS_CORE_EXPORT Authenticator : public QObject
{
    Q_OBJECT

public:
    ~Authenticator();
    
    User loggedUser() const;
    Q_SIGNAL void loggedUserChanged(const User &user);
    
    bool logIn(const QString &login, const QString &password);
    void logOut();
    Q_SIGNAL void loggedIn();
    Q_SIGNAL void loggedOut();
    Q_SIGNAL void logInError(const AuthenticationError &error);
    
    AuthenticationError lastError() const;
    
    static Authenticator *instance();
    
private:
    QScopedPointer<AuthenticatorPrivate> d;

    static QScopedPointer<Authenticator> m_instance;
    
    friend class System;
};

}
