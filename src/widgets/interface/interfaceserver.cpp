#include "interfaceserver.h"
#include "interfaceserver_p.h"

#include <SystemusWidgets/userinterface.h>

namespace Systemus {

InterfaceServer::InterfaceServer(QObject *parent) :
    QObject(parent),
    d(new InterfaceServerPrivate(this))
{
}

InterfaceServer::~InterfaceServer()
{
}

QIcon InterfaceServer::currentIcon() const
{
    return (d->currentInterface ? d->currentInterface->interfaceIcon() : QIcon());
}

QString InterfaceServer::currentTitle() const
{
    return (d->currentInterface ? d->currentInterface->interfaceTitle() : QString());
}

bool InterfaceServer::currentActionSupported(int action) const
{
    return (d->currentInterface ? d->currentInterface->supportAction(action) : false);
}

void InterfaceServer::setCurrentIndex(int index)
{
    if (SimpleInterfaceHolder::currentIndex() == index)
        return;

    SimpleInterfaceHolder::setCurrentIndex(index);
    emit currentIndexChanged(index);

    d->currentInterface = interface(index);
    emit currentInterfaceChanged(d->currentInterface);

    emit currentIconChanged(d->currentInterface ? d->currentInterface->interfaceIcon() : QIcon());
    emit currentTitleChanged(d->currentInterface ? d->currentInterface->interfaceTitle() : QString());
}

void InterfaceServer::registerInterface(UserInterface *interface)
{
    connect(interface, &UserInterface::interfaceIconChanged, this, &InterfaceServer::processIconChange);
    connect(interface, &UserInterface::interfaceTitleChanged, this, &InterfaceServer::processTitleChange);
    connect(interface, &UserInterface::actionSupportUpdated, this, &InterfaceServer::processActionSupportUpdate);
    connect(interface, &UserInterface::externalActionRequested, this, &InterfaceServer::processExternalAction);
    connect(interface, &UserInterface::printingRequested, this, &InterfaceServer::processPrintRequest);
}

void InterfaceServer::unregisterInterface(UserInterface *interface)
{
    disconnect(interface, &UserInterface::interfaceIconChanged, this, &InterfaceServer::processIconChange);
    disconnect(interface, &UserInterface::interfaceTitleChanged, this, &InterfaceServer::processTitleChange);
    disconnect(interface, &UserInterface::actionSupportUpdated, this, &InterfaceServer::processActionSupportUpdate);
    disconnect(interface, &UserInterface::externalActionRequested, this, &InterfaceServer::processExternalAction);
    disconnect(interface, &UserInterface::printingRequested, this, &InterfaceServer::processPrintRequest);
}

void InterfaceServer::processIconChange(const QIcon &icon)
{
    if (sender() == d->currentInterface)
        emit currentIconChanged(icon);
}

void InterfaceServer::processTitleChange(const QString &title)
{
    if (sender() == d->currentInterface)
        emit currentTitleChanged(title);
}

void InterfaceServer::processActionSupportUpdate(int action, bool supported)
{
    if (sender() == d->currentInterface)
        emit currentActionSupportUpdated(action, supported);
}

void InterfaceServer::processExternalAction(const QByteArray &targetId, int action, const QVariantList &data)
{
    if (targetId == SYSTEMUS_INTERFACE_SERVER_ID)
        processServerAction(action, data);
    else {
        UserInterface *interface = interfaceById(targetId);
        if (interface && interface->canHandleAction(action))
            interface->trigger(action, data);
    }
}

void InterfaceServer::processPrintRequest(QTextDocument *document, const QPageLayout &layout)
{
    emit printRequested(qobject_cast<UserInterface *>(sender()), document, layout);
}

void InterfaceServer::processServerAction(int action, const QVariantList &data)
{
    if (action == ShowInterfaceAction) {
        for (int i(0); i < count(); ++i) {
            if (interface(i)->interfaceId() == data.constFirst()) {
                emit interfaceRequested(i);
                break;
            }
        }
    }
}

InterfaceServerPrivate::InterfaceServerPrivate(InterfaceServer *q) :
    q(q),
    currentInterface(nullptr)
{
}

}
