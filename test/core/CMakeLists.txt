add_executable(core_test main.cpp sqlquerylist.cpp
    data/datainfotest.h data/datainfotest.cpp
    data/datatest.h data/datatest.cpp
    data/usertest.h data/usertest.cpp)

target_link_libraries(core_test PRIVATE GTest::gtest Qt::Test Core)

add_subdirectory(data)
