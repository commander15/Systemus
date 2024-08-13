set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(QT_MODULES Core Sql)

if (Systemus_BUILD_TEST)
    list(APPEND QT_MODULES Test)
endif()

if (Systemus_BUILD_TRANSLATIONS)
    list(APPEND QT_MODULES LinguistTools)
endif()

if (Systemus_BUILD_WIDGETS)
    list(APPEND QT_MODULES Widgets)
endif()

find_package(Qt6 COMPONENTS ${QT_MODULES})

set(Qt_FOUND ${Qt6_FOUND})
