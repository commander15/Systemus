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

    QTimer::singleShot(0, this, &System::sync);

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
    if (!d->settings.contains(d->settingKey(name)) || d->settings.value(d->settingKey(name)) != value)
        d->settings.setValue(d->settingKey(name), value);
    d->dirtySettingKeys.append(name);
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

void System::sync()
{
    d->getData();
    d->syncSettings(true);
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
    now(QDateTime::currentDateTime()),
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
    timer.start();
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
    QSqlQuery query = Data::execQuery("SELECT name, version, CURRENT_TIMESTAMP FROM Systems ORDER BY id DESC LIMIT 1", &ok);

    if (ok && query.next()) {
        name = query.value(0).toString();
        version = QVersionNumber::fromString(query.value(1).toString());
        now = query.value(2).toDateTime();

        settings.beginGroup("System");
        settings.setValue("name", name);
        settings.setValue("version", version.toString());
        settings.endGroup();
    }

    return ok;
}

bool SystemPrivate::syncSettings(bool force)
{
    const QString table = QStringLiteral("SystemSettings");
    QSqlRecord record;
    record.append(QSqlField("id", QMetaType::fromType<int>(), table));
    record.append(QSqlField("name", QMetaType::fromType<QString>(), table));
    record.append(QSqlField("value", QMetaType::fromType<QString>(), table));
    record.append(QSqlField("type", QMetaType::fromType<int>(), table));

    bool ok = false;

    settings.beginGroup(qApp->applicationName());

    if (settingKeyIds.isEmpty() || dirtySettingKeys.isEmpty()) {
        QString statement = Data::driver()->sqlStatement(QSqlDriver::SelectStatement, table, record, false);

        QSqlQuery query = Data::execQuery(statement, &ok);
        if (ok) {
            settingKeyIds.clear();
            while (query.next()) {
                const QString name = query.value(1).toString();
                settingKeyIds.insert(name, query.value(0).toInt());
                QVariant value = query.value(2);
                if (value.convert(metaTypeFromSettingType(query.value(3).toInt())))
                    settings.setValue(name, value);
            }
        }
    } else {
        force = true;
    }

    if (force) {
        Data::beginTransaction();

        for (const QString &name : dirtySettingKeys) {
            const QVariant value = settings.value(name);

            record.setValue(1, name);
            record.setValue(2, settings.value(name).toString());
            record.setValue(3, settingTypeFromMetatype(value.metaType()));

            QString statement;

            // Bug: saves data with string type only (30)

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
    }

    settings.endGroup();
    return ok;
}

QString SystemPrivate::settingKey(const QString &name) const
{
    return qApp->applicationName() + '/' + name;
}

int SystemPrivate::settingTypeFromMetatype(const QMetaType &type)
{
    switch (type.id()) {
    case QMetaType::Int:
        return SettingType::Int;
    case QMetaType::Double:
        return SettingType::Double;
    case QMetaType::Bool:
        return SettingType::Bool;
    case QMetaType::QString:
        return SettingType::String;
    case QMetaType::QDate:
        return SettingType::Date;
    case QMetaType::QTime:
        return SettingType::Time;
    case QMetaType::QDateTime:
        return SettingType::DateTime;
    case QMetaType::QByteArray:
        return SettingType::ByteArray;
    default:
        return SettingType::Unknown;
    }
}

QMetaType SystemPrivate::metaTypeFromSettingType(int type)
{
    switch (type) {
    case SettingType::Int:
        return QMetaType(QMetaType::Int);
    case SettingType::Double:
        return QMetaType(QMetaType::Double);
    case SettingType::Bool:
        return QMetaType(QMetaType::Bool);
    case SettingType::String:
        return QMetaType(QMetaType::QString);
    case SettingType::Date:
        return QMetaType(QMetaType::QDate);
    case SettingType::Time:
        return QMetaType(QMetaType::QTime);
    case SettingType::DateTime:
        return QMetaType(QMetaType::QDateTime);
    case SettingType::ByteArray:
        return QMetaType(QMetaType::QByteArray);
    default:
        return QMetaType(QMetaType::UnknownType);
    }
}

}
