systemus_add_library(Core
    NAME   Core
    SYSTEM Systemus
    resources/systemus_core_res.qrc
)

add_subdirectory(kernel)
add_subdirectory(orm)
add_subdirectory(user)
add_subdirectory(model)

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

target_compile_definitions(Core
    PUBLIC
        SYSTEMUS_${BUILD_TYPE}
    PRIVATE
        SYSTEMUS_PRIMARY_CHECKER=hasId
        SYSTEMUS_PRIMARY_GETTER=id
        SYSTEMUS_PRIMARY_SETTER=setId
        SYSTEMUS_PRIMARY_TYPE=int
)

install(TARGETS Core
    EXPORT SystemusCoreTargets
    ARCHIVE       DESTINATION lib
    LIBRARY       DESTINATION lib
    RUNTIME       DESTINATION bin
    PUBLIC_HEADER DESTINATION include/SystemusCore
)
