#include <QtCore/qcoreapplication.h>
#include <QtCore/qloggingcategory.h>

#include <QtSql/qsqldatabase.h>

#include <SystemusCore/global.h>

#include <gtest/gtest.h>

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);

    QCoreApplication app(argc, argv);

    Systemus::init(argc, argv);

    QLoggingCategory::setFilterRules("systemus.*.debug = true");

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QStringLiteral(TESTDATA_DIR) + "/sample_db.sqlite3");

    return RUN_ALL_TESTS();
}
