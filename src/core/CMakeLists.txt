systemus_add_library(core
    NAME   Core
    SYSTEM Systemus
)

add_subdirectory(kernel)
add_subdirectory(orm)
add_subdirectory(user)

target_include_directories(core
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
)

target_link_libraries(core PUBLIC Qt::Core Qt::Sql)

install(TARGETS core
    EXPORT SystemusCoreTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include/SystemusCore
)
