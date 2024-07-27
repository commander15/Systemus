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
        StandardInterface
    };

    UserInterface(QWidget *parent = nullptr);
    UserInterface(const QByteArray &id, QWidget *parent = nullptr);
    virtual ~UserInterface();

    QByteArray interfaceId() const;

    QIcon interfaceIcon() const;
    Q_SLOT void setInterfaceIcon(const QIcon &icon);
    Q_SIGNAL void interfaceIconChanged(const QIcon &icon);

    QString interfaceTitle() const;
    Q_SLOT void setInterfaceTitle(const QString &title);
    Q_SIGNAL void interfaceTitleChanged(const QString &title);

    virtual bool canHandleAction(int action) const;
    virtual bool supportAction(int action) const;
    QVariant trigger(int action, const QVariantList &data = QVariantList());
    Q_SIGNAL void actionSupportUpdated(int action, bool supported);
    Q_SIGNAL void externalActionRequested(const QByteArray &targetId, int action, const QVariantList &data);

    QAction *interfaceAction() const;

    Q_SIGNAL void printingRequested(QTextDocument *document, const QPageLayout &layout);

    virtual InterfaceType interfaceType() const
    { return DefaultInterface; }

    bool event(QEvent *event) override;

protected:
    UserInterface(UserInterfacePrivate *dd, QWidget *parent = nullptr);

    virtual void initUi();
    virtual void cleanupUi();
    virtual void translateUi();

    virtual QVariant processAction(int action, const QVariantList &data);

    void switchToInterface(const QByteArray &id);
    void requestServerAction(int action, const QVariantList &data = QVariantList());
    void requestExternalAction(int action, const QVariantList &data = QVariantList());
    void requestExternalAction(const QByteArray &targetId, int action, const QVariantList &data = QVariantList());

    mutable QScopedPointer<UserInterfacePrivate> d_ptr;

private:
    void setEditWidget(DataEdit *edit);
};

}

#endif // SYSTEMUS_USERINTERFACE_H
