#include "userinterface.h"
#include "userinterface_p.h"

#include <SystemusWidgets/interfaceserver.h>

#include <QtGui/qevent.h>

namespace Systemus {

UserInterface::UserInterface(QWidget *parent) :
    UserInterface(new UserInterfacePrivate(this), parent)
{
}

UserInterface::UserInterface(UserInterfacePrivate *dd, QWidget *parent) :
    QWidget(parent),
    d_ptr(dd)
{
    setWindowIcon(QIcon(":/systemus/icons/systemus_mini.png"));
    connect(this, &QWidget::windowIconChanged, this, &UserInterface::interfaceIconChanged);

    setWindowTitle("Interface");
    connect(this, &QWidget::windowTitleChanged, this, &UserInterface::interfaceTitleChanged);
}

UserInterface::~UserInterface()
{
}

QByteArray UserInterface::interfaceId() const
{
    S_D(const UserInterface);
    return d->id;
}

void UserInterface::setInterfaceId(const QByteArray &id)
{
    S_D(UserInterface);
    d->id = id;
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

QAction *UserInterface::interfaceAction() const
{
    S_D(const UserInterface);
    return d->action();
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

bool UserInterface::isDataInterface() const
{
    return isDataInterface() == DefaultDataInterface;
}

int UserInterface::interfaceType() const
{
    return DefaultInterface;
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

void UserInterface::translateUi(bool full)
{
    Q_UNUSED(full);
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

UserInterfacePrivate::UserInterfacePrivate(UserInterface *q) :
    q_ptr(q),
    m_action(nullptr)
{
}

UserInterfacePrivate::~UserInterfacePrivate()
{
    if (m_action)
        delete m_action;
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

    if (!m_action) {
        m_action = new QAction(q->interfaceIcon(), q->interfaceTitle(), q);
        m_action->setProperty("interfaceId", id);
        QObject::connect(q, &UserInterface::interfaceIconChanged, m_action, &QAction::setIcon);
        QObject::connect(q, &UserInterface::interfaceTitleChanged, m_action, &QAction::setText);
    }

    return m_action;
}

}
