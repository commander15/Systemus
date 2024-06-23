systemus_add_library(widgets
    NAME   Widgets
    SYSTEM Systemus
    resources/systemus_widgets_res.qrc
)

add_subdirectory(kernel)
add_subdirectory(user)

target_link_libraries(widgets PUBLIC Qt::Widgets core)

install(TARGETS widgets
    EXPORT SystemusWidgetsTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include/SystemusWidgets
)
