target_headers(Core
    PUBLIC
        orm.h ormbackend.h
        metatable.h
        data.h
        datarelation.h
        authorizationdata.h
        datamodel.h
        datatablemodel.h
    PRIVATE
        metatable_p.h
        data_p.h
        authorizationdata_p.h
        abstractdatamodel_p.h
        datatablemodel_p.h
)

target_sources(Core
    PRIVATE
        orm.cpp ormbackend.cpp
        metatable.cpp
        data.cpp
        datarelation.cpp
        authorizationdata.cpp
        datatablemodel.cpp
)
