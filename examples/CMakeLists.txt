systemus_add_executable(Example main.cpp)

target_sources(Example
  PRIVATE
    data.h data.cpp
    direction.h direction.cpp
)

target_link_libraries(Example PRIVATE Core)

target_compile_definitions(Example
    PRIVATE
        DB_FILE="${CMAKE_CURRENT_SOURCE_DIR}/sample.db"
)
