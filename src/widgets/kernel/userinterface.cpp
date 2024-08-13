#include "userinterface.h"
#include "userinterface_p.h"

#include <SystemusWidgets/interfaceserver.h>

#include <QtGui/qevent.h>

namespace Systemus {

UserInterface::UserInterface(QWidget *parent) :
    UserInterface("default", parent)
{
}

UserInterface::UserInterface(const QByteArray &id, QWidget *parent) :
    UserInterface(new UserInterfacePrivate(id, this), parent)
{
}

UserInterface::UserInterface(UserInterfacePrivate *dd, QWidget *parent) :
    QWidget(parent),
    d_ptr(dd)
{
    S_D(UserInterface);

    setInterfaceIcon(QIcon(":/systemus/icons/systemus_mini.png"));

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

bool UserInterface::canHandleAction(int action) const
{
    return supportAction(action);
}

bool UserInterface::supportAction(int action) const
{
    Q_UNUSED(action);
    return false;
}

QVariant UserInterface::trigger(int action, const QVariantList &data)
{
    if (canHandleAction(action))
        return processAction(action, data);
    else {
        qWarning() << "Unsupported action" << action << "trigerred on" << d_ptr->id;
        return QVariant();
    }
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

QVariant UserInterface::processAction(int action, const QVariantList &data)
{
    Q_UNUSED(action);
    Q_UNUSED(data);
    return QVariant();
}

void UserInterface::switchToInterface(const QByteArray &id)
{
    emit externalActionRequested(QByteArrayLiteral(SYSTEMUS_INTERFACE_SERVER_ID), InterfaceServer::ShowInterfaceAction, { id });
}

void UserInterface::requestServerAction(int action, const QVariantList &data)
{
    emit externalActionRequested(QByteArrayLiteral(SYSTEMUS_INTERFACE_SERVER_ID), action, data);
}

void UserInterface::requestExternalAction(int action, const QVariantList &data)
{
    emit externalActionRequested(QByteArray(), action, data);
}

void UserInterface::requestExternalAction(const QByteArray &targetId, int action, const QVariantList &data)
{
    emit externalActionRequested(targetId, action, data);
}

UserInterfacePrivate::UserInterfacePrivate(const QByteArray &id, UserInterface *q) :
    q_ptr(q),
    id(id),
    _action(nullptr)
{
}

UserInterfacePrivate::~UserInterfacePrivate()
{
    if (_action)
        delete _action;
}

void UserInterfacePrivate::init()
{
    S_Q(UserInterface);

    QObject::connect(q, &QWidget::windowIconChanged, q, &UserInterface::interfaceIconChanged);
    QObject::connect(q, &QWidget::windowTitleChanged, q, &UserInterface::interfaceTitleChanged);
}

QAction *UserInterfacePrivate::action() const
{
    S_Q(UserInterface);

    if (!_action) {
        _action = new QAction(q->interfaceIcon(), q->interfaceTitle(), q);
        _action->setProperty("interfaceId", id);
        QObject::connect(q, &UserInterface::interfaceIconChanged, _action, &QAction::setIcon);
        QObject::connect(q, &UserInterface::interfaceTitleChanged, _action, &QAction::setText);
    }

    return _action;
}

}
