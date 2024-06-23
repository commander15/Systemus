configure_file(config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/config.h @ONLY)

target_headers(core
    PUBLIC
        global.h config.h
        #system.h
    PRIVATE
        debug_p.h
)

target_sources(core
    PRIVATE
        debug.cpp
)
