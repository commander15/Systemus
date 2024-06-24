#ifndef SYSTEMUS_DATA_IMPL_H
#define SYSTEMUS_DATA_IMPL_H

#include <SystemusCore/data.h>

#include <QtSql/qsqldriver.h>

namespace Systemus {

DataSearch::DataSearch()  :
    _page(0), _itemsPerPage(100)
{}

}

#endif // SYSTEMUS_DATA_IMPL_H
