#ifndef SYSTEMUS_SYSTEM_P_H
#define SYSTEMUS_SYSTEM_P_H

#include <SystemusCore/system.h>

#include <QtSql/qsqlrecord.h>

#include <QtCore/qabstractnativeeventfilter.h>

namespace Systemus {

class SystemPrivate : public SystemData, public QAbstractNativeEventFilter
{
public:
    enum SystemProperty {
        LogoProperty,
        NameProperty,
        VersionProperty
    };

    SystemPrivate(System *q);

    bool getProperties();

    bool hasPendingPropertyChanges() const;
    void markPropertyChange(SystemProperty property, bool changed = true);
    bool commitPropertyChanges(QList<SystemProperty> *properties = nullptr);

    bool getTime();

    bool timersActive() const;
    void startTimers();
    void stopTimers();
    void processTimerEvent(QTimerEvent *event);

    bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override;

    System *q_ptr;

    int heartbeatInterval;

    QString dir;

private:
    static QString systemTable();
    static QSqlRecord systemRecord();

    QList<SystemProperty> m_dirtyProperties;

    int m_heartbeatTimerId;
    int m_nowTimerId;
};

}

#endif // SYSTEMUS_SYSTEM_P_H
