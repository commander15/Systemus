#include "logindialog.h"
#include "ui_logindialog.h"

#include <SystemusCore/authenticator.h>
#include <SystemusCore/system.h>

#include <QtSql/qsqldatabase.h>

#include <QtCore/qversionnumber.h>
#include <QtCore/qtimer.h>

namespace Systemus {

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog),
    _showOnLogOut(false),
    _cleanTimer(new QTimer(this))
{
    ui->setupUi(this);

    System *system = System::instance();

    QPixmap logoPixmap = QPixmap::fromImage(system->logo());
    if (!logoPixmap.isNull())
        ui->systemLogoOutput->setPixmap(logoPixmap.scaled(320, 80, Qt::KeepAspectRatio));

    ui->loginInput->addAction(ui->actionLogin, QLineEdit::LeadingPosition);

    ui->passwordInput->addAction(ui->actionPassword, QLineEdit::LeadingPosition);
    ui->passwordInput->addAction(ui->actionShowPassword, QLineEdit::TrailingPosition);
    connect(ui->actionShowPassword, &QAction::triggered, this, &LoginDialog::togglePasswordVisibility);

    connect(ui->connectButton, &QAbstractButton::clicked, this, &LoginDialog::logIn);
    connect(ui->settingsButton, &QAbstractButton::toggled, this, &LoginDialog::toggleView);

    QSqlDatabase db = QSqlDatabase::database();
    ui->databaseHostInput->setText(db.hostName());
    ui->databasePortInput->setValue(db.port());
    ui->databaseUserInput->setText(db.userName());
    ui->databasePasswordInput->setText(db.password());
    ui->databaseNameInput->setText(db.databaseName());

    connect(ui->databaseSaveButton, &QAbstractButton::clicked, this, &LoginDialog::saveDatabaseSettings);
    connect(ui->databaseTestButton, &QAbstractButton::clicked, this, &LoginDialog::testDatabaseConnection);

    ui->messageTextOutput->clear();

    _cleanTimer->setInterval(3000);
    _cleanTimer->setSingleShot(true);
    connect(_cleanTimer, &QTimer::timeout, this, &LoginDialog::clearError);

    Authenticator *auth = Authenticator::instance();
    connect(auth, &Authenticator::loggedIn, this, [this](const User &) { accept(); });
    connect(auth, &Authenticator::logInError, this, &LoginDialog::processError);

    connect(System::instance(), &System::online, this, &LoginDialog::updateSystemData);
    connect(System::instance(), &System::online, this, &QWidget::show);

    connect(this, &QDialog::rejected, qApp, &QCoreApplication::quit);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

QPixmap LoginDialog::logo() const
{
    return ui->systemLogoOutput->pixmap();
}

void LoginDialog::setLogo(const QPixmap &logo, Qt::Orientation orientation)
{
    if (orientation == Qt::Horizontal)
        ui->systemLogoOutput->setPixmap(logo.scaled(QSize(256, 80), Qt::KeepAspectRatio));
    else
        ui->systemLogoOutput->setPixmap(logo.scaled(QSize(80, 80), Qt::KeepAspectRatio));
}

void LoginDialog::setShowOnLogOut(bool show)
{
    if (_showOnLogOut == show)
        return;

    Authenticator *auth = Authenticator::instance();
    if (show)
        connect(auth, &Authenticator::loggedOut, this, &QWidget::show);
    else
        disconnect(auth, &Authenticator::loggedOut, this, &QWidget::show);

    _showOnLogOut = show;
}

void LoginDialog::showLogin()
{
    toggleView(false);
    show();
}

void LoginDialog::showSettings()
{
    toggleView(true);
    show();
}

void LoginDialog::done(int r)
{
    ui->passwordInput->clear();
    QDialog::done(r);
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
    Authenticator *auth = Authenticator::instance();
    auth->logIn(ui->loginInput->text(), ui->passwordInput->text());
}

void LoginDialog::processError(const AuthenticationError &error)
{
    ui->messageTextOutput->setText(error.errorString());
    _cleanTimer->start();
}

void LoginDialog::clearError()
{
    ui->messageTextOutput->clear();
}

void LoginDialog::toggleView(bool settings)
{
    ui->stackedWidget->setCurrentIndex(settings ? 1 : 0);
}

void LoginDialog::saveDatabaseSettings()
{
    QSqlDatabase db = QSqlDatabase::database();
    db.setHostName(ui->databaseHostInput->text());
    db.setPort(ui->databasePortInput->value());
    db.setUserName(ui->databaseUserInput->text());
    db.setPassword(ui->databasePasswordInput->text());
    db.setDatabaseName(ui->databaseNameInput->text());
}

void LoginDialog::testDatabaseConnection()
{
    const QString connection = QStringLiteral("SYSTEMUS_TEST_CONNECTION");
    {
        QSqlDatabase db = QSqlDatabase::addDatabase(QSqlDatabase::database().driverName(), connection);
        db.setHostName(ui->databaseHostInput->text());
        db.setPort(ui->databasePortInput->value());
        db.setUserName(ui->databaseUserInput->text());
        db.setPassword(ui->databasePasswordInput->text());
        db.setDatabaseName(ui->databaseNameInput->text());

        if (db.open()) {
            ui->databaseTestResultOutput->setText(tr("Connection succeded !"));
            ui->databaseTestResultOutput->setStyleSheet("color: green");
            db.close();
        } else {
            ui->databaseTestResultOutput->setText(tr("Connection failed !"));
            ui->databaseTestResultOutput->setStyleSheet("color: red");
        }
    }
    QSqlDatabase::removeDatabase(connection);
}

void LoginDialog::updateSystemData()
{
    System *system = System::instance();
    ui->systemNameOutput2->setText(system->name());
    ui->systemVersionOutput->setText(system->version().toString());
}

}
