#include <QtCore/qcoreapplication.h>
#include <QtCore/qloggingcategory.h>

#include <QtSql/qsqldatabase.h>

#include <SystemusCore/systemus.h>

#include <gtest/gtest.h>

#include "sqlquerylist.h"

static QtMessageHandler handler;
void messageFilter(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    if (type == QtMsgType::QtInfoMsg && context.category == QStringLiteral("systemus.sql"))
        SqlQueryList::add(msg);
    else
        handler(type, context, msg);
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);

    QCoreApplication app(argc, argv);

    Systemus::init(app);

    handler = qInstallMessageHandler(messageFilter);

    QLoggingCategory::setFilterRules("systemus.sql.debug = true");

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QStringLiteral(TESTDATA_DIR) + "/sample_db.sqlite3");

    return RUN_ALL_TESTS();
}
