target_headers(Widgets
    PUBLIC
        filterwidget.h filteritemwidget.h
        datawindow.h dataedit.h
    PRIVATE
        filterwidget_p.h filteritemwidget.h
        datawindow_p.h dataedit_p.h
)

target_sources(Widgets
    PRIVATE
        filterwidget.cpp filterwidget.ui
        filteritemwidget.cpp
        datawindow.cpp dataedit.cpp dataeditdialog.ui
)
