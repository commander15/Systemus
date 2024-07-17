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
    bool syncSettings(bool force = false);

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
    enum SettingType {
        Unknown = -1,
        Int = 0,
        Double = 10,
        Bool = 20,
        String = 30,
        Date = 40,
        Time = 50,
        DateTime = 60,
        ByteArray = 70
    };

    static int settingTypeFromMetatype(const QMetaType &type);
    static QMetaType metaTypeFromSettingType(int type);

    bool _online;
};

}

#endif // SYSTEMUS_SYSTEM_P_H
