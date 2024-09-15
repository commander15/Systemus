#ifndef SYSTEMUS_SYSTEM_H
#define SYSTEMUS_SYSTEM_H

#include <SystemusCore/global.h>

#include <QtCore/qobject.h>
#include <QtCore/qversionnumber.h>
#include <QtCore/qdatetime.h>

class QSettings;

namespace Systemus {

//class SystemNotification;

class SYSTEMUS_CORE_EXPORT SystemData
{
public:
    QByteArray logoData;
    QString name;
    QVersionNumber version;
    QDateTime now;
};

class SYSTEMUS_CORE_EXPORT System : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString version READ versionString WRITE setVersionString NOTIFY versionChanged)
    Q_PROPERTY(QDateTime now READ now)
    Q_PROPERTY(int heartbeatInterval READ heartbeatInterval WRITE setHeartbeatInterval)

public:
    ~System();

    QByteArray logoData() const;
    Q_SLOT void setLogoData(const QByteArray &data);
    Q_SIGNAL void logoDataChanged(const QByteArray &data);

    QString name() const;
    Q_SLOT void setName(const QString &name);
    Q_SIGNAL void nameChanged(const QString &name);

    QVersionNumber version() const;
    Q_SLOT void setVersion(const QVersionNumber &version);
    Q_SIGNAL void versionChanged(const QVersionNumber &version);

    QDate currentDate() const;
    QTime currentTime() const;
    QDateTime now() const;

    Q_SIGNAL void ready();

    int heartbeatInterval() const;
    void setHeartbeatInterval(int interval);

    Q_SLOT void sync();
    Q_SIGNAL void notify();

    //Q_SIGNAL void notificationReceived(const SystemNotification &notification);

    static System *instance();

protected:
    void timerEvent(QTimerEvent *event) override;

private:
    System(QObject *parent = nullptr);

    QString versionString() const;
    void setVersionString(const QString &version);

    QScopedPointer<SystemData> d_ptr;
    static QScopedPointer<System> s_instance;

    friend class Authenticator;
};

class SYSTEMUS_CORE_EXPORT SystemNotificationData : public QSharedData
{
public:
    int id = 0;
    QString title;
    QString text;
    QByteArray data;
    QDate date;
    QTime time;
    QString type;
};

class SYSTEMUS_CORE_EXPORT SystemNotification
{
public:
    /*
    QString title() const;
    QString text() const;
    QVariant data() const;
    QDate date() const;
    QTime time() const;
    QString type() const;
    */

private:
    QSharedDataPointer<SystemData> d_ptr;
};

}

#endif // SYSTEMUS_SYSTEM_H
