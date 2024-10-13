#ifndef DATAWINDOWMANAGER_H
#define DATAWINDOWMANAGER_H

#include <SystemusWidgets/global.h>

#include <QtCore/qobject.h>

namespace Systemus {
class DataWindow;
class Data;

/*
    To ReWork !
    This class must be rewritten to support multiple windows with the same primary value
*/

class DataWindowManagerPrivate;
class SYSTEMUS_WIDGETS_EXPORT DataWindowManager : public QObject
{
    Q_OBJECT

public:
    explicit DataWindowManager(QObject *parent = nullptr);
    ~DataWindowManager();

    bool hasWindow(const QVariant &primary) const;

    QIcon windowIcon(const QVariant &primary) const;
    Q_SIGNAL void windowIconChanged(const QVariant &primary, const QIcon &icon);

    QString windowTitle(const QVariant &primary) const;
    Q_SIGNAL void windowTitleChanged(const QVariant &primary, const QString &title);

    Data windowData(const QVariant &primary) const;
    Q_SIGNAL void windowDataChanged(const QVariant &primary, const Data &data);

    DataWindow *window(const QVariant &primary) const;

    QVariantList availableWindows() const;
    Q_SIGNAL void availableWindowsChanged();

    Q_SLOT void addWindow(const QVariant &primary, DataWindow *window);
    Q_SIGNAL void windowAdded(const QVariant &primary, DataWindow *window);

    Q_SLOT void changeWindow(const QVariant &primary, DataWindow *window);
    Q_SIGNAL void windowChanged(const QVariant &primary, DataWindow *window);

    Q_SLOT void removeWindow(const QVariant &primary);
    Q_SIGNAL void windowRemoved(const QVariant &primary);

protected:
    enum SetupMode {
        Registration,
        Unregistration
    };

    virtual void setupWindow(const QVariant &primary, DataWindow *window, int mode);

private:
    Q_SLOT void processWindowIconChange(const QIcon &icon);
    Q_SLOT void processWindowTitleChange(const QString &title);
    Q_SLOT void processWindowDataChange(const Data &data);
    Q_SLOT void processWindowDestruction();

    QScopedPointer<DataWindowManagerPrivate> d_ptr;
};

}

#endif // DATAWINDOWMANAGER_H
