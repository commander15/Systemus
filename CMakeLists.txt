cmake_minimum_required(VERSION 3.20)

project(Systemus
    DESCRIPTION "The framework designed for us !"
    VERSION     1.0.0
    LANGUAGES   CXX C
)

set(SYSTEMUS_VERSION_CODE 0)

set(Systemus_LOCALES fr)

option(Systemus_BUILD_TOOL         "" OFF)
option(Systemus_BUILD_WIDGETS      "" OFF)
option(Systemus_BUILD_TEST         "" OFF)
option(Systemus_BUILD_TRANSLATIONS "" OFF)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(cmake/Utils.cmake)

find_package(Qt REQUIRED)

add_subdirectory(3rdparty)

add_subdirectory(src)

if (Systemus_BUILD_TEST)
    add_subdirectory(test)
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
