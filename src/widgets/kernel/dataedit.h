#ifndef SYSTEMUS_DATAEDIT_H
#define SYSTEMUS_DATAEDIT_H

#include <SystemusWidgets/global.h>

#include <SystemusCore/data.h>

#include <QtWidgets/qdialog.h>
#include <QtWidgets/qwidget.h>

namespace Systemus {

namespace Ui {
class DataEditDialog;
}

class DataEditPrivate;
class SYSTEMUS_WIDGETS_EXPORT DataEdit : public QWidget
{
    Q_OBJECT

public:
    explicit DataEdit(QWidget *parent = nullptr);
    virtual ~DataEdit();

    Data data() const;
    Q_SLOT void setData(const Data &data);
    Q_SIGNAL void dataChanged();

    Q_SLOT void clear();

    bool isReadOnly() const;
    void setReadOnly(bool r);

protected:
    DataEdit(DataEditPrivate *d, QWidget *parent);

    virtual Data extractFields() const;
    virtual void fillFields(const Data &data);
    virtual void clearFields();

    virtual void makeFieldsWritable(bool writable = true);

    QScopedPointer<DataEditPrivate> d_ptr;
};

class DataEditDialogPrivate;
class SYSTEMUS_WIDGETS_EXPORT DataEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DataEditDialog(QWidget *parent = nullptr);
    virtual ~DataEditDialog();

    Data data() const;
    Q_SLOT void setData(const Data &data);
    Q_SIGNAL void dataChanged();

    Q_SLOT void clear();

    bool isReadOnly() const;
    void setReadOnly(bool r);

    template<typename T>
    static DataEditDialog *fromEdit(QWidget *parent)
    { return fromEdit(new T(parent), parent); }
    static DataEditDialog *fromEdit(DataEdit *edit, QWidget *parent);

protected:
    DataEditDialog(DataEdit *edit, QWidget *parent = nullptr);
    DataEditDialog(DataEditDialogPrivate *d, QWidget *parent = nullptr);

    virtual Data extractFields() const;
    virtual void fillFields(const Data &data);
    virtual void clearFields();

    virtual void makeFieldsWritable(bool writable = true);

    QWidget *contentWidget;

private:
    QScopedPointer<DataEditDialogPrivate> d;
    Ui::DataEditDialog *ui;

    friend class DefaultDataEditDialogPrivate;
    friend class ForwardedDataEditDialogPrivate;
};

}

#endif // SYSTEMUS_DATAEDIT_H
