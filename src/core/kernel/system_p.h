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

    QByteArray logoData;

    QString name;
    QVersionNumber version;

    QDateTime now;
    int nowTimerId;

    QString dir;

    QSettings settings;
    QHash<QString, int> settingKeyIds;
    QStringList dirtySettingKeys;

    int heartbeatInterval;
    int heartbeatTimerId;

private:
    enum SettingType {
        Unknown = -1,
        Int = 10,
        Double = 20,
        Bool = 30,
        String = 40,
        Date = 50,
        Time = 60,
        DateTime = 70,
        ByteArray = 80
    };

    static int settingTypeFromMetatype(const QMetaType &type);
    static QMetaType metaTypeFromSettingType(int type);

    bool _online;
};

}

#endif // SYSTEMUS_SYSTEM_P_H
