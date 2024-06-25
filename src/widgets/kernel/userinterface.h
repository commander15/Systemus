#ifndef SYSTEMUS_USERINTERFACE_H
#define SYSTEMUS_USERINTERFACE_H

#include <SystemusWidgets/global.h>

#include <QtWidgets/qwidget.h>

namespace Systemus {

class UserInterfacePrivate;
class SYSTEMUS_WIDGETS_EXPORT UserInterface : public QWidget
{
    Q_OBJECT

public:
    enum InterfaceAction {
        SearchAction,
        RefreshAction,
        ShowAction,
        AddAction,
        EditAction,
        DeleteAction,
        PrintAction
    };

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

    virtual bool supportAction(int action) const;
    QVariant trigger(int action, const QVariant &data = QVariant());
    Q_SIGNAL void actionSupportUpdated(InterfaceAction action, bool supported);

    QAction *interfaceAction() const;

    virtual InterfaceType interfaceType() const
    { return DefaultInterface; }

    bool event(QEvent *event) override;

protected:
    UserInterface(UserInterfacePrivate *dd, QWidget *parent = nullptr);

    virtual void initUi();
    virtual void cleanupUi();
    virtual void translateUi();

    virtual QVariant processAction(int action, const QVariant &data);

    mutable QScopedPointer<UserInterfacePrivate> d_ptr;
};

}

#endif // SYSTEMUS_USERINTERFACE_H
