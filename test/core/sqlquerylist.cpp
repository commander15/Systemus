#include "sqlquerylist.h"

SqlQueryList::SqlQueryList()
{
    _lists.append(this);
}

SqlQueryList::~SqlQueryList()
{
    _lists.removeOne(this);
}

void SqlQueryList::add(QString query)
{
    query.remove('"');
    for (SqlQueryList *list : _lists)
        list->append(query);
}

QVector<SqlQueryList *> SqlQueryList::_lists;
