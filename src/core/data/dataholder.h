#ifndef SYSTEMUS_DATAHOLDER_H
#define SYSTEMUS_DATAHOLDER_H

#include <SystemusCore/global.h>
#include <SystemusCore/data.h>

namespace Systemus {

template<typename T>
class SYSTEMUS_CORE_EXPORT DataHolder
{
public:
    DataHolder(const T &data) :
        _data(data)
    {}

    const Data &data() const
    { return _data; }

    Data &data()
    { return _data; }

    const T &operator*() const
    { return _data; }

    T &operator*()
    { return _data; }

private:
    T _data;
};

template<typename T>
class SYSTEMUS_CORE_EXPORT DataList : public QList<T>
{
public:
    const Data &dataAt(int index) const
    { return QList<T>::at(index); }

    Data &dataAt(int index)
    { return QList<Data>::operator[](index); }
};

}

#endif // SYSTEMUS_DATAHOLDER_H
