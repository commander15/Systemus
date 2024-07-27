#ifndef SYSTEMUS_USEREDIT_H
#define SYSTEMUS_USEREDIT_H

#include <SystemusWidgets/global.h>
#include <SystemusWidgets/dataedit.h>

#include <SystemusCore/user.h>

namespace Systemus {

namespace Ui {
class UserEdit;
}

class AbstractDataModel;

class SYSTEMUS_WIDGETS_EXPORT UserEdit : public Systemus::DataEdit
{
    Q_OBJECT

public:
    explicit UserEdit(QWidget *parent = nullptr);
    ~UserEdit();

    User user() const;
    void setUser(const User &user);

    AbstractDataModel *roleModel() const;
    void setRoleModel(AbstractDataModel *model);

protected:
    Data extractFields() const override;
    void fillFields(const Data &data) override;
    void clearFields() override;
    void makeFieldsWritable(bool writable) override;

private:
    Ui::UserEdit *ui;
    User _user;
};

}

#endif // SYSTEMUS_USEREDIT_H
