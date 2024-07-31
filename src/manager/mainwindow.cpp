#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <SystemusWidgets/standarduserinterface.h>
#include <SystemusWidgets/useredit.h>

#include <SystemusCore/datamodel.h>
#include <SystemusCore/user.h>
#include <SystemusCore/authenticator.h>

#include <QtWidgets/qlabel.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    {
        QLabel *label = new QLabel(this);

        QSize size = ui->toolBar->iconSize();
        size.setWidth(size.width() * 6);
        label->setPixmap(QPixmap(":/systemus/icons/systemus_full.png").scaled(size, Qt::KeepAspectRatio));
        ui->toolBar->addWidget(label);
        ui->toolBar->addSeparator();
    }

    manager.registerHolder(ui->toolBar);
    manager.registerHolder(ui->stackedWidget);

    Systemus::StandardUserInterface *i;

    i = new Systemus::StandardUserInterface("user", this);
    i->setDataModel("Systemus::User");
    i->dataModel()->setProperties({ "name", "active", "creationDate", "creationTime" });
    i->dataModel()->setHeaderData(0, Qt::Horizontal, tr("Name"));
    i->dataModel()->setHeaderData(1, Qt::Horizontal, tr("Active"));
    i->dataModel()->setHeaderData(2, Qt::Horizontal, tr("Creation Date"));
    i->dataModel()->setHeaderData(3, Qt::Horizontal, tr("Creation Time"));
    i->dataModel()->setFilter("id != " + QString::number(Systemus::Authenticator::instance()->loggedUser().id()));
    i->setDataEditDialog(Systemus::DataEditDialog::fromEdit(new Systemus::UserEdit(this), this));
    manager.addInterface(i);

    ui->toolBar->addSeparator();

    i = new Systemus::StandardUserInterface("role", this);
    i->setDataModel(QByteArrayLiteral("Systemus::Role"));
    i->dataModel()->setProperties({ "name", "active", "creationDate", "creationTime" });
    manager.addInterface(i);

    i = new Systemus::StandardUserInterface("group", this);
    i->setDataModel(QByteArrayLiteral("Systemus::Group"));
    i->dataModel()->setProperties({ "name", "active", "creationDate", "creationTime" });
    manager.addInterface(i);

    ui->toolBar->addSeparator();

    i = new Systemus::StandardUserInterface("privilege", this);
    i->setDataModel(QByteArrayLiteral("Systemus::Privilege"));
    i->dataModel()->setProperties({ "name", "active", "creationDate", "creationTime" });
    manager.addInterface(i);

    i = new Systemus::StandardUserInterface("permission", this);
    i->setDataModel(QByteArrayLiteral("Systemus::Permission"));
    i->dataModel()->setProperties({ "name", "active", "creationDate", "creationTime" });
    i->dataModel()->setHeaderData(0, Qt::Horizontal, tr("Name"));
    i->dataModel()->setHeaderData(1, Qt::Horizontal, tr("Active"));
    i->dataModel()->setHeaderData(2, Qt::Horizontal, tr("Creation Date"));
    i->dataModel()->setHeaderData(3, Qt::Horizontal, tr("Creation Time"));
    manager.addInterface(i);

    manager.setCurrentIndex(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}
