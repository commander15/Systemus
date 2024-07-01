#include "interfaceholder.h"

namespace Systemus {

InterfaceHolder::InterfaceHolder()
{}

InterfaceHolder::~InterfaceHolder()
{
}

UserInterface *InterfaceHolder::currentInterface() const
{
    return interface(currentIndex());
}

int InterfaceHolder::addInterface(UserInterface *interface)
{
    return insertInterface(count() - 1, interface);
}

int InterfaceHolder::indexOf(UserInterface *interface) const
{
    for (int i(0); i < count(); ++i)
        if (this->interface(i) == interface)
            return i;
    return -1;
}

void InterfaceHolder::completeRegistration(InterfaceHolderManager *manager)
{
    Q_UNUSED(manager);
}

void InterfaceHolder::completeUnregistration(InterfaceHolderManager *manager)
{
    Q_UNUSED(manager);
}

InterfaceHolderManager::InterfaceHolderManager()
{
}

InterfaceHolderManager::~InterfaceHolderManager()
{
}

bool InterfaceHolderManager::hasHolders() const
{
    return _holders.size() > 0;
}

void InterfaceHolderManager::registerHolder(InterfaceHolder *holder)
{
    if (!_holders.contains(holder)) {
        _holders.append(holder);
        holder->completeRegistration(this);
    }
}

void InterfaceHolderManager::unregisterHolder(InterfaceHolder *holder)
{
    if (_holders.contains(holder)) {
        _holders.removeOne(holder);
        holder->completeUnregistration(this);
    }
}

UserInterface *InterfaceHolderManager::currentInterface() const
{
    return (hasHolders() ? _holders.constFirst()->currentInterface() : nullptr);
}

int InterfaceHolderManager::currentIndex() const
{
    return (hasHolders() ? _holders.constFirst()->currentIndex() : -1);
}

void InterfaceHolderManager::setCurrentIndex(int index)
{
    for (InterfaceHolder *holder : _holders)
        holder->setCurrentIndex(index);
}

UserInterface *InterfaceHolderManager::interface(int index) const
{
    return (hasHolders() ? _holders.constFirst()->interface(index) : nullptr);
}

int InterfaceHolderManager::count() const
{
    return (hasHolders() ? _holders.constFirst()->count() : 0);
}

int InterfaceHolderManager::addInterface(UserInterface *interface)
{
    int index;
    for (InterfaceHolder *holder : _holders)
        index = holder->addInterface(interface);
    return index;
}

int InterfaceHolderManager::insertInterface(int index, UserInterface *interface)
{
    int interfaceIndex;
    for (InterfaceHolder *holder : _holders)
        interfaceIndex = holder->insertInterface(index, interface);
    return interfaceIndex;
}

void InterfaceHolderManager::removeInterface(UserInterface *interface)
{
    for (InterfaceHolder *holder : _holders)
        holder->removeInterface(interface);
}

int InterfaceHolderManager::indexOf(UserInterface *interface) const
{
    return (hasHolders() ? _holders.constFirst()->indexOf(interface) : -1);
}

}
