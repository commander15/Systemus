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

    QPixmap logo() const;
    void setLogo(const QPixmap &logo, Qt::Orientation orientation);

    void setShowOnLogOut(bool show = true);

    Q_SLOT void done(int r) override;

private:
    Q_SLOT void logIn();
    Q_SLOT void processError(const AuthenticationError &error);
    Q_SLOT void clearError();

    Q_SLOT void toggleView(bool settings);
    Q_SLOT void saveDatabaseSettings();
    Q_SLOT void testDatabaseConnection();

    Q_SLOT void updateSystemData();

    Ui::LoginDialog *ui;

    bool _showOnLogOut;
    QTimer *_cleanTimer;
};

}

#endif // SYSTEMUS_LOGINDIALOG_H
