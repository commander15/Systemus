#include <QtWidgets/qapplication.h>

#include <QtSql/qsqldatabase.h>

#include <SystemusCore/system.h>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    SYSTEMUS_INIT()

    QApplication app(argc, argv);
    app.setApplicationName("Manager");

    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setDatabaseName("Systemus");
    db.setHostName("127.0.0.1");
    db.setUserName("root");
    db.open();

    Systemus::System::instance();

    MainWindow w;
    w.show();

    return app.exec();
}
