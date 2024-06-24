target_headers(core
    PUBLIC
        data.h
        data_impl.h
        datamodel.h
    PRIVATE
        data_p.h
        datamodel_p.h
)

target_sources(core
    PRIVATE
        data.cpp
        datamodel.cpp
)
