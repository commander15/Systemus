#ifndef SYSTEMUS_SYSTEM_H
#define SYSTEMUS_SYSTEM_H

#include <SystemusCore/global.h>
#include <SystemusCore/data.h>

#include <QtCore/qobject.h>
#include <QtCore/qversionnumber.h>
#include <QtCore/qdatetime.h>

class QSettings;

namespace Systemus {

//class SystemNotification;

class SYSTEMUS_CORE_EXPORT SystemData
{
public:
    SystemData() = default;
    virtual ~SystemData() = default;

    QByteArray logoData;
    QString name;
    QVersionNumber version;
    QDateTime now;
};

class SYSTEMUS_CORE_EXPORT System : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QByteArray logoData READ logoData WRITE setLogoData NOTIFY logoDataChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged USER true)
    Q_PROPERTY(QString version READ versionString WRITE setVersionString NOTIFY versionChanged)
    Q_PROPERTY(QDateTime now READ now STORED false)
    Q_PROPERTY(int heartbeatInterval READ heartbeatInterval WRITE setHeartbeatInterval STORED false)
    Q_CLASSINFO("logoDataField", "logo")

public:
    ~System();

    int id() const;

    QByteArray logoData() const;
    Q_SLOT void setLogoData(const QByteArray &data);
    Q_SIGNAL void logoDataChanged();

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

    //void postNotification(const SystemNotification &notification);
    //Q_SIGNAL void notificationPosted(const SystemNotification &notification);
    //Q_SIGNAL void notificationReceived(const SystemNotification &notification);

    static System *instance();

protected:
    void timerEvent(QTimerEvent *event) override;

private:
    System(QObject *parent = nullptr);

    QString versionString() const;
    void setVersionString(const QString &version);

    QScopedPointer<SystemData> d_ptr;

    friend class Authenticator;
};

// ToDo: move on dedicated files
class SYSTEMUS_CORE_EXPORT SystemNotificationData : public QSharedData
{
public:
    SystemNotificationData() = default;
    virtual ~SystemNotificationData() = default;

    int id = 0;
    QString title;
    QString text;
    QByteArray data;
    QDate date;
    QTime time;
    QString type;
};

class SYSTEMUS_CORE_EXPORT SystemNotification /* public Orm::Data */
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
