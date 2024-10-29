#ifndef SYSTEMUS_CONTROLGATE_H
#define SYSTEMUS_CONTROLGATE_H

#include <SystemusCore/global.h>

#include <QtCore/qstring.h>
#include <QtCore/qscopedpointer.h>

namespace Systemus {

class ControlGateAgent;
class User;

class SYSTEMUS_CORE_EXPORT ControlGate
{
public:
    static bool hasPrivilege(const QString &name);
    static bool hasPermission(const QString &name);

    static ControlGateAgent *agent();
    static void setAgent(ControlGateAgent *agent);

private:
    static User user();

    static QScopedPointer<ControlGateAgent> s_agent;
};

class SYSTEMUS_CORE_EXPORT ControlGateAgent
{
public:
    virtual ~ControlGateAgent();

    virtual bool hasPrivilege(const QString &name, const User &user) const = 0;
    virtual bool hasPermission(const QString &name, const User &user) const = 0;
};

}

#endif // SYSTEMUS_CONTROLGATE_H
