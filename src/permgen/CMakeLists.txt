systemus_add_executable(permgen)

target_sources(permgen
  PRIVATE
    mainwidget.h mainwidget.cpp mainwidget.ui
    main.cpp
)

target_link_libraries(permgen PRIVATE Qt::Widgets)
