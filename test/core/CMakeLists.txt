add_executable(core_test main.cpp
    data/datainfotest.h data/datainfotest.cpp
    data/datatest.h data/datatest.cpp)

target_link_libraries(core_test PRIVATE GTest::gtest Qt::Test core)

add_subdirectory(data)
