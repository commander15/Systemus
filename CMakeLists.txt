cmake_minimum_required(VERSION 3.20)

project(Systemus
    DESCRIPTION "The framework !"
    VERSION     1.0.0
    LANGUAGES   CXX C
)

set(SYSTEMUS_VERSION_CODE 1)

option(Systemus_BUILD_TOOL    "" OFF)
option(Systemus_BUILD_WIDGETS "" OFF)
option(Systemus_BUILD_TEST    "" OFF)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(cmake/Utils.cmake)

find_package(Qt REQUIRED)

add_subdirectory(src)

if (Systemus_BUILD_TEST)
    add_subdirectory(test)
endif()
