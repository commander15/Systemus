#include "userinterface.h"
#include "userinterface_p.h"

#include <QtGui/qevent.h>

namespace Systemus {

UserInterface::UserInterface(QWidget *parent) :
    UserInterface("default", parent)
{
}

UserInterface::UserInterface(const QByteArray &id, QWidget *parent) :
    QWidget(parent),
    d_ptr(new UserInterfacePrivate(id, this))
{
    S_D(UserInterface);
    d->init();
}

UserInterface::UserInterface(UserInterfacePrivate *dd, QWidget *parent) :
    QWidget(parent),
    d_ptr(dd)
{
    S_D(UserInterface);
    dd->init();
}

UserInterface::~UserInterface()
{
}

QByteArray UserInterface::interfaceId() const
{
    S_D(const UserInterface);
    return d->id;
}

QIcon UserInterface::interfaceIcon() const
{
    return windowIcon();
}

void UserInterface::setInterfaceIcon(const QIcon &icon)
{
    setWindowIcon(icon);
}

QString UserInterface::interfaceTitle() const
{
    return windowTitle();
}

void UserInterface::setInterfaceTitle(const QString &title)
{
    setWindowTitle(title);
}

bool UserInterface::supportAction(InterfaceAction &action) const
{
    Q_UNUSED(action);
    return false;
}

void UserInterface::trigger(InterfaceAction action, const QVariant &data)
{
    if (supportAction(action))
        processAction(action, data);
}

QAction *UserInterface::interfaceAction() const
{
    S_D(const UserInterface);
    return d->action();
}

bool UserInterface::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::Show:
        initUi();
        break;

    case QEvent::Hide:
        cleanupUi();
        break;

    case QEvent::LanguageChange:
        translateUi();
        break;

    default:
        break;
    }

    return QWidget::event(event);
}

void UserInterface::initUi()
{
}

void UserInterface::cleanupUi()
{
}

void UserInterface::translateUi()
{
}

void UserInterface::processAction(InterfaceAction action, const QVariant &data)
{
    Q_UNUSED(action);
    Q_UNUSED(data);
}

UserInterfacePrivate::UserInterfacePrivate(const QByteArray &id, UserInterface *qq) :
    q(qq),
    id(id)
{
}

void UserInterfacePrivate::init()
{
    QObject::connect(q, &QWidget::windowIconChanged, q, &UserInterface::interfaceIconChanged);
    QObject::connect(q, &QWidget::windowTitleChanged, q, &UserInterface::interfaceTitleChanged);
}

QAction *UserInterfacePrivate::action() const
{
    if (!_action) {
        _action = new QAction(q->interfaceIcon(), q->interfaceTitle(), q);
        _action->setCheckable(true);
        QObject::connect(q, &UserInterface::interfaceIconChanged, _action, &QAction::setIcon);
        QObject::connect(q, &UserInterface::interfaceTitleChanged, _action, &QAction::setText);
    }

    return _action;
}

}
