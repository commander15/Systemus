target_headers(Core
    PUBLIC
        sqlquerymodel.h datatablemodel.h
    PRIVATE
        datatablemodel_p.h
)

target_sources(Core
    PRIVATE
        sqlquerymodel.cpp datatablemodel.cpp
)
