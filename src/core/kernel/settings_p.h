#ifndef SYSTEMUS_SETTINGS_P_H
#define SYSTEMUS_SETTINGS_P_H

#include <QtCore/qhash.h>

#include <QtSql/qsqlrecord.h>

class QSettings;

class QSqlDatabase;

namespace Systemus {

class SettingsPrivate
{
public:
    SettingsPrivate();
    ~SettingsPrivate() = default;

    bool hasSetting(const QString &name) const;
    QVariant setting(const QString &name, const QVariant &defaultValue) const;
    void setSetting(const QString &name, const QVariant &value);

    bool syncSettings(int maxCount = 100);

    QString group;
    QSettings *settings;

protected:
    bool getSettings();
    int insertSetting(const QString &name, const QVariant &value);
    bool updateSetting(const QString &name, const QVariant &value);
    bool deleteSetting(const QString &name);

private:
    enum SettingType {
        Unknown = -1,
        Int = 10,
        Double = 20,
        Bool = 30,
        String = 40,
        Date = 50,
        Time = 60,
        DateTime = 70,
        ByteArray = 80
    };

    QString settingKey(const QString &name) const;
    static int settingTypeFromMetatype(const QMetaType &type);
    static QMetaType metaTypeFromSettingType(int type);

    int getSettingKeyId(const QString &name);

    QStringList m_dirtyKeys;
    QHash<QString, int> m_keyIds;

    QString m_tableName;
    QSqlRecord m_record;
};

}

#endif // SYSTEMUS_SETTINGS_P_H
