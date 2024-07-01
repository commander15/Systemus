#ifndef SYSTEMUS_AUTHORIZATIONDATA_H
#define SYSTEMUS_AUTHORIZATIONDATA_H

#include <SystemusCore/global.h>
#include <SystemusCore/data.h>

namespace Systemus {

class AuthorizationDataPrivate;
class SYSTEMUS_CORE_EXPORT AuthorizationData : public Data
{
    Q_GADGET
    Q_PROPERTY(QString name READ name WRITE setName USER true)
    Q_PROPERTY(QString description READ description WRITE setDescription)
    Q_PROPERTY(QDate creationDate READ creationDate)
    Q_PROPERTY(QTime creationTime READ creationTime)

public:
    AuthorizationData();
    AuthorizationData(const AuthorizationData &other);
    virtual ~AuthorizationData();

    QString name() const;
    void setName(const QString &name);

    QString description() const;
    void setDescription(const QString &description);

    QDate creationDate() const;
    QTime creationTime() const;

    bool isValid() const override;

protected:
    AuthorizationData(AuthorizationDataPrivate *data);
};

}

Q_DECLARE_METATYPE(Systemus::AuthorizationData)

#endif // SYSTEMUS_AUTHORIZATIONDATA_H
