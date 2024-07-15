#ifndef SYSTEMUS_SYSTEM_P_H
#define SYSTEMUS_SYSTEM_P_H

#include <SystemusCore/system.h>

#include <QtCore/qversionnumber.h>
#include <QtCore/qsettings.h>
#include <QtCore/qdatetime.h>
#include <QtCore/qtimer.h>

namespace Systemus {

class SystemPrivate : public QObject
{
    Q_OBJECT

public:
    SystemPrivate(System *q);

    bool isOnline() const;
    void setOnline(bool online);
    Q_SIGNAL void onlineStateChanged(bool online);

    Q_SLOT void update();
    bool getData();
    bool syncSettings();

    QString settingKey(const QString &name) const;

    System *q;

    QString name;
    QVersionNumber version;

    QSettings settings;
    QHash<QString, int> settingKeyIds;
    QStringList dirtySettingKeys;
    QDateTime lastSettingsSyncTime;

    QTimer timer;

private:
    bool _online;
};

}

#endif // SYSTEMUS_SYSTEM_P_H
