#include "actionwidget.h"

#include <SystemusWidgets/userinterface.h>

#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qtoolbutton.h>

namespace Systemus {

ActionWidget::ActionWidget(QWidget *parent) :
    ActionWidget(Qt::Horizontal, parent)
{
}

ActionWidget::ActionWidget(Qt::Orientation orientation, QWidget *parent) :
    QWidget(parent),
    _layout(new QBoxLayout((orientation == Qt::Horizontal ? QBoxLayout::LeftToRight : QBoxLayout::TopToBottom), parent)),
    _refreshButton(new QToolButton(this)),
    _addButton(new QToolButton(this)),
    _editButton(new QToolButton(this)),
    _deleteButton(new QToolButton(this)),
    _printButton(new QToolButton(this))
{
    _refreshButton->setIcon(QIcon(":/systemus/icons/action_refresh.png"));
    _layout->addWidget(_refreshButton);
    connect(_refreshButton, &QAbstractButton::clicked, this, &ActionWidget::refreshData);

    _addButton->setIcon(QIcon(":/systemus/icons/action_add.png"));
    _layout->addWidget(_addButton);
    connect(_addButton, &QAbstractButton::clicked, this, &ActionWidget::addData);

    _editButton->setIcon(QIcon(":/systemus/icons/action_edit.png"));
    _layout->addWidget(_editButton);
    connect(_editButton, &QAbstractButton::clicked, this, &ActionWidget::editData);

    _deleteButton->setIcon(QIcon(":/systemus/icons/action_delete.png"));
    _layout->addWidget(_deleteButton);
    connect(_deleteButton, &QAbstractButton::clicked, this, &ActionWidget::deleteData);

    _printButton->setIcon(QIcon(":/systemus/icons/action_print.png"));
    _layout->addWidget(_printButton);
    connect(_printButton, &QAbstractButton::clicked, this, &ActionWidget::printData);

    retranslateUi();
}

ActionWidget::~ActionWidget()
{
}

void ActionWidget::processInterfaceChange(UserInterface *from, UserInterface *to)
{
    if (from) {
        disconnect(from, &UserInterface::actionSupportUpdated, this, &ActionWidget::updateUi);
        updateUi(UserInterface::RefreshAction, false);
        updateUi(UserInterface::AddAction, false);
        updateUi(UserInterface::EditAction, false);
        updateUi(UserInterface::DeleteAction, false);
        updateUi(UserInterface::PrintAction, false);
    }

    if (to) {
        updateUi(UserInterface::RefreshAction, to->supportAction(UserInterface::RefreshAction));
        updateUi(UserInterface::AddAction, to->supportAction(UserInterface::AddAction));
        updateUi(UserInterface::EditAction, to->supportAction(UserInterface::EditAction));
        updateUi(UserInterface::DeleteAction, to->supportAction(UserInterface::DeleteAction));
        updateUi(UserInterface::PrintAction, to->supportAction(UserInterface::PrintAction));
        connect(to, &UserInterface::actionSupportUpdated, this, &ActionWidget::updateUi);
    }
}

void ActionWidget::updateUi(int action, bool supported)
{
    switch (action) {
    case UserInterface::RefreshAction:
        _refreshButton->setEnabled(supported);
        break;


    case UserInterface::AddAction:
        _addButton->setEnabled(supported);
        break;


    case UserInterface::EditAction:
        _editButton->setEnabled(supported);
        break;


    case UserInterface::DeleteAction:
        _deleteButton->setEnabled(supported);
        break;


    case UserInterface::PrintAction:
        _printButton->setEnabled(supported);
        break;

    default:
        break;
    }
}

void ActionWidget::retranslateUi()
{
    _refreshButton->setText(tr("Refresh"));
    _addButton->setText(tr("Add"));
    _editButton->setText(tr("Edit"));
    _deleteButton->setText(tr("Delete"));
    _printButton->setText(tr("Print"));
}

void ActionWidget::refreshData()
{
    currentInterface()->trigger(UserInterface::RefreshAction);
}

void ActionWidget::addData()
{
    currentInterface()->trigger(UserInterface::AddAction);
}

void ActionWidget::editData()
{
    currentInterface()->trigger(UserInterface::EditAction);
}

void ActionWidget::deleteData()
{
    currentInterface()->trigger(UserInterface::DeleteAction);
}

void ActionWidget::printData()
{
    currentInterface()->trigger(UserInterface::PrintAction);
}

}
