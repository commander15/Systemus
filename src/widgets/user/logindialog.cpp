#include "logindialog.h"
#include "ui_logindialog.h"

#include <SystemusCore/authenticator.h>

namespace Systemus {

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);

    connect(ui->connectButton, &QAbstractButton::clicked, this, &LoginDialog::logIn);

    Authenticator *auth = Authenticator::instance();
    connect(auth, &Authenticator::loggedIn, this, [this](const User &) { accept(); });
    connect(auth, &Authenticator::logInError, this, &LoginDialog::processError);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::logIn()
{
    Authenticator *auth = Authenticator::instance();
    auth->logIn(ui->loginInput->text(), ui->passwordInput->text());
}

void LoginDialog::processError(const AuthenticationError &error)
{
    ui->messageTextOutput->setText(error.errorString());
}

}
