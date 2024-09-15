set(TESTDATA_DIR ${CMAKE_CURRENT_BINARY_DIR}/testdata)
file(COPY ${Systemus_SOURCE_DIR}/testdata/ DESTINATION ${TESTDATA_DIR})

enable_testing()

find_package(GTest REQUIRED)

add_compile_definitions(TESTDATA_DIR="${TESTDATA_DIR}")

include_directories(include)

add_subdirectory(core)
