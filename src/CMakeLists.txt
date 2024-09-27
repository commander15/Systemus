include(CMakePackageConfigHelpers)

configure_package_config_file(SystemusConfig.cmake.in ${CMAKE_CURRENT_SOURCE_DIR}/SystemusConfig.cmake
    INSTALL_DESTINATION lib/cmake/Systemus
)

write_basic_package_version_file(${CMAKE_CURRENT_SOURCE_DIR}/SystemusConfigVersion.cmake
    VERSION ${Systemus_VERSION} COMPATIBILITY AnyNewerVersion
    ARCH_INDEPENDENT
)

include(SystemusMacros.cmake)

add_subdirectory(core)

if (Systemus_BUILD_TOOL)
    add_subdirectory(tool)
endif()

install(EXPORT SystemusCoreTargets NAMESPACE Systemus:: DESTINATION lib/cmake/Systemus)

if (Systemus_BUILD_WIDGETS)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_AUTOUIC ON)

    add_subdirectory(widgets)

    if (Systemus_BUILD_TOOL)
        add_subdirectory(manager)
        #add_subdirectory(permgen)
    endif()

    install(EXPORT SystemusWidgetsTargets NAMESPACE Systemus:: DESTINATION lib/cmake/Systemus)
endif()

install(FILES SystemusConfig.cmake SystemusConfigVersion.cmake
    SystemusMacros.cmake
    DESTINATION lib/cmake/Systemus
)

install(FILES Systemus DESTINATION include)
