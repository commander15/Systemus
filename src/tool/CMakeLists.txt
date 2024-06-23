systemus_add_executable(tool main.cpp system.cpp)

target_link_libraries(tool PRIVATE core)

install(TARGETS tool
    EXPORT SystemusCoreTargets
    RUNTIME DESTINATION bin
)
