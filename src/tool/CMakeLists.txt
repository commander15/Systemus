systemus_add_executable(tool main.cpp)

target_sources(tool
  PRIVATE
    command.cpp
    infoscommand.cpp
    res.qrc
    installcommand.h installcommand.cpp
    installcommand.h installcommand.cpp
)

target_link_libraries(tool PRIVATE core)

install(TARGETS tool
    EXPORT SystemusCoreTargets
    RUNTIME DESTINATION bin
)
