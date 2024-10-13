#include "system.h"
#include "system_p.h"

#include <SystemusCore/orm.h>
#include <SystemusCore/metatable.h>
#include <SystemusCore/namespace.h>
#include <SystemusCore/settings.h>
#include <SystemusCore/private/namespace_p.h>

#include <QtSql/qsqlquery.h>
#include <QtSql/qsqlrecord.h>
#include <QtSql/qsqlfield.h>
#include <QtSql/qsqldriver.h>

#include <QtCore/qcoreapplication.h>
#include <QtCore/qsettings.h>
#include <QtCore/qcoreevent.h>
#include <QtCore/qstandardpaths.h>
#include <QtCore/qdir.h>
#include <QtCore/qfile.h>
#include <QtCore/qtimer.h>

#include <xcb/xcb.h>

namespace Systemus {

System::System(QObject *parent) :
    QObject(parent),
    d_ptr(new SystemPrivate(this))
{
    S_D(System);

    QSqlDatabase db = QSqlDatabase::database(SystemusPrivate::dbConnection, false);
    if (db.isOpen()) {
        d->getProperties();
        d->getTime();
    }

    QTimer::singleShot(0, this, &System::sync);
}

System::~System()
{
}

int System::id() const
{
    return 1;
}

QByteArray System::logoData() const
{
    S_D(const System);

    QFile file(d->dir + QStringLiteral("/system/logo.png"));
    if (!file.exists()) {
        if (file.open(QIODevice::WriteOnly)) {
            file.write(d->logoData);
            file.flush();
            file.close();
        }
    }

    return d->logoData;
}

void System::setLogoData(const QByteArray &data)
{
    S_D(System);
    if (d->logoData != data) {
        d->logoData = data;
        d->markPropertyChange(SystemPrivate::LogoProperty);
    }
}

QString System::name() const
{
    return d_ptr->name;
}

void System::setName(const QString &name)
{
    S_D(System);
    if (d->name != name) {
        d->name = name;
        d->markPropertyChange(SystemPrivate::NameProperty);
    }
}

QVersionNumber System::version() const
{
    return d_ptr->version;
}

void System::setVersion(const QVersionNumber &version)
{
    S_D(System);
    if (d->version != version) {
        d->version = version;
        d->markPropertyChange(SystemPrivate::VersionProperty);
    }
}

QDate System::currentDate() const
{
    return d_ptr->now.date();
}

QTime System::currentTime() const
{
    return d_ptr->now.time();
}

QDateTime System::now() const
{
    return d_ptr->now;
}

int System::heartbeatInterval() const
{
    S_D(const System);
    return d->heartbeatInterval;
}

void System::setHeartbeatInterval(int interval)
{
    S_D(System);

    if (interval < 500)
        interval = 500;

    if (d->heartbeatInterval != interval) {
        bool startTimers = false;
        if (d->timersActive()) {
            d->stopTimers();
            startTimers = true;
        }

        d->heartbeatInterval = interval;

        if (startTimers)
            d->startTimers();
    }
}

void System::sync()
{
    S_D(System);

    if (!d->timersActive())
        d->startTimers();

    if (d->name.isEmpty()) {
        if (d->getProperties()) {
            d->startTimers();
            emit ready();
        }
    } else if (d->hasPendingPropertyChanges()) {
        QList<SystemPrivate::SystemProperty> properties;
        if (d->commitPropertyChanges(&properties)) {
            for (SystemPrivate::SystemProperty property : std::as_const(properties)) {
                switch (property) {
                case Systemus::SystemPrivate::LogoProperty:
                    emit logoDataChanged();
                    break;

                case Systemus::SystemPrivate::NameProperty:
                    emit nameChanged(d->name);
                    break;

                case Systemus::SystemPrivate::VersionProperty:
                    emit versionChanged(d->version);
                    break;
                }
            }
        }
    }

    Settings::instance()->sync();

    emit notify();
}

System *System::instance()
{
    static System system;
    return &system;
}

void System::timerEvent(QTimerEvent *event)
{
    S_D(System);
    d->processTimerEvent(event);
}

QString System::versionString() const
{
    return d_ptr->version.toString();
}

void System::setVersionString(const QString &version)
{
    S_D(System);
    d->version = QVersionNumber::fromString(version);
    d->markPropertyChange(SystemPrivate::VersionProperty);
}

SystemPrivate::SystemPrivate(System *q) :
    q_ptr(q),
    heartbeatInterval(60000),
    dir(QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/Systemus"),
    m_heartbeatTimerId(-1),
    m_nowTimerId(-1)
{
    QDir systemusDir(dir);
    if (!systemusDir.exists("system"))
        systemusDir.mkpath("system");

    qApp->installNativeEventFilter(this);
}

bool SystemPrivate::getProperties()
{
    QString statement = "SELECT {{logoData}}, {{name}}, {{version}}, CURRENT_TIMESTAMP FROM {{Systemus::System}} LIMIT 1";
    statement = Orm::formatExpression(statement, q_ptr->metaObject()->className());

    bool ok = false;
    QSqlQuery query = Systemus::exec(statement, &ok);

    if (ok && query.next()) {
        logoData = query.value(0).toByteArray();
        name = query.value(1).toString();
        version = QVersionNumber::fromString(query.value(2).toString());

        QSettings *settings = Settings::instance()->settings();
        if (settings) {
            settings->beginGroup("System");
            settings->setValue("name", name);
            settings->setValue("version", version.toString());
            settings->endGroup();
        }

        now = query.value(3).toDateTime();

        return true;
    } else {
        return false;
    }
}

bool SystemPrivate::hasPendingPropertyChanges() const
{
    return !m_dirtyProperties.isEmpty();
}

void SystemPrivate::markPropertyChange(SystemProperty property, bool changed)
{
    if (changed && !m_dirtyProperties.contains(property))
        m_dirtyProperties.append(property);
    else if (!changed && m_dirtyProperties.contains(property))
        m_dirtyProperties.removeOne(property);
}

bool SystemPrivate::commitPropertyChanges(QList<SystemProperty> *properties)
{
    QSqlRecord record = systemRecord();

    if (m_dirtyProperties.contains(LogoProperty))
        record.setValue("logo", logoData);
    else
        record.remove(record.indexOf("logo"));

    if (m_dirtyProperties.contains(NameProperty))
        record.setValue("name", name);
    else
        record.remove(record.indexOf("name"));

    if (m_dirtyProperties.contains(VersionProperty))
        record.setValue("version", version.toString());
    else
        record.remove(record.indexOf("version"));

    QString statement = Systemus::sqlStatement(QSqlDriver::UpdateStatement, systemTable(), record, false);
    bool ok = false;

    QSqlQuery query = Systemus::exec(statement + " LIMIT 1", &ok);

    if (ok && query.next()) {
        name = query.value(0).toString();
        version = QVersionNumber::fromString(query.value(1).toString());

        QSettings *settings = Settings::instance()->settings();
        if (settings) {
            settings->beginGroup("System");
            settings->setValue("name", name);
            settings->setValue("version", version.toString());
            settings->endGroup();
        }

        if (properties)
            properties->swap(m_dirtyProperties);
        else
            m_dirtyProperties.clear();
    }

    return ok;
}

bool SystemPrivate::getTime()
{
    bool ok = false;
    QSqlQuery query = Systemus::exec("SELECT CURRENT_TIMESTAMP", &ok);
    if (ok && query.next()) {
        now = query.value(0).toDateTime();
        return true;
    } else {
        return false;
    }
}

bool SystemPrivate::timersActive() const
{
    return m_heartbeatTimerId > 0 && m_nowTimerId > 0;
}

void SystemPrivate::startTimers()
{
    m_heartbeatTimerId = q_ptr->startTimer(heartbeatInterval);
    m_nowTimerId = q_ptr->startTimer(1000);
}

void SystemPrivate::stopTimers()
{
    q_ptr->killTimer(m_heartbeatTimerId);
    m_heartbeatTimerId = -1;

    q_ptr->killTimer(m_nowTimerId);
    m_nowTimerId = -1;
}

void SystemPrivate::processTimerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_heartbeatTimerId) {
        q_ptr->sync();
        event->accept();
    } else if (event->timerId() == m_nowTimerId) {
        now = now.addSecs(1);
        event->accept();
    }
}

bool SystemPrivate::nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result)
{
    Q_UNUSED(eventType);
    Q_UNUSED(message);
    Q_UNUSED(result);
    return false;
}

QString SystemPrivate::systemTable()
{
    return QStringLiteral("SSystems");
}

QSqlRecord SystemPrivate::systemRecord()
{
    QSqlRecord record;
    record.append(QSqlField("logo", QMetaType::fromType<QByteArray>()));
    record.append(QSqlField("name", QMetaType::fromType<QString>()));
    record.append(QSqlField("version", QMetaType::fromType<QString>()));
    return record;
}

}
