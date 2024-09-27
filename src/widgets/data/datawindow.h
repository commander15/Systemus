#ifndef DATAWINDOW_H
#define DATAWINDOW_H

#include <SystemusWidgets/global.h>
#include <SystemusWidgets/userinterface.h>

#include <QtWidgets/qwidget.h>

namespace Systemus {
class Data;
class DataEdit;

class DataWindowPrivate;
class SYSTEMUS_WIDGETS_EXPORT DataWindow : public UserInterface
{
    Q_OBJECT

public:
    enum WindowType {
        DefaultWindow,
        TabWindow
    };

    explicit DataWindow(QWidget *parent = nullptr);
    DataWindow(int type, QWidget *parent = nullptr);
    virtual ~DataWindow();

    Data data() const;
    void setData(const Data &data);

    DataEdit *edit() const;

    int windowType() const;

protected:
    DataWindow(DataWindowPrivate *d, QWidget *parent = nullptr);

    virtual void initUi(DataEdit *edit);

    QScopedPointer<DataWindowPrivate> d_ptr;
};

}

#endif // DATAWINDOW_H
