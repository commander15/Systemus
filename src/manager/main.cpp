#include <QtWidgets/qapplication.h>

#include <QtSql/qsqldatabase.h>

#include <SystemusCore/system.h>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    Systemus::init(argc, argv);
    SYSTEMUS_INIT()

    QApplication app(argc, argv);
    app.setApplicationName("Manager");

    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setDatabaseName("Systemus");
    db.setHostName("127.0.0.1");
    db.setUserName("root");
    db.open();

    Systemus::System *system = Systemus::System::instance();
    system->sync();

    qDebug() << system->setting("screenCount");

    MainWindow w;
    w.setWindowTitle(system->setting("app").toString() + " on screen " + system->setting("screenCount").toString());
    w.show();

    return app.exec();
}
