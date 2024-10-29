#include "controlgate.h"
#include "controlgate_p.h"

#include <SystemusCore/authenticator.h>
#include <SystemusCore/user.h>
#include <SystemusCore/private/debug_p.h>

#include <QtCore/qfile.h>
#include <QtCore/qjsondocument.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>

namespace Systemus {

bool ControlGate::hasPrivilege(const QString &name)
{
    return agent()->hasPrivilege(name, user());
}

bool ControlGate::hasPermission(const QString &name)
{
    return agent()->hasPermission(name, user());
}

ControlGateAgent *ControlGate::agent()
{
    if (!s_agent) {
        if (RoleBasedControlGateAgent::isSupported())
            s_agent.reset(new RoleBasedControlGateAgent());
        else
            s_agent.reset(new DefaultControlAgent());
    }
    return s_agent.get();
}

void ControlGate::setAgent(ControlGateAgent *agent)
{
    s_agent.reset(agent);
}

User ControlGate::user()
{
    return Authenticator::instance()->loggedUser();
}

QScopedPointer<ControlGateAgent> ControlGate::s_agent;

QScopedPointer<ControlGateAgent> s_agent;

ControlGateAgent::~ControlGateAgent()
{
}

RoleBasedControlGateAgent::RoleBasedControlGateAgent()
{
    const QString fileName = configFileName();
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        file.open(QIODevice::ReadOnly|QIODevice::Text);
        loadDataFromDevice(file.fileName(), &file);
    }
}

bool RoleBasedControlGateAgent::hasPrivilege(const QString &name, const User &user) const
{
    if (m_rolePrivileges.contains(user.role().name(), name))
        return true;
    else
        return DefaultControlAgent::hasPrivilege(name, user);
}

bool RoleBasedControlGateAgent::hasPermission(const QString &name, const User &user) const
{
    if (m_rolePermissions.contains(user.role().name(), name))
        return true;
    else
        return DefaultControlAgent::hasPermission(name, user);
}

bool RoleBasedControlGateAgent::isSupported()
{
    return !configFileName().isEmpty();
}

QString RoleBasedControlGateAgent::configFileName()
{
    const QStringList fileNames = { "rbac", "acf" };
    for (const QString &fileName : fileNames) {
        const QString fullName = ":/systemus/" + fileName + ".json";
        if (QFile::exists(fullName))
            return fullName;
    }
    return QString();
}

bool RoleBasedControlGateAgent::loadDataFromDevice(const QString &fileName, QIODevice *device)
{
    if (!device->isOpen())
        return false;

    QJsonParseError parseError;

    const QJsonDocument doc = QJsonDocument::fromJson(device->readAll(), &parseError);
    if (parseError.error == QJsonParseError::NoError) {
        const QJsonObject root = doc.object();
        const QStringList &roles = root.keys();
        for (const QString &role : roles) {
            const QJsonValue value = root.value(role);
            if (value.isObject()) {
                const QJsonObject object = value.toObject();
                readRolePrivileges(role, object.value("privileges").toArray());
                readRolePermissions(role, object.value("permissions").toArray());
            } else if (value.isArray())
                readRolePermissions(role, value.toArray());
        }

        return true;
    } else {
        systemusWarning() << "ControlGate: json parse error \"" << parseError.errorString() << '"'
                          << ", please check " << fileName << " file";
        return false;
    }
}

void RoleBasedControlGateAgent::readRolePrivileges(const QString &role, const QJsonArray &privilegeArray)
{
    for (const QJsonValue &privilegeValue : privilegeArray) {
        if (privilegeValue.isString())
            m_rolePrivileges.insert(role, privilegeValue.toString());
        else if (privilegeValue.isObject()) {
            const QJsonObject object = privilegeValue.toObject();
            m_rolePrivileges.insert(role, object.value("name").toString());
            readRolePermissions(role, object.value("permissions").toArray());
        }
    }
}

void RoleBasedControlGateAgent::readRolePermissions(const QString &role, const QJsonArray &permissionArray)
{
    for (const QJsonValue &permissionValue : permissionArray)
        m_rolePermissions.insert(role, permissionValue.toString());
}

bool DefaultControlAgent::hasPrivilege(const QString &name, const User &user) const
{
    // ToDo: use database retrieved data to determine !
    return false;
}

bool DefaultControlAgent::hasPermission(const QString &name, const User &user) const
{
    // ToDo: use database retrieved data to determine !
    return false;
}

}
