add_executable(core_test main.cpp sqlquerylist.cpp
    orm/sample.h orm/sample.cpp
    orm/metatabletest.h orm/metatabletest.cpp
)

target_link_libraries(core_test PRIVATE GTest::gtest Qt::Test Core)
