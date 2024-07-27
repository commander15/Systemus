#include <QtWidgets/qapplication.h>

#include <QtGui/qscreen.h>

#include <QtSql/qsqldatabase.h>

#include <SystemusWidgets/logindialog.h>
#include <SystemusWidgets/useredit.h>

#include <SystemusCore/system.h>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    Systemus::init(argc, argv);
    SYSTEMUS_INIT()

#ifdef Q_OS_LINUX
    qputenv("GTK_THEME", "Adwaita:light");
#endif

    QApplication app(argc, argv);
    app.setApplicationName("Manager");

    QLocale::setDefault(QLocale(QLocale::English, QLocale::Cameroon));

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(DB_FILE);
    db.open();

    Systemus::System *system = Systemus::System::instance();
    system->setHeartBeatInterval(60000);
    system->sync();

    Systemus::LoginDialog login;
    if (!login.exec())
        return 0;
    else {
        Systemus::DataEditDialog *d = Systemus::DataEditDialog::fromEdit<Systemus::UserEdit>(nullptr);
        d->setData(system->user());
        d->setReadOnly(true);
        d->exec();
        delete d;
    }

    MainWindow window;
    {
        const QSize screenSize = app.primaryScreen()->availableSize();
        window.resize(screenSize * 0.7);

        QPoint centerPoint;
        centerPoint.setX((screenSize.width() - window.width()) / 2);
        centerPoint.setY((screenSize.height() - window.height()) / 2);
        window.move(centerPoint);
    }
    window.show();

    return app.exec();
}
