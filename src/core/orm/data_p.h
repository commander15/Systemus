namespace Systemus {

class DataPrivate : public QSharedData
{
public:
    QSqlRecord table;
    QSqlError lastError;
};

}
