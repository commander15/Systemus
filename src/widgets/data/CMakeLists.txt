target_headers(Widgets
    PUBLIC
        filterwidget.h filteritemwidget.h
        dataedit.h
    PRIVATE
        filterwidget_p.h filteritemwidget.h
        dataedit_p.h
)

target_sources(Widgets
    PRIVATE
        #filterwidget.cpp filterwidget.ui
        filteritemwidget.cpp
        dataedit.cpp dataeditdialog.ui
)
