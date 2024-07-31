#ifndef SQLQUERYLIST_H
#define SQLQUERYLIST_H

#include <QtCore/qstringlist.h>

class SqlQueryList : public QStringList
{
public:
    SqlQueryList();
    ~SqlQueryList();

    static void add(QString query);

private:
    static QVector<SqlQueryList *> _lists;
};

#endif // SQLQUERYLIST_H
