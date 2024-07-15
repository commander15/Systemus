#include "system.h"
#include "system_p.h"

#include <SystemusCore/authenticator.h>
#include <SystemusCore/data.h>
#include <SystemusCore/user.h>

#include <QtSql/qsqlfield.h>
#include <QtSql/qsqldriver.h>

namespace Systemus {

System::System(QObject *parent) :
    QObject(parent),
    d(new SystemPrivate(this))
{
    connect(d.get(), &SystemPrivate::onlineStateChanged, this, [this](bool online) {
        emit onlineStateChanged(online);
        if (online)
            emit this->online();
        else
            emit this->offline();
    });

    d->getData();
    d->syncSettings();

    _s_register_internal_types();
}

System::~System()
{
}

QString System::name() const
{
    return d->name;
}

QVersionNumber System::version() const
{
    return d->version;
}

bool System::hasSetting(const QString &name) const
{
    return d->settings.contains(d->settingKey(name));
}

QVariant System::setting(const QString &name) const
{
    return d->settings.value(d->settingKey(name));
}

QVariant System::setting(const QString &name, const QVariant &defaultValue) const
{
    return d->settings.value(d->settingKey(name), defaultValue);
}

void System::setSetting(const QString &name, const QVariant &value)
{
    if (!d->settings.contains(d->settingKey(name)) || d->settings.value(d->settingKey(name)) != value) {
        d->settings.setValue(d->settingKey(name), value);
        d->dirtySettingKeys.append(name);
    }
}

User System::user() const
{
    return Authenticator::instance()->loggedUser();
}

bool System::isOnline() const
{
    return d->isOnline();
}

int System::heartBeatInterval() const
{
    return d->timer.interval();
}

void System::setHeartInterval(int interval)
{
    d->timer.setInterval(interval);
}

System *System::instance()
{
    if (!_instance)
        _instance.reset(new System());
    return _instance.get();
}

void System::setUser(const User &user)
{
    emit userChanged(user);
}

QScopedPointer<System> System::_instance;

SystemPrivate::SystemPrivate(System *q) :
    q(q),
#ifdef QT_DEBUG
    settings("systemus.ini", QSettings::IniFormat),
#endif
    _online(false)
{
    settings.beginGroup("System");
    name = settings.value("name").toString();
    version = QVersionNumber::fromString(settings.value("version").toString());
    settings.endGroup();

    timer.setInterval(3000);
    connect(&timer, &QTimer::timeout, this, &SystemPrivate::update);

    QTimer::singleShot(0, this, [=] { setOnline(!name.isEmpty()); });
}

bool SystemPrivate::isOnline() const
{
    return _online;
}

void SystemPrivate::setOnline(bool online)
{
    if (_online != online) {
        _online = online;
        emit onlineStateChanged(online);
    }
}

void SystemPrivate::update()
{
    setOnline(getData());

    if (_online) {
        QDateTime now = QDateTime::currentDateTime();
        if ((!dirtySettingKeys.isEmpty() && lastSettingsSyncTime.msecsTo(now) > 60000) || (lastSettingsSyncTime.msecsTo(now) > 1800000)) {
            // Sync settings after 60 seconds if there are local changes, every 30 minutes if there are not !
            syncSettings();
            lastSettingsSyncTime = now.addSecs(5);
        }
    }
}

bool SystemPrivate::getData()
{
    bool ok;
    QSqlQuery query = Data::execQuery("SELECT id, name, version FROM Systems ORDER BY id DESC LIMIT 1", &ok);

    if (ok && query.next()) {
        name = query.value("name").toString();
        version = QVersionNumber::fromString(query.value("version").toString());

        settings.beginGroup("System");
        settings.setValue("name", name);
        settings.setValue("version", version.toString());
        settings.endGroup();
    }

    return ok;
}

bool SystemPrivate::syncSettings()
{
    const QString table = QStringLiteral("SystemSettings");
    QSqlRecord record;
    record.append(QSqlField("id", QMetaType::fromType<int>(), table));
    record.append(QSqlField("name", QMetaType::fromType<QString>(), table));
    record.append(QSqlField("value", QMetaType::fromType<QString>(), table));
    record.append(QSqlField("type", QMetaType::fromType<int>(), table));

    bool ok = false;

    settings.beginGroup(qApp->applicationName());

    if (!dirtySettingKeys.isEmpty()) {
        Data::beginTransaction();

        for (const QString &name : dirtySettingKeys) {
            record.setValue(1, name);
            record.setValue(2, settings.value(name).toString());
            record.setValue(3, settings.value(name).typeName());

            QString statement;

            if (!settingKeyIds.contains(name)) {
                statement = Data::driver()->sqlStatement(QSqlDriver::InsertStatement, table, record, false);
                QSqlQuery query = Data::execQuery(statement, &ok);

                if (ok)
                    settingKeyIds.insert(name, query.lastInsertId().toInt());
            } else {
                statement = Data::driver()->sqlStatement(QSqlDriver::UpdateStatement, table, record, false);
                statement.append(" WHERE id = " + QString::number(settingKeyIds.value(name)));
                Data::execQuery(statement, &ok);
            }

            if (!ok)
                Data::rollbackTransaction();
        }

        Data::commitTransaction();
        dirtySettingKeys.clear();
    } else {
        QString statement = Data::driver()->sqlStatement(QSqlDriver::SelectStatement, table, record, false);

        QSqlQuery query = Data::execQuery(statement, &ok);
        if (ok) {
            settingKeyIds.clear();
            while (query.next()) {
                settingKeyIds.insert(query.value(1).toString(), query.value(0).toInt());
                QVariant value = query.value(2);
                if (value.convert(QMetaType::fromName(query.value(3).toByteArray())))
                    settings.setValue(query.value(1).toString(), value);
            }
        }
    }

    settings.endGroup();
    return ok;
}

QString SystemPrivate::settingKey(const QString &name) const
{
    return qApp->applicationName() + '/' + name;
}

}
