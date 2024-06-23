#ifndef SYSTEMUS_AUTHENTICATOR_P_H
#define SYSTEMUS_AUTHENTICATOR_P_H

#include <SystemusCore/authenticator.h>

#include <SystemusCore/user.h>

namespace Systemus {

class AuthenticatorPrivate
{
public:
    User user;
    AuthenticationError error;
};

}

#endif // SYSTEMUS_AUTHENTICATOR_P_H
