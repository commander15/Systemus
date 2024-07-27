#ifndef SYSTEMUS_DATAEDIT_P_H
#define SYSTEMUS_DATAEDIT_P_H

#include <SystemusWidgets/dataedit.h>

namespace Systemus {

class DataEditDialogPrivate
{
public:
    DataEditDialogPrivate(DataEditDialog *q);
    virtual ~DataEditDialogPrivate();

    virtual void init();

    virtual Data data() const = 0;
    virtual void setData(const Data &data) = 0;
    virtual void clear() = 0;

    virtual bool isReadOnly() const = 0;
    virtual void setReadOnly(bool r) = 0;

    DataEditDialog *q;
};

class DefaultDataEditDialogPrivate : public DataEditDialogPrivate
{
public:
    DefaultDataEditDialogPrivate(DataEditDialog *q);

    Data data() const override;
    void setData(const Data &data) override;
    void clear() override;

    bool isReadOnly() const override;
    void setReadOnly(bool r) override;

    bool readOnly;

    int dataId;
};

class ForwardedDataEditDialogPrivate : public DataEditDialogPrivate
{
public:
    ForwardedDataEditDialogPrivate(DataEdit *edit, DataEditDialog *q);

    void init() override;

    Data data() const override;
    void setData(const Data &data) override;
    void clear() override;

    bool isReadOnly() const override;
    void setReadOnly(bool r) override;

    DataEdit *edit;
};

class DataEditPrivate
{
public:
    DataEditPrivate(DataEdit *q);
    virtual ~DataEditPrivate();

    DataEdit *q_ptr;

    int dataId;

    bool readOnly;
};

}

#endif // SYSTEMUS_DATAEDIT_P_H
