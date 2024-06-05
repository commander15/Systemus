#include <SystemusCore/global.h>
#include <SystemusCore/user.h>

#include <QtCore/qobject.h>
#include <QtCore/qversionnumber.h>
#include <QtCore/qdatetime.h>

class QSqlDatabase;

namespace Systemus {

class SYSTEMUS_CORE_EXPORT System : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name             READ name             WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString version          READ versionString                  NOTIFY versionChanged)
    Q_PROPERTY(QDate   installationDate READ installationDate               NOTIFY versionChanged)
    Q_PROPERTY(QTime   installationTime READ installationTime               NOTIFY versionChanged)
    Q_PROPERTY(User    user             READ user                           NOTIFY userChanged)

public:
    ~Systemus();
    
    QString name() const;
    Q_SLOT void setName(const QString &name);
    Q_SIGNAL void nameChanged(const QString &name);
    
    int versionCode() const;
    QString versionString() const;
    QVersionNumber version() const;
    Q_SIGNAL void versionChanged(const QVersionNumber &version);
    
    QDate installationDate() const;
    QDate installationTime() const;
    
    User user() const;
    Q_SIGNAL void userChanged(const User &user);
    
    QSqlDatabase database() const;
    void setDatabase(const QSqlDatabase &db);
    void setDatabase(const QString &name);
    
    void loadSettings(QSetting &setings);
    void saveSettings(QSettings *settings);
    
    static System *instance();

private:
    System();

    static QScopedPointer<System> m_instance;
};

}
