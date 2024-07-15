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

public:
    ~System();

    QString name() const;
    QVersionNumber version() const;

    bool hasSetting(const QString &name) const;
    QVariant setting(const QString &name) const;
    QVariant setting(const QString &name, const QVariant &defaultValue) const;
    void setSetting(const QString &name, const QVariant &value);

    User user() const;
    Q_SIGNAL void userChanged(const User &user);

    bool isOnline() const;
    Q_SIGNAL void online();
    Q_SIGNAL void offline();
    Q_SIGNAL void onlineStateChanged(bool online);

    int heartBeatInterval() const;
    void setHeartInterval(int interval);

    Q_SIGNAL void notify();

    //Q_SIGNAL void notificationReceived(const SystemNotification &notification);

    static System *instance();

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
