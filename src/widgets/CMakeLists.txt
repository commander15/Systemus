systemus_add_library(Widgets
    NAME   Widgets
    SYSTEM Systemus
    resources/systemus_widgets_res.qrc
)

add_subdirectory(kernel)
#add_subdirectory(window)
add_subdirectory(interface)
add_subdirectory(data)
add_subdirectory(container)
#add_subdirectory(view)
add_subdirectory(user)

target_link_libraries(Widgets PUBLIC Qt::Widgets Core)

install(TARGETS Widgets
    EXPORT SystemusWidgetsTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include/SystemusWidgets
)
