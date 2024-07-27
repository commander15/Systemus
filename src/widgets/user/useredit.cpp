#include "useredit.h"
#include "ui_useredit.h"

#include <SystemusCore/abstractdatamodel.h>
#include <SystemusCore/role.h>

namespace Systemus {

UserEdit::UserEdit(QWidget *parent) :
    ui(new Ui::UserEdit),
    _roleModel(nullptr)
{
    ui->setupUi(this);
}

UserEdit::~UserEdit()
{
    delete ui;
}

User UserEdit::user() const
{
    return data().to<User>();
}

void UserEdit::setUser(const User &user)
{
    setData(user);
}

AbstractDataModel *UserEdit::roleModel() const
{
    return static_cast<AbstractDataModel *>(ui->roleInput->model());
}

void UserEdit::setRoleModel(AbstractDataModel *model)
{
    ui->roleInput->setModel(model);
}

Data UserEdit::extractFields() const
{
    User user = _user;
    user.setName(ui->nameInput->text());

    if (!ui->passwordInput->text().isEmpty())
        user.setPassword(ui->passwordInput->text(), true);

    AbstractDataModel *roleModel = this->roleModel();
    if (roleModel) {
        // Setting role !
    }

    user.setActive(ui->activeYesInput->isChecked());

    UserProfile profile = _user.profile();
    profile.setName(ui->profileNameInput->text());
    profile.setFirstName(ui->firstNameInput->text());
    user.setProfile(profile);

    return user;
}

void UserEdit::fillFields(const Data &data)
{
    User user = data.to<User>();
    ui->nameInput->setText(user.name());
    ui->passwordInput->clear();

    AbstractDataModel *roleModel = this->roleModel();
    if (roleModel) {
        for (int i(0); i < roleModel->rowCount(); ++i) {
            if (user.hasRole(roleModel->item(i).to<Role>().name())) {
                ui->roleInput->setCurrentIndex(i);
                break;
            }
        }
    }

    if (user.isActive())
        ui->activeYesInput->setChecked(true);
    else
        ui->activeNoInput->setChecked(true);

    ui->sinceOutput->setDate(user.creationDate());
    ui->sinceOutput->setTime(user.creationTime());

    UserProfile profile = user.profile();
    ui->profileNameInput->setText(profile.name());
    ui->firstNameInput->setText(profile.firstName());

    _user = user;
}

void UserEdit::clearFields()
{
    ui->nameInput->clear();
    ui->passwordInput->clear();
    ui->roleInput->clear();
    ui->activeNoInput->setChecked(true);
    ui->sinceOutput->clear();

    ui->profileNameInput->clear();
    ui->firstNameInput->clear();
    ui->sexUnknownInput->setChecked(true);
}

void UserEdit::makeFieldsWritable(bool writable)
{
    ui->nameInput->setReadOnly(!writable);
    ui->passwordInput->setReadOnly(!writable);
    ui->roleInput->setEnabled(writable);
    ui->activeInput->setEnabled(writable);
    ui->sinceOutput->setReadOnly(!writable);

    ui->profileNameInput->setReadOnly(!writable);
    ui->firstNameInput->setReadOnly(!writable);
    ui->sexInput->setEnabled(writable);
}

}
