#ifndef SYSTEMUS_INTERFACESERVER_H
#define SYSTEMUS_INTERFACESERVER_H

#include <SystemusWidgets/global.h>
#include <SystemusWidgets/interfaceholder.h>

#include <QtCore/qobject.h>

#define SYSTEMUS_INTERFACE_SERVER_ID "systemus.interface.server"

class QTextDocument;
class QPageLayout;
class QPagedPaintDevice;

namespace Systemus {

class InterfaceServerPrivate;
class SYSTEMUS_WIDGETS_EXPORT InterfaceServer : public QObject, public SimpleInterfaceHolder
{
    Q_OBJECT

public:
    enum ServerAction {
        ShowInterfaceAction
    };

    explicit InterfaceServer(QObject *parent = nullptr);
    virtual ~InterfaceServer();

    QIcon currentIcon() const;
    Q_SIGNAL void currentIconChanged(const QIcon &icon);

    QString currentTitle() const;
    Q_SIGNAL void currentTitleChanged(const QString &title);

    bool currentActionSupported(int action) const;
    Q_SIGNAL void currentActionSupportUpdated(int action, bool supported);

    Q_SIGNAL void printRequested(UserInterface *origin, QTextDocument *document, const QPageLayout &layout);

    Q_SLOT void setCurrentIndex(int index) override;
    Q_SIGNAL void currentIndexChanged(int index);

    Q_SIGNAL void currentInterfaceChanged(UserInterface *interface);

protected:
    void registerInterface(UserInterface *interface) override;
    void unregisterInterface(UserInterface *interface) override;

private:
    Q_SLOT void processIconChange(const QIcon &icon);
    Q_SLOT void processTitleChange(const QString &title);
    Q_SLOT void processActionSupportUpdate(int action, bool supported);
    Q_SLOT void processExternalAction(const QByteArray &targetId, int action, const QVariantList &data);
    Q_SLOT void processPrintRequest(QTextDocument *document, const QPageLayout &layout);

    void processServerAction(int action, const QVariantList &data);

    QScopedPointer<InterfaceServerPrivate> d;
};

}

#endif // SYSTEMUS_INTERFACESERVER_H
