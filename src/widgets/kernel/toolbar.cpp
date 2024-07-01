#include "toolbar.h"

#include <SystemusWidgets/userinterface.h>

namespace Systemus {

ToolBar::ToolBar(QWidget *parent) :
    QToolBar(parent),
    _actions(this)
{
    _actions.setExclusive(true);
}

ToolBar::~ToolBar()
{
}

void ToolBar::clear()
{
    const QList<QAction *> actions = _actions.actions();
    for (QAction *action : actions)
        _actions.removeAction(action);

    _interfaces.clear();

    QToolBar::clear();
}

int ToolBar::currentIndex() const
{
    QAction *action = _actions.checkedAction();
    for (int i(0); i < _interfaces.size(); ++i)
        if (_interfaces.at(i)->interfaceAction() == action)
            return i;
    return -1;
}

void ToolBar::setCurrentIndex(int index)
{
    _interfaces.at(index)->interfaceAction()->trigger();
}

UserInterface *ToolBar::interface(int index) const
{
    return _interfaces.at(index);
}

int ToolBar::count() const
{
    return _interfaces.size();
}

int ToolBar::addInterface(UserInterface *interface)
{
    interface->interfaceAction()->setCheckable(true);

    _actions.addAction(interface->interfaceAction());
    _interfaces.append(interface);
    addAction(interface->interfaceAction());
    return _interfaces.size();
}

int ToolBar::insertInterface(int index, UserInterface *interface)
{
    interface->interfaceAction()->setCheckable(true);

    _actions.addAction(interface->interfaceAction());
    _interfaces.insert(index, interface);
    insertAction(actions().at(index), interface->interfaceAction());
    return index;
}

void ToolBar::removeInterface(UserInterface *interface)
{
    _actions.removeAction(interface->interfaceAction());
    _interfaces.removeOne(interface);
    removeAction(interface->interfaceAction());
}

}
