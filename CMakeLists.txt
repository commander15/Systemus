cmake_minimum_required(VERSION 3.20)

project(Systemus
    DESCRIPTION "The framework designed for us !"
    VERSION     1.2.0
    LANGUAGES   CXX C
)

set(SYSTEMUS_VERSION_CODE 0)

set(Systemus_LOCALES fr)

set(Systemus_BUILD_TOOL         OFF CACHE BOOL "Build tool")
set(Systemus_BUILD_EXAMPLES     OFF CACHE BOOL "Build examples")
set(Systemus_BUILD_WIDGETS      OFF CACHE BOOL "Build Widgets module")
set(Systemus_BUILD_TEST         OFF CACHE BOOL "Build tests")
set(Systemus_BUILD_TRANSLATIONS OFF CACHE BOOL "Build translations")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED TRUE)

set(C_STANDARD 17)
set(C_STANDARD_REQUIRED TRUE)

include(cmake/Utils.cmake)

find_package(Qt REQUIRED)

add_subdirectory(3rdparty)

set(Systemus_EXPORT TRUE)

add_subdirectory(src)

if (Systemus_BUILD_TEST)
    add_subdirectory(test)
endif()

if (Systemus_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if (Systemus_BUILD_TRANSLATIONS)
    set(TARGETS core)

    if (Systemus_BUILD_WIDGETS)
        list(APPEND TARGETS widgets)
    endif()

    set(TS_FILES ${Systemus_LOCALES})
    list(TRANSFORM TS_FILES PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/translations/systemus_)
    list(TRANSFORM TS_FILES APPEND .ts)
    qt_add_translations(core TARGETS ${TARGETS} SOURCE_TARGETS ${TARGETS}
        TS_FILES ${TS_FILES} QM_FILES_OUTPUT_VARIABLE QM_FILES
    )

    install(FILES ${QM_FILES} DESTINATION translations)
endif()

install(DIRECTORY src/tool/scripts DESTINATION .)
