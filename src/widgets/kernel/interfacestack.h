#ifndef SYSTEMUS_INTERFACESTACK_H
#define SYSTEMUS_INTERFACESTACK_H

#include <SystemusWidgets/global.h>
#include <SystemusWidgets/interfaceholder.h>

#include <QtWidgets/qwidget.h>

class QTextDocument;

namespace Systemus {

namespace Ui {
class InterfaceStack;
}

class UserInterface;

class SYSTEMUS_WIDGETS_EXPORT InterfaceStack : public QWidget, public InterfaceHolder
{
    Q_OBJECT

public:
    explicit InterfaceStack(QWidget *parent = nullptr);
    ~InterfaceStack();

    UserInterface *currentInterface() const override;

    int currentIndex() const override;
    Q_SLOT void setCurrentIndex(int index) override;
    Q_SIGNAL void currentIndexChanged(int index);

    UserInterface *interface(int index) const override;
    int count() const override;
    int addInterface(UserInterface *interface) override;
    int insertInterface(int index, UserInterface *interface) override;
    void removeInterface(UserInterface *interface) override;

    int indexOf(UserInterface *interface) const override;

private:
    Q_SLOT void showInterface();

    Q_SLOT void processWidgetChange(int index);

    Ui::InterfaceStack *ui;
};

}

#endif // SYSTEMUS_INTERFACESTACK_H
