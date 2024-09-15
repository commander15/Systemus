#ifndef SYSTEMUS_SETTINGS_H
#define SYSTEMUS_SETTINGS_H

#include <SystemusCore/global.h>

#include <QtCore/qscopedpointer.h>

class QSettings;

namespace Systemus {

class SettingsPrivate;
class SYSTEMUS_CORE_EXPORT Settings
{
public:
    Settings();
    Settings(const QString &group, QSettings *settings);
    ~Settings();

    bool hasSettings(const QString &name) const;

    QVariant setting(const QString &name) const;
    QVariant setting(const QString &name, const QVariant &defaultValue) const;

    void setSetting(const QString &name, const QVariant &value);
    void unsetSetting(const QString &name);

    QString group() const;
    void setGroup(const QString &name);

    bool sync(int maxCount = 100);

    QSettings *settings() const;

    static void setSettings(const QString &group, QSettings *settings);
    static Settings *instance();

private:
    Settings(SettingsPrivate *d);

    QScopedPointer<SettingsPrivate> d_ptr;
    static QScopedPointer<Settings> s_instance;
};

}

#endif // SYSTEMUS_SETTINGS_H
