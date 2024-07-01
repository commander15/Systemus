#include "centralwidget.h"
#include "ui_centralwidget.h"

namespace Systemus {

CentralWidget::CentralWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CentralWidget)
{
    ui->setupUi(this);
}

CentralWidget::~CentralWidget()
{
    delete ui;
}

}
