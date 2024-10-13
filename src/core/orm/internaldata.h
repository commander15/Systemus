#ifndef SYSTEMUS_INTERNALDATA_H
#define SYSTEMUS_INTERNALDATA_H

#include <SystemusCore/global.h>
#include <SystemusCore/data.h>

#include <QtCore/qdatetime.h>

namespace Systemus {

class InternalDataPrivate;
class SYSTEMUS_CORE_EXPORT InternalData : public Systemus::Orm::Data
{
    Q_GADGET
    Q_PROPERTY(int id READ id WRITE setId)

public:
    InternalData();
    InternalData(const InternalData &other);
    ~InternalData();

    int id() const;
    void setId(int id);

    QString defaultFilter() const;

protected:
    InternalData(InternalDataPrivate *d);
};

class DescriptiveDataPrivate;
class SYSTEMUS_CORE_EXPORT DescriptiveData : public InternalData
{
    Q_GADGET
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString description READ description WRITE setDescription)
    Q_PROPERTY(QDate creationDate READ creationDate)
    Q_PROPERTY(QTime creationTime READ creationTime)

public:
    DescriptiveData();
    DescriptiveData(const DescriptiveData &other) = default;

    QString name() const;
    void setName(const QString &name);

    QString description() const;
    void setDescription(const QString &desc);

    QDate creationDate() const;
    QTime creationTime() const;

protected:
    bool writeProperty(const QString &name, const QVariant &value) override;
};

}

#endif // SYSTEMUS_INTERNALDATA_H
