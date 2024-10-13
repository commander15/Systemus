#include "logindialog.h"
#include "ui_logindialog.h"

#include <SystemusCore/system.h>
#include <SystemusCore/authenticator.h>

#include <QtCore/qversionnumber.h>
#include <QtCore/qtimer.h>

namespace Systemus {

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog),
    m_showOnLogOut(false),
    m_cleanTimer(new QTimer(this))
{
    ui->setupUi(this);

    System *system = System::instance();
    changeLogo();
    connect(system, &System::logoDataChanged, this, &LoginDialog::changeLogo);

    QPixmap logoPixmap;
    logoPixmap.loadFromData(system->logoData(), "PNG");
    if (!logoPixmap.isNull())
        ui->logoOutput->setPixmap(logoPixmap.scaled(320, 80, Qt::KeepAspectRatio));

    ui->loginInput->addAction(ui->actionLogin, QLineEdit::LeadingPosition);

    ui->passwordInput->addAction(ui->actionPassword, QLineEdit::LeadingPosition);
    ui->passwordInput->addAction(ui->actionShowPassword, QLineEdit::TrailingPosition);
    connect(ui->actionShowPassword, &QAction::triggered, this, &LoginDialog::togglePasswordVisibility);

    connect(ui->logInButton, &QAbstractButton::clicked, this, &LoginDialog::logIn);

    ui->errorOutput->clear();

    m_cleanTimer->setInterval(3000);
    m_cleanTimer->setSingleShot(true);
    connect(m_cleanTimer, &QTimer::timeout, this, &LoginDialog::hideError);

    Authenticator *auth = Authenticator::instance();
    connect(auth, &Authenticator::loggedIn, this, [this](const User &user) { accept(); });
    connect(auth, &Authenticator::logInError, this, &LoginDialog::showError);

    connect(this, &QDialog::rejected, qApp, &QCoreApplication::quit);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::showOnLogOut()
{
    setShowOnLogOut(true);
}

void LoginDialog::setShowOnLogOut(bool show)
{
    if (m_showOnLogOut == show)
        return;

    Authenticator *auth = Authenticator::instance();
    if (show)
        connect(auth, &Authenticator::loggedOut, this, &QWidget::show);
    else
        disconnect(auth, &Authenticator::loggedOut, this, &QWidget::show);

    m_showOnLogOut = show;
}

void LoginDialog::setVisible(bool visible)
{
    QDialog::setVisible(visible);

    if (ui->loginInput->text().isEmpty())
        ui->loginInput->setFocus();
    else
        ui->passwordInput->setFocus();
}

void LoginDialog::changeLogo()
{
    const QByteArray logoData = System::instance()->logoData();

    QPixmap pixmap;
    if (!logoData.isEmpty()) {
        pixmap.loadFromData(logoData);
    } else {
        pixmap.load(":/systemus/icons/systemus_full.png");
    }
    ui->logoOutput->setPixmap(pixmap.scaled(ui->logoOutput->size(), Qt::KeepAspectRatio));
}

void LoginDialog::togglePasswordVisibility()
{
    bool hidden = (ui->passwordInput->echoMode() == QLineEdit::Password);
    if (hidden) {
        ui->passwordInput->setEchoMode(QLineEdit::Normal);
        ui->actionShowPassword->setIcon(QIcon(":/systemus/icons/password_hide.png"));
    } else {
        ui->passwordInput->setEchoMode(QLineEdit::Password);
        ui->actionShowPassword->setIcon(QIcon(":/systemus/icons/password_show.png"));
    }
}

void LoginDialog::logIn()
{
    ui->errorOutput->setText("...");
    ui->stackedWidget->setCurrentIndex(1);

    Authenticator *auth = Authenticator::instance();
    auth->logIn(ui->loginInput->text(), ui->passwordInput->text());
    ui->passwordInput->clear();
}

void LoginDialog::showError(const AuthenticationError &error)
{
    ui->errorOutput->setText(error.errorString());
    ui->stackedWidget->setCurrentIndex(1);
    m_cleanTimer->start();

    ui->passwordInput->setFocus();
}

void LoginDialog::hideError()
{
    ui->stackedWidget->setCurrentIndex(0);
}

}
