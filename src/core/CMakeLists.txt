systemus_add_library(Core
    NAME   Core
    SYSTEM Systemus
)

add_subdirectory(kernel)
add_subdirectory(data)
add_subdirectory(user)

target_include_directories(Core
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
)

target_link_libraries(Core PUBLIC Qt::Core Qt::Sql PRIVATE bcrypt)

if (BUILD_SHARED_LIBS)
    set(BUILD_TYPE SHARED)
else()
    set(BUILD_TYPE STATIC)
endif()

target_compile_definitions(Core PUBLIC SYSTEMUS_${BUILD_TYPE})

install(TARGETS Core
    EXPORT SystemusCoreTargets
    ARCHIVE       DESTINATION lib
    LIBRARY       DESTINATION lib
    RUNTIME       DESTINATION bin
    PUBLIC_HEADER DESTINATION include/SystemusCore
)
