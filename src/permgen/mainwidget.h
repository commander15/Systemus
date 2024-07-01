#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

private slots:
    void on_tabWidget_currentChanged(int index);

    void on_addButton_clicked();

    void on_copyButton_clicked();

    void on_populateButton_clicked();

    void on_removeButton_clicked();

private:
    void populate(int row, const QString &name, const QString &desc);
    QString generateScript();

    Ui::MainWidget *ui;
};

#endif // MAINWIDGET_H
