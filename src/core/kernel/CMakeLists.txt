configure_file(config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/config.h @ONLY)

target_headers(Core
    PUBLIC
        systemus.h global.h config.h
        namespace.h
        system.h
    PRIVATE
        debug_p.h
        system_p.h
)

target_sources(Core
    PRIVATE
        debug.cpp
        namespace.cpp
        system.cpp
)
