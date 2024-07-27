systemus_add_executable(manager main.cpp mainwindow.cpp)

target_link_libraries(manager PRIVATE widgets)

target_compile_definitions(manager
    PRIVATE
        DB_FILE="${CMAKE_CURRENT_SOURCE_DIR}/database.db"
)
