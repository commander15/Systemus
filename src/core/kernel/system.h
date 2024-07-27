#ifndef SYSTEMUS_SYSTEM_H
#define SYSTEMUS_SYSTEM_H

#include <SystemusCore/global.h>

#ifdef QT_GUI_LIB
#   include <QtGui/qimage.h>
#endif

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

#ifdef QT_GUI_LIB
    inline QImage logo() const;
#endif
    QString name() const;
    QVersionNumber version() const;

    QDate currentDate() const;
    QTime currentTime() const;
    QDateTime now() const;

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
    void setHeartBeatInterval(int interval);

    Q_SLOT void sync();
    Q_SIGNAL void notify();

    //Q_SIGNAL void notificationReceived(const SystemNotification &notification);

    static System *instance();

protected:
    void timerEvent(QTimerEvent *event) override;

private:
    System(QObject *parent = nullptr);

    bool isLogoAvailable() const;
    QString logoFileName() const;
    QByteArray logoData();

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

#ifdef QT_GUI_LIB

QImage System::logo() const
{
    QImage image;
    if (isLogoAvailable()) {
        image.load(logoFileName());
    } else {
        image.loadFromData(_instance->logoData(), "PNG");
        image.save(logoFileName(), "PNG");
    }
    return image;
}

#endif

}

#endif // SYSTEMUS_SYSTEM_H
