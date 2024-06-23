#ifndef SYSTEMUS_LOGINDIALOG_H
#define SYSTEMUS_LOGINDIALOG_H

#include <SystemusWidgets/global.h>

#include <QtWidgets/qdialog.h>

namespace Systemus {

namespace Ui {
class LoginDialog;
}

class AuthenticationError;

class SYSTEMUS_WIDGETS_EXPORT LoginDialog : public QDialog
{
    Q_OBJECT

public:
    LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

private:
    Q_SLOT void logIn();
    Q_SLOT void processError(const AuthenticationError &error);

    Ui::LoginDialog *ui;
};

}

#endif // SYSTEMUS_LOGINDIALOG_H
