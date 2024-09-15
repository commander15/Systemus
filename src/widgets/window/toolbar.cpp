#include "toolbar.h"

#include <SystemusWidgets/userinterface.h>

namespace Systemus {

ToolBar::ToolBar(QWidget *parent) :
    QToolBar(parent),
    m_actions(this)
{
    m_actions.setExclusive(true);
}

ToolBar::~ToolBar()
{
}

void ToolBar::clear()
{
    const QList<QAction *> actions = m_actions.actions();
    for (QAction *action : actions)
        m_actions.removeAction(action);

    m_interfaces.clear();

    QToolBar::clear();
}

int ToolBar::currentIndex() const
{
    QAction *action = m_actions.checkedAction();
    for (int i(0); i < m_interfaces.size(); ++i)
        if (m_interfaces.at(i)->interfaceAction() == action)
            return i;
    return -1;
}

void ToolBar::setCurrentIndex(int index)
{
    m_interfaces.at(index)->interfaceAction()->trigger();
}

UserInterface *ToolBar::interface(int index) const
{
    return m_interfaces.at(index);
}

int ToolBar::count() const
{
    return m_interfaces.size();
}

int ToolBar::addInterface(UserInterface *interface)
{
    interface->interfaceAction()->setCheckable(true);

    m_actions.addAction(interface->interfaceAction());
    m_interfaces.append(interface);
    addAction(interface->interfaceAction());
    return m_interfaces.size();
}

int ToolBar::insertInterface(int index, UserInterface *interface)
{
    interface->interfaceAction()->setCheckable(true);

    m_actions.addAction(interface->interfaceAction());
    m_interfaces.insert(index, interface);
    insertAction(actions().at(index), interface->interfaceAction());
    return index;
}

void ToolBar::removeInterface(UserInterface *interface)
{
    m_actions.removeAction(interface->interfaceAction());
    m_interfaces.removeOne(interface);
    removeAction(interface->interfaceAction());
}

}
