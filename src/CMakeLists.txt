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

if (Systemus_BUILD_TRANSLATIONS)
    set(TARGETS Core)

    if (Systemus_BUILD_WIDGETS)
        list(APPEND TARGETS Widgets)
    endif()

    set(TS_FILES ${Systemus_LOCALES})
    list(TRANSFORM TS_FILES PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/../translations/systemus_)
    list(TRANSFORM TS_FILES APPEND .ts)
    qt_add_translations(Core SOURCE_TARGETS ${TARGETS}
        TS_FILES ${TS_FILES} QM_FILES_OUTPUT_VARIABLE QM_FILES
    )

    install(FILES ${QM_FILES} DESTINATION translations)
endif()
