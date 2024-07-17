#include "usertest.h"

TEST(UserTest, userGathering)
{
    Systemus::User user = Systemus::User::fromId(1);
    ASSERT_TRUE(user.isValid());

    ASSERT_EQ(user.name().toStdString(), "admin");
    ASSERT_EQ(user.password().toStdString(), "1234");

    ASSERT_TRUE(user.hasRole("Administrator"));
    ASSERT_TRUE(user.inGroup("Administrators"));
}
