#ifndef SYSTEMUS_CONTROLGATE_P_H
#define SYSTEMUS_CONTROLGATE_P_H

#include "controlgate.h"

#include <QtCore/qhash.h>
#include <QtCore/qmap.h>

class QIODevice;
class QJsonObject;
class QJsonArray;

namespace Systemus {

class DefaultControlAgent : public ControlGateAgent
{
public:
    virtual ~DefaultControlAgent() = default;

    bool hasPrivilege(const QString &name, const User &user) const override;
    bool hasPermission(const QString &name, const User &user) const override;
};

class RoleBasedControlGateAgent : public DefaultControlAgent
{
public:
    RoleBasedControlGateAgent();

    bool hasPrivilege(const QString &name, const User &user) const override;
    bool hasPermission(const QString &name, const User &user) const override;

    static bool isSupported();
    static QString configFileName();

private:
    bool loadDataFromDevice(const QString &fileName, QIODevice *device);
    void readRolePrivileges(const QString &role, const QJsonArray &privilegeArray);
    void readRolePermissions(const QString &role, const QJsonArray &permissionArray);

    QMultiHash<QString, QString> m_rolePrivileges;
    QMultiHash<QString, QString> m_rolePermissions;
};

}

#endif // SYSTEMUS_CONTROLGATE_P_H
