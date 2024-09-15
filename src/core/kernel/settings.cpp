#include "settings.h"
#include "settings_p.h"

#include <SystemusCore/namespace.h>

#include <QtCore/qcoreapplication.h>
#include <QtCore/qsettings.h>

#include <QtSql/qsqldatabase.h>
#include <QtSql/qsqlquery.h>
#include <QtSql/qsqldriver.h>
#include <QtSql/qsqlfield.h>

namespace Systemus {

Settings::Settings() :
    Settings(QStringLiteral("Systemus"), new QSettings(qApp))
{
}

Settings::Settings(const QString &group, QSettings *settings) :
    d_ptr(new SettingsPrivate())
{
    d_ptr->group = group;
    d_ptr->settings = settings;
}

Settings::Settings(SettingsPrivate *d) :
    d_ptr(d)
{
}

Settings::~Settings()
{
}

bool Settings::hasSettings(const QString &name) const
{
    return d_ptr->hasSetting(name);
}

QVariant Settings::setting(const QString &name) const
{
    return d_ptr->setting(name, QVariant());
}

QVariant Settings::setting(const QString &name, const QVariant &defaultValue) const
{
    return d_ptr->setting(name, defaultValue);
}

void Settings::setSetting(const QString &name, const QVariant &value)
{
    d_ptr->setSetting(name, value);
}

void Settings::unsetSetting(const QString &name)
{
    d_ptr->setSetting(name, QVariant());
}

QString Settings::group() const
{
    return d_ptr->group;
}

void Settings::setGroup(const QString &name)
{
    d_ptr->group = name;
}

bool Settings::sync(int maxCount)
{
    return d_ptr->syncSettings(maxCount);
}

QSettings *Settings::settings() const
{
    return d_ptr->settings;
}

void Settings::setSettings(const QString &group, QSettings *settings)
{
    if (!s_instance) {
        s_instance.reset(new Settings(group, settings));
    } else {
        SettingsPrivate *d = s_instance.get()->d_ptr.get();
        d->group = group;
        d->settings = settings;
    }
}

Settings *Settings::instance()
{
    if (!s_instance)
        s_instance.reset(new Settings());
    return s_instance.get();
}

QScopedPointer<Settings> Settings::s_instance;

SettingsPrivate::SettingsPrivate() :
    group(QStringLiteral("Systemus")),
    settings(nullptr),
    m_tableName(QStringLiteral("SSystemSettings"))
{
    m_record.append(QSqlField("id", QMetaType::fromType<int>()));
    m_record.append(QSqlField("name", QMetaType::fromType<QString>()));
    m_record.append(QSqlField("value", QMetaType::fromType<QString>()));
    m_record.append(QSqlField("type", QMetaType::fromType<int>()));
}

bool SettingsPrivate::hasSetting(const QString &name) const
{
    return (settings ? settings->contains(settingKey(name)) : false);
}

QVariant SettingsPrivate::setting(const QString &name, const QVariant &defaultValue) const
{
    return (settings ? settings->value(settingKey(name), defaultValue) : defaultValue);
}

void SettingsPrivate::setSetting(const QString &name, const QVariant &value)
{
    if (!settings)
        return;

    settings->setValue(settingKey(name), value);

    if (!m_dirtyKeys.contains(name)) {
        m_dirtyKeys.append(name);

        if (!m_keyIds.contains(name))
            m_keyIds.insert(name, getSettingKeyId(name));
    }
}

bool SettingsPrivate::syncSettings(int maxCount)
{
    Systemus::beginTransaction();

    int count = 0;
    for (const QString &key : m_dirtyKeys) {
        int id;
        if (!settings->contains(settingKey(key))) {
            deleteSetting(key);
        } else if ((id = insertSetting(key, settings->value(key))) > 0 || updateSetting(key, settings->value(key))) {
            if (id > 0)
                m_keyIds.insert(key, id);
        } else {
            Systemus::rollbackTransaction();
            return false;
        }

        ++count;
        if (count == maxCount)
            break;
    }

    Systemus::commitTransaction();

    if (count > 0)
        m_dirtyKeys.remove(count);

    return (count < maxCount ? getSettings() : true);
}

bool SettingsPrivate::getSettings()
{
    QString statement = Systemus::sqlStatement(QSqlDriver::SelectStatement, m_tableName, m_record, false);

    bool ok = false;
    QSqlQuery query = Systemus::exec(statement, &ok);
    if (ok) {
        settings->beginGroup("");
        while (query.next()) {
            m_keyIds.insert(query.value(1).toString(), query.value(0).toInt());

            QVariant value = query.value(2);
            value.convert(QMetaType(query.value(3).toInt()));

            settings->setValue(query.value(1).toString(), value);
        }
        settings->endGroup();
        return true;
    } else {
        return false;
    }
}

int SettingsPrivate::insertSetting(const QString &name, const QVariant &value)
{
    QSqlRecord record = m_record;
    record.remove(0);
    record.setValue(0, name);
    record.setValue(1, value);
    record.setValue(2, settingTypeFromMetatype(value.metaType()));

    QString query = Systemus::sqlStatement(QSqlDriver::InsertStatement, m_tableName, m_record, false);

    QSqlQuery sqlQuery = Systemus::exec(query);
    return (sqlQuery.exec(query) ? sqlQuery.lastInsertId().toInt() : 0);
}

bool SettingsPrivate::updateSetting(const QString &name, const QVariant &value)
{
    QSqlRecord record = m_record;
    record.remove(0);
    record.remove(0);
    record.setValue(0, value);
    record.setValue(1, settingTypeFromMetatype(value.metaType()));

    QString query = Systemus::sqlStatement(QSqlDriver::UpdateStatement, m_tableName, m_record, false);

    QSqlQuery sqlQuery = Systemus::exec(query);
    return (sqlQuery.exec(query + " WHERE name = '" + name + '\''));
}

bool SettingsPrivate::deleteSetting(const QString &name)
{
    bool ok = false;
    Systemus::exec("DELETE FROM " + m_tableName + " WHERE name = '" + name + '\'', &ok);
    return ok;
}

QString SettingsPrivate::settingKey(const QString &name) const
{
    return QStringLiteral("Settings/") + name;
}

int SettingsPrivate::settingTypeFromMetatype(const QMetaType &type)
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

QMetaType SettingsPrivate::metaTypeFromSettingType(int type)
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

int SettingsPrivate::getSettingKeyId(const QString &name)
{
    bool ok = false;
    QSqlQuery query = Systemus::exec("SELECT id FROM " + m_tableName + " WHERE name = '" + name + '\'', &ok);
    if (ok && query.next())
        return query.value(0).toInt();
    else
        return 0;
}

}
