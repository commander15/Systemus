set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(QT_MODULES Core Sql)

if (Systemus_BUILD_WIDGETS)
    list(APPEND QT_MODULES Widgets)
endif()

find_package(Qt6 COMPONENTS ${QT_MODULES})

set(Qt_FOUND ${Qt6_FOUND})
