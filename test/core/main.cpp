#include <QtCore/qcoreapplication.h>

#include <QtSql/qsqldatabase.h>

#include <gtest/gtest.h>

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);

    QCoreApplication app(argc, argv);

    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setDatabaseName("Systemus");
    db.setHostName("127.0.0.1");
    db.setPort(3306);
    db.open("root", QString());

    return RUN_ALL_TESTS();
}
