#ifndef SYSTEMUS_LOGINDIALOG_H
#define SYSTEMUS_LOGINDIALOG_H

#include <SystemusWidgets/global.h>

#include <QtWidgets/qdialog.h>

class QTimer;

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

    void showOnLogOut();
    void setShowOnLogOut(bool show);

    Q_SLOT void setVisible(bool visible) override;

private:
    Q_SLOT void changeLogo();

    Q_SLOT void togglePasswordVisibility();

    Q_SLOT void logIn();
    Q_SLOT void showError(const AuthenticationError &error);
    Q_SLOT void hideError();

    Ui::LoginDialog *ui;

    bool m_showOnLogOut;
    QTimer *m_cleanTimer;
};

}

#endif // SYSTEMUS_LOGINDIALOG_H
