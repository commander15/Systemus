#include "searchbar.h"

#include <SystemusWidgets/userinterface.h>

namespace Systemus {

SearchBar::SearchBar(QWidget *parent) :
    QLineEdit(parent),
    _leadingAction(new QAction(QIcon(":/systemus/icons/search_icon.png"), QString(), this)),
    _trailingAction(new QAction(QIcon(":/systemus/icons/search_go.png"), QString(), this))
{
    addAction(_leadingAction, LeadingPosition);

    connect(_trailingAction, &QAction::triggered, this, QOverload<>::of(&SearchBar::search));
    addAction(_trailingAction, TrailingPosition);

    setMaxLength(30);
    setClearButtonEnabled(true);

    connect(this, &QLineEdit::returnPressed, this, QOverload<>::of(&SearchBar::search));

    retranslateUi();
}

SearchBar::~SearchBar()
{
}

void SearchBar::search()
{
    search(text());
}

void SearchBar::search(const QString &query)
{
    setText(query);

    UserInterface *ui = currentInterface();
    if (ui)
        ui->trigger(UserInterface::SearchAction, query);
}

void SearchBar::retranslateUi()
{
    setPlaceholderText(tr("Search..."));

    _trailingAction->setText(tr("Go"));
}

}
