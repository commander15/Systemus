#ifndef DATAWINDOW_H
#define DATAWINDOW_H

#include <SystemusWidgets/global.h>
#include <SystemusWidgets/userinterface.h>

#include <QtWidgets/qwidget.h>

namespace Systemus {
class Data;
class DataEdit;
class DataEditDialog;

class DataWindowPrivate;
class SYSTEMUS_WIDGETS_EXPORT DataWindow : public UserInterface
{
    Q_OBJECT

public:
    enum WindowType {
        DefaultWindow
    };

    explicit DataWindow(QWidget *parent = nullptr);
    virtual ~DataWindow();

    Data data() const;
    void setData(const Data &data);
    Q_SIGNAL void dataChanged(const Data &data);

    DataEdit *edit() const;
    virtual QWidget *container() const;

    virtual int windowType() const;

protected:
    DataWindow(DataWindowPrivate *d, QWidget *parent = nullptr);

    void initWindow(DataEdit *edit);

    virtual void display(const Data &data);

    QScopedPointer<DataWindowPrivate> d_ptr;
};

}

#endif // DATAWINDOW_H
