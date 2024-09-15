#ifndef SYSTEMUS_USERINTERFACE_H
#define SYSTEMUS_USERINTERFACE_H

#include <SystemusWidgets/global.h>

#include <QtWidgets/qwidget.h>

class QTextDocument;
class QPageLayout;

namespace Systemus {

class DataEdit;

typedef DataEdit*(DataEditCreation)();

class UserInterfacePrivate;
class SYSTEMUS_WIDGETS_EXPORT UserInterface : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QByteArray interfaceId READ interfaceId CONSTANT)

public:
    enum InterfaceType {
        DefaultInterface,
        DefaultDataInterface
    };

    UserInterface(QWidget *parent = nullptr);
    virtual ~UserInterface();

    QByteArray interfaceId() const;
    void setInterfaceId(const QByteArray &id);

    QIcon interfaceIcon() const;
    Q_SLOT void setInterfaceIcon(const QIcon &icon);
    Q_SIGNAL void interfaceIconChanged(const QIcon &icon);

    QString interfaceTitle() const;
    Q_SLOT void setInterfaceTitle(const QString &title);
    Q_SIGNAL void interfaceTitleChanged(const QString &title);

    QAction *interfaceAction() const;

    virtual bool canHandleAction(int action) const;

    virtual bool supportAction(int action) const;
    Q_SIGNAL void actionSupportUpdated(int action, bool supported);

    QVariant trigger(int action, const QVariantList &data = QVariantList());
    Q_SIGNAL void externalActionRequested(const QByteArray &targetId, int action, const QVariantList &data);

    Q_SIGNAL void printingRequested(QTextDocument *document, const QPageLayout &layout);

    bool isDataInterface() const;
    virtual int interfaceType() const;

    bool event(QEvent *event) override;

protected:
    UserInterface(UserInterfacePrivate *dd, QWidget *parent = nullptr);

    virtual void initUi();
    virtual void cleanupUi();
    virtual void translateUi(bool full = true);

    virtual QVariant processAction(int action, const QVariantList &data);

    void switchToInterface(const QByteArray &id);
    void requestServerAction(int action, const QVariantList &data = QVariantList());

    void requestExternalAction(int action, const QVariantList &data = QVariantList());
    void requestExternalAction(const QByteArray &targetId, int action, const QVariantList &data = QVariantList());

    mutable QScopedPointer<UserInterfacePrivate> d_ptr;
};

}

#endif // SYSTEMUS_USERINTERFACE_H
