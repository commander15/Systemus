#include <QtCore/qcoreapplication.h>
#include <QtCore/qtextstream.h>

#include <SystemusCore/systemus.h>
#include <SystemusCore/datatablemodel.h>

#include <QtSql/qsqldatabase.h>

#include <QtCore/qelapsedtimer.h>
#include <QtCore/qjsondocument.h>

#include "direction.h"

template<typename T>
Systemus::Orm::DataTableModel *testType(QTextStream &stream)
{
    sRegisterClass<T>();

    const Systemus::Orm::MetaTable table = Systemus::Orm::MetaTable::fromType<T>();
    stream << "Class:" << table.className() << Qt::endl;
    stream << "Table:" << table.tableName() << Qt::endl;
    stream << Qt::endl;

    auto *model = new Systemus::Orm::DataTableModel(qApp);
    model->setClass(table);
    return model;
}

int main(int argc, char *argv[])
{
    QElapsedTimer timer;
    timer.start();

    QCoreApplication app(argc, argv);

    QTextStream out(stdout);

    Systemus::init(app);
    {
        sRegisterClass<Direction>();
    }

    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
        db.setDatabaseName("SodecShield");
        db.setUserName("root");
        db.setHostName("127.0.0.1");
        db.open();
    }

    QString str;
    auto *model = testType<Direction>(out);

    model->select();

    for (int i(0); i < model->rowCount(); ++i) {
        Systemus::Data data = model->item(i);
        data.postGet();

        QJsonDocument doc;
        doc.setObject(data.toJsonObject());
        str.append(doc.toJson(QJsonDocument::Indented) + "\n\n");
    }

    out << str;

    out << "\nProgram comple, processed " << model->rowCount() << " records, "
        << "generated " << str.size() << " caracter(s) text, "
        << "execution time: " << timer.elapsed() << " ms." << Qt::endl;
    return 0;
}

#include "main.moc"
