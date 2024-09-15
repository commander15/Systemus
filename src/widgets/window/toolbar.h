#ifndef SYSTEMUS_TOOLBAR_H
#define SYSTEMUS_TOOLBAR_H

#include <SystemusWidgets/global.h>
#include <SystemusWidgets/interfaceholder.h>

#include <QtWidgets/qtoolbar.h>
#include <QtWidgets/qactiongroup.h>

namespace Systemus {

class SYSTEMUS_WIDGETS_EXPORT ToolBar : public QToolBar, public InterfaceHolder
{
    Q_OBJECT

public:
    explicit ToolBar(QWidget *parent = nullptr);
    ~ToolBar();

    void clear();

    int currentIndex() const override;
    void setCurrentIndex(int index) override;

    UserInterface *interface(int index) const override;
    int count() const override;
    int addInterface(UserInterface *interface) override;
    int insertInterface(int index, UserInterface *interface) override;
    void removeInterface(UserInterface *interface) override;

private:
    QActionGroup m_actions;
    QVector<UserInterface *> m_interfaces;
};

}

#endif // SYSTEMUS_TOOLBAR_H
