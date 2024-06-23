#ifndef SYSTEM_H
#define SYSTEM_H

#include <SystemusCore/data.h>

class Installation : public Systemus::Data
{
    Q_GADGET
    Q_PROPERTY(QString version MEMBER version USER true)
    Q_PROPERTY(QDate date MEMBER date)
    Q_PROPERTY(QTime time MEMBER time)
    Q_CLASSINFO("table", "SystemInstallations")

public:
    Installation();

    QString version;
    QDate date;
    QTime time;

private:
    SYSTEMUS_DATA_METHODS(Installation)
};

Q_DECLARE_METATYPE(Installation)

class System : public Systemus::Data
{
    Q_GADGET
    Q_PROPERTY(QString name MEMBER name USER true)
    Q_PROPERTY(QString version MEMBER version REQUIRED)
    //Q_PROPERTY(QList<Installation> installations MEMBER installations STORED false)

public:
    System();

    QString name;
    QString version;
    Systemus::OneToManyRelationship<System, Installation> installations;

private:
    SYSTEMUS_DATA_METHODS(System)
};

Q_DECLARE_METATYPE(System)

#endif // SYSTEM_H
