#ifndef SYSTEMUS_SEARCHBAR_H
#define SYSTEMUS_SEARCHBAR_H

#include <SystemusWidgets/global.h>
#include <SystemusWidgets/interfaceholder.h>

#include <QtWidgets/qlineedit.h>

namespace Systemus {

class SearchBar : public QLineEdit, public SimpleInterfaceHolder
{
public:
    explicit SearchBar(QWidget *parent = nullptr);
    ~SearchBar();

    Q_SLOT void search();
    Q_SLOT void search(const QString &query);

private:
    void retranslateUi();

    QAction *_leadingAction;
    QAction *_trailingAction;
};

}

#endif // SYSTEMUS_SEARCHBAR_H
