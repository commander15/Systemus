target_headers(core
    PUBLIC
        data.h
        datarelation.h datarelation_impl.h
        authorizationdata.h
        datamodel.h
    PRIVATE
        data_p.h
        authorizationdata_p.h
        datamodel_p.h
)

target_sources(core
    PRIVATE
        data.cpp
        authorizationdata.cpp
        datamodel.cpp
)
