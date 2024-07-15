#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <SystemusWidgets/standarduserinterface.h>

#include <SystemusCore/datamodel.h>
#include <SystemusCore/user.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    manager.registerHolder(ui->toolBar);
    manager.registerHolder(ui->stackedWidget);

    Systemus::StandardUserInterface *i;

    i = new Systemus::StandardUserInterface("user", this);
    i->dataModel()->setDataInfo(Systemus::DataInfo::fromType<Systemus::User>());
    manager.addInterface(i);

    i = new Systemus::StandardUserInterface("role", this);
    i->dataModel()->setDataInfo(Systemus::DataInfo::fromName("Systemus::Role"));
    manager.addInterface(i);

    i = new Systemus::StandardUserInterface("group", this);
    i->dataModel()->setDataInfo(Systemus::DataInfo::fromName("Systemus::Group"));
    manager.addInterface(i);

    i = new Systemus::StandardUserInterface("privilege", this);
    i->dataModel()->setDataInfo(Systemus::DataInfo::fromName("Systemus::Privilege"));
    manager.addInterface(i);

    i = new Systemus::StandardUserInterface("permission", this);
    i->dataModel()->setDataInfo(Systemus::DataInfo::fromName("Systemus::Permission"));
    manager.addInterface(i);

    manager.setCurrentIndex(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}
