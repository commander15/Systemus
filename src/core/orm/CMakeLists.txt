target_headers(Core
    PUBLIC
        orm.h ormbackend.h
        metamapper.h querybuilder.h metatable.h secretproperty.h
        abstractdata.h ormdata.h
        internaldata.h
        datarelation.h
        #authorizationdata.h
        #datamodel.h
        #datatablemodel.h
    PRIVATE
        orm_p.h
        metatable_p.h secretproperty_p.h
        ormdata_p.h
        internaldata_p.h
        #authorizationdata_p.h
        #abstractdatamodel_p.h
        #datatablemodel_p.h
)

target_sources(Core
    PRIVATE
        orm.cpp ormbackend.cpp
        metamapper.cpp querybuilder.cpp metatable.cpp secretproperty.cpp
        abstractdata.cpp ormdata.cpp
        internaldata.cpp
        datarelation.cpp
        #authorizationdata.cpp
        #datatablemodel.cpp
)
