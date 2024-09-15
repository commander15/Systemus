#include "interfaceholder.h"

#include <SystemusWidgets/userinterface.h>

namespace Systemus {

InterfaceHolder::InterfaceHolder()
{
}

InterfaceHolder::~InterfaceHolder()
{
}

UserInterface *InterfaceHolder::currentInterface() const
{
    return interface(currentIndex());
}

int InterfaceHolder::addInterface(UserInterface *interface)
{
    return insertInterface(count(), interface);
}

UserInterface *InterfaceHolder::interfaceById(const QByteArray &id) const
{
    for (int i(0); i < count(); ++i) {
        UserInterface *interface = this->interface(i);
        if (interface->interfaceId() == id)
            return interface;
    }

    return nullptr;
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

SimpleInterfaceHolder::SimpleInterfaceHolder() :
    _current(-1)
{
}

SimpleInterfaceHolder::~SimpleInterfaceHolder()
{
}

UserInterface *SimpleInterfaceHolder::currentInterface() const
{
    return (_current >= 0 && _current < _interfaces.size() ? _interfaces.at(_current) : nullptr);
}

int SimpleInterfaceHolder::currentIndex() const
{
    return _current;
}

void SimpleInterfaceHolder::setCurrentIndex(int index)
{
    if (isValidIndex(index)) {
        const int oldIndex = _current;
        _current = index;
        processInterfaceChange(interface(oldIndex), interface(index));
    }
}

UserInterface *SimpleInterfaceHolder::interface(int index) const
{
    if (isValidIndex(index))
        return _interfaces.at(index);
    else
        return nullptr;
}

int SimpleInterfaceHolder::count() const
{
    return _interfaces.count();
}

int SimpleInterfaceHolder::addInterface(UserInterface *interface)
{
    if (!_interfaces.contains(interface))
        registerInterface(interface);

    _interfaces.append(interface);
    return _interfaces.count() - 1;
}

int SimpleInterfaceHolder::insertInterface(int index, UserInterface *interface)
{
    if (index >= 0 && index <= _interfaces.size()) {
        if (!_interfaces.contains(interface))
            registerInterface(interface);

        _interfaces.insert(index, interface);
        return index;
    } else
        return -1;
}

void SimpleInterfaceHolder::removeInterface(UserInterface *interface)
{
    if (!_interfaces.contains(interface))
        return;

    unregisterInterface(interface);
    _interfaces.removeOne(interface);
}

int SimpleInterfaceHolder::indexOf(UserInterface *interface) const
{
    return _interfaces.indexOf(interface);
}

void SimpleInterfaceHolder::processInterfaceChange(UserInterface *from, UserInterface *to)
{
    Q_UNUSED(from);
    Q_UNUSED(to);
}

void SimpleInterfaceHolder::registerInterface(UserInterface *interface)
{
    Q_UNUSED(interface);
}

void SimpleInterfaceHolder::unregisterInterface(UserInterface *interface)
{
    Q_UNUSED(interface);
}

bool SimpleInterfaceHolder::isValidIndex(int index) const
{
    return index >= 0 && index < _interfaces.size();
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
    int index = -1;
    for (InterfaceHolder *holder : _holders)
        index = holder->addInterface(interface);
    return index;
}

int InterfaceHolderManager::insertInterface(int index, UserInterface *interface)
{
    int interfaceIndex = -1;
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
