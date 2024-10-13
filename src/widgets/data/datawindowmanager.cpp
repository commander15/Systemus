#include "datawindowmanager.h"
#include "datawindowmanager_p.h"

#include "datawindow.h"

#include <SystemusCore/private/debug_p.h>
#include <SystemusCore/data.h>

namespace Systemus {

DataWindowManager::DataWindowManager(QObject *parent) :
    QObject(parent),
    d_ptr(new DataWindowManagerPrivate(this))
{
}

DataWindowManager::~DataWindowManager()
{
    const QVariantList primaries = d_ptr->windows.keys();
    for (const QVariant &primary : primaries)
        delete d_ptr->windows.take(primary);
}

bool DataWindowManager::hasWindow(const QVariant &primary) const
{
    return d_ptr->windows.contains(primary);
}

QIcon DataWindowManager::windowIcon(const QVariant &primary) const
{
    if (d_ptr->windows.contains(primary))
        return d_ptr->windows.value(primary)->windowIcon();
    else
        return QIcon();
}

QString DataWindowManager::windowTitle(const QVariant &primary) const
{
    if (d_ptr->windows.contains(primary))
        return d_ptr->windows.value(primary)->windowTitle();
    else
        return QString();
}

Data DataWindowManager::windowData(const QVariant &primary) const
{
    if (d_ptr->windows.contains(primary))
        return d_ptr->windows.value(primary)->data();
    else
        return Data();
}

DataWindow *DataWindowManager::window(const QVariant &primary) const
{
    return d_ptr->windows.value(primary);
}

QVariantList DataWindowManager::availableWindows() const
{
    return d_ptr->windows.keys();
}

void DataWindowManager::addWindow(const QVariant &primary, DataWindow *window)
{
    if (!d_ptr->windows.contains(primary)) {
        setupWindow(primary, window, Registration);
        d_ptr->windows.insert(primary, window);
        emit windowAdded(primary, window);
    } else {
        changeWindow(primary, window);
    }
}

void DataWindowManager::changeWindow(const QVariant &primary, DataWindow *window)
{
    if (!d_ptr->windows.contains(primary))
        return;

    DataWindow *oldWindow = d_ptr->windows.value(primary);
    setupWindow(primary, oldWindow, Unregistration);

    setupWindow(primary, window, Registration);
    d_ptr->windows.insert(primary, window);
    emit windowChanged(primary, window);
}

void DataWindowManager::removeWindow(const QVariant &primary)
{
    if (!d_ptr->windows.contains(primary))
        return;

    setupWindow(primary, d_ptr->windows.value(primary), Unregistration);
    d_ptr->windows.remove(primary);
    emit windowRemoved(primary);
}

void DataWindowManager::setupWindow(const QVariant &primary, DataWindow *window, int mode)
{
    switch (mode) {
    case Registration:
        connect(window, &DataWindow::windowIconChanged, this, &DataWindowManager::processWindowIconChange);
        connect(window, &DataWindow::windowTitleChanged, this, &DataWindowManager::processWindowTitleChange);
        connect(window, &DataWindow::dataChanged, this, &DataWindowManager::processWindowDataChange);
        connect(window, &DataWindow::destroyed, this, &DataWindowManager::processWindowDestruction);
        break;

    case Unregistration:
        disconnect(window, &DataWindow::windowIconChanged, this, &DataWindowManager::processWindowIconChange);
        disconnect(window, &DataWindow::windowTitleChanged, this, &DataWindowManager::processWindowTitleChange);
        disconnect(window, &DataWindow::dataChanged, this, &DataWindowManager::processWindowDataChange);
        disconnect(window, &DataWindow::destroyed, this, &DataWindowManager::processWindowDestruction);
        break;
    }
}

void DataWindowManager::processWindowIconChange(const QIcon &icon)
{
    DataWindow *w = static_cast<DataWindow *>(sender());
    emit windowIconChanged(d_ptr->windows.key(w), icon);
}

void DataWindowManager::processWindowTitleChange(const QString &title)
{
    DataWindow *w = static_cast<DataWindow *>(sender());
    emit windowTitleChanged(d_ptr->windows.key(w), w->windowTitle());
}

void DataWindowManager::processWindowDataChange(const Data &data)
{
    DataWindow *w = static_cast<DataWindow *>(sender());

    QVariant primary = d_ptr->windows.key(w);
    if (primary != data.primaryValue()) {
        d_ptr->windows.remove(primary);
        emit windowRemoved(primary);

        primary = data.primaryValue();
        if (d_ptr->windows.contains(primary))
            systemusWarning() << "DataWindowManager: " << "Duplicate window for "
                              << data.className() << ", primary = " << primary;

        d_ptr->windows.insert(primary, w);
        emit windowAdded(primary, w);
    }
}

void DataWindowManager::processWindowDestruction()
{
    DataWindow *w = static_cast<DataWindow *>(sender());

    const QVariant primary = d_ptr->windows.key(w);
    d_ptr->windows.remove(primary);
    emit windowRemoved(primary);
}

DataWindowManagerPrivate::DataWindowManagerPrivate(DataWindowManager *q) :
    q_ptr(q),
    maxCount(-1)
{
}

}
