target_headers(Widgets
    PUBLIC
        userinterface.h
        datainterface.h
        interfaceholder.h
        interfaceserver.h
    PRIVATE
        userinterface_p.h
        datainterface_p.h
        interfaceserver_p.h
)

target_sources(Widgets
    PRIVATE
        userinterface.cpp
        datainterface.cpp datainterface.ui
        interfaceholder.cpp
        interfaceserver.cpp
)
