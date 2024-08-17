#ifndef SYSTEMUS_SYSTEM_H
#define SYSTEMUS_SYSTEM_H

#include <SystemusCore/global.h>

#include <QtCore/qobject.h>

class QVersionNumber;

namespace Systemus {

class User;
//class SystemNotification;

class SystemPrivate;
class SYSTEMUS_CORE_EXPORT System : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name NOTIFY online)
    Q_PROPERTY(QByteArray logoData READ logoData NOTIFY online)
    Q_PROPERTY(QString version READ versionString NOTIFY online)
    Q_PROPERTY(QDateTime now READ now)
    Q_PROPERTY(bool online READ isOnline NOTIFY onlineStateChanged)
    Q_PROPERTY(int heartbeatInterval READ heartbeatInterval WRITE setHeartbeatInterval)

public:
    ~System();

    QString name() const;

    QByteArray logoData() const;
    bool setLogoData(const QByteArray &data);

    QVersionNumber version() const;
    QString versionString() const;

    QDate currentDate() const;
    QTime currentTime() const;
    QDateTime now() const;

    bool hasSetting(const QString &name) const;
    QVariant setting(const QString &name) const;
    QVariant setting(const QString &name, const QVariant &defaultValue) const;
    void setSetting(const QString &name, const QVariant &value);
    //void unsetSettings(const QString &name);

    User user() const;
    Q_SIGNAL void userChanged(const User &user);

    bool isOnline() const;
    Q_SIGNAL void online();
    Q_SIGNAL void offline();
    Q_SIGNAL void onlineStateChanged(bool online);

    int heartbeatInterval() const;
    void setHeartbeatInterval(int interval);

    Q_SLOT void sync();
    Q_SIGNAL void notify();

    bool isDatabaseOpen() const;
    bool openDatabase(const QString &driver) const;
    bool openDatabase(const QString &connection, const QString &driver) const;
    void closeDatabase();

    void restoreDatabaseSettings() const;
    void restoreDatabaseSettings(const QString &connection) const;
    void saveDatabaseSettings();
    void saveDatabaseSettings(const QString &connection);

    //Q_SIGNAL void notificationReceived(const SystemNotification &notification);

    static System *instance();

protected:
    void timerEvent(QTimerEvent *event) override;

private:
    System(QObject *parent = nullptr);

    Q_SLOT void setUser(const User &user);

    QScopedPointer<SystemPrivate> d;
    static QScopedPointer<System> _instance;

    friend class Authenticator;
};

/*class SystemNotification;
class SYSTEMUS_CORE_EXPORT SystemNotification
{
public:
    QString title() const;
    QString text() const;
    QVariant data() const;
    QString type() const;
    QDate date() const;
    QTime time() const;
};*/

}

#endif // SYSTEMUS_SYSTEM_H
