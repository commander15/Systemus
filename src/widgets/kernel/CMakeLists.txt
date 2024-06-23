target_headers(widgets
    PUBLIC
        global.h
        userinterface.h
        standarduserinterface.h
        tableview.h
    PRIVATE
        userinterface_p.h
        standarduserinterface_p.h
)

target_sources(widgets
    PRIVATE
        userinterface.cpp
        standarduserinterface.cpp
        tableview.cpp
)
