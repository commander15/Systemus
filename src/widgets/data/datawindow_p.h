#ifndef SYSTEMUS_DATAWINDOW_P_H
#define SYSTEMUS_DATAWINDOW_P_H

#include "datawindow.h"

#include <SystemusWidgets/interfaceholder.h>

class QTabWidget;

namespace Systemus {

class DataWindowPrivate : public InterfaceHolder
{
public:
    DataWindowPrivate(int type, DataWindow *q);
    virtual ~DataWindowPrivate();

    void init(DataEdit *edit);
    virtual QWidget *createContainer(QWidget *parent) = 0;
    virtual int windowType() const = 0;

    static DataWindowPrivate *create(int type, DataWindow *q);

    DataWindow *q_ptr;
    DataEdit *edit;
};

class DefaultDataWindowPrivate : public DataWindowPrivate
{
public:
    DefaultDataWindowPrivate(DataWindow *q);

    QWidget *createContainer(QWidget *parent) override
    { return nullptr; }

    int currentIndex() const override { return 0; }
    void setCurrentIndex(int) override {}
    UserInterface *interface(int) const override { return m_interface; }
    int count() const override { return (m_interface ? 1 : 0); }
    int insertInterface(int, UserInterface *i) override { m_interface = i; return 0; }
    void removeInterface(UserInterface *) override {}

    int windowType() const override
    { return DataWindow::DefaultWindow; }

private:
    UserInterface *m_interface;
};

class DataTabWindowPrivate : public DataWindowPrivate
{
public:
    DataTabWindowPrivate(DataWindow *q);

    QWidget *createContainer(QWidget *parent) override;

    int currentIndex() const override;
    void setCurrentIndex(int index) override;
    UserInterface *interface(int index) const override;
    int count() const override;
    int insertInterface(int index, UserInterface *interface) override;
    void removeInterface(UserInterface *interface) override;

    int windowType() const override
    { return DataWindow::TabWindow; }

    QTabWidget *tab;
};

}

#endif // SYSTEMUS_DATAWINDOW_P_H
