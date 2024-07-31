#include "usertest.h"

#include "../sqlquerylist.h"

TEST(UserTest, userGathering)
{
    SqlQueryList queries;

    Systemus::User user = Systemus::User::fromId(1);

    ASSERT_GE(queries.size(), 1);
    ASSERT_EQ(queries.at(0).toStdString(),
              "SELECT id, name, description, creation_date, creation_time, password, active, profile_id, role_id "
              "FROM Users "
              "WHERE id = 1 LIMIT 1");

    ASSERT_EQ(user.id(), 1);

    ASSERT_GE(queries.size(), 2);
    ASSERT_EQ(queries.at(1).toStdString(), "SELECT Privileges.id, name, description, creation_date, creation_time "
                                           "FROM Privileges "
                                           "INNER JOIN UserPrivileges ON UserPrivileges.privilege_id = Privileges.id "
                                           "WHERE user_id = 1");

    ASSERT_GE(queries.size(), 3);
    ASSERT_EQ(queries.at(2).toStdString(), "SELECT Permissions.id, name, description, creation_date, creation_time "
                                           "FROM Permissions "
                                           "INNER JOIN UserPermissions ON UserPermissions.permission_id = Permissions.id "
                                           "WHERE user_id = 1");

    ASSERT_TRUE(user.isValid());

    ASSERT_EQ(user.name().toStdString(), "admin");
    ASSERT_EQ(user.password().toStdString(), "1234");

    ASSERT_TRUE(user.hasRole("Administrator"));
    ASSERT_TRUE(user.inGroup("Administrators"));
}
