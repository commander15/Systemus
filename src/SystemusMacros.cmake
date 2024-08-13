function(systemus_add_executable name)
    qt_add_executable(${name} ${ARGN})
    _systemus_register_target(${name} EXECUTABLE)
endfunction()

function(systemus_add_library name)
    set(options)
    set(oneValueArgs NAME SYSTEM)
    set(multiValueArgs)

    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (ARG_NAME AND ARG_SYSTEM)
        set(NAME ${ARG_SYSTEM}${ARG_NAME})
        set(MACRO ${ARG_SYSTEM}_${ARG_NAME})
    elseif (ARG_NAME)
        set(NAME ${ARG_NAME})
        set(MACRO ${ARG_NAME})
    else()
        message(FATAL_ERROR "Name is missing")
    endif()
    string(TOUPPER ${MACRO} MACRO)

    add_library(${name} ${ARG_UNPARSED_ARGUMENTS})

    set_target_properties(${name}
        PROPERTIES
            SYSTEMUS_NAME   "${ARG_NAME}"
            SYSTEMUS_SYSTEM "${ARG_SYSTEM}"
            OUTPUT_NAME     "${ARG_SYSTEM}${ARG_NAME}"
    )

    target_compile_definitions(${name}
        PUBLIC
            ${MACRO}_LIB
        PRIVATE
            ${MACRO}_BUILD
    )

    _systemus_register_target(${name} LIBRARY)
endfunction()

function(_systemus_register_target target type)
    set_target_properties(${target} PROPERTIES
        SYSTEMUS_TYPE ${type}
    )
endfunction()

function(target_headers target)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs PUBLIC PRIVATE)

    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(PUBLIC_HEADERS)
    foreach (header ${ARG_PUBLIC})
        if (NOT EXISTS ${header})
            set(header ${CMAKE_CURRENT_SOURCE_DIR}/${header})
        endif()

        list(APPEND PUBLIC_HEADERS ${header})
    endforeach()

    set(PRIVATE_HEADERS)
    foreach (header ${ARG_PRIVATE})
        if (NOT EXISTS ${header})
            set(header ${CMAKE_CURRENT_SOURCE_DIR}/${header})
        endif()

        list(APPEND PRIVATE_HEADERS ${header})
    endforeach()

    set_property(TARGET ${target}
        APPEND PROPERTY PUBLIC_HEADER "${PUBLIC_HEADERS}"
    )

    set_property(TARGET ${target}
        APPEND PROPERTY PRIVATE_HEADER "${PRIVATE_HEADERS}"
    )

    get_target_property(NAME ${target} SYSTEMUS_NAME)
    get_target_property(SYSTEM ${target} SYSTEMUS_SYSTEM)

    generate_nested_headers(${CMAKE_BINARY_DIR}/include/${SYSTEM}${NAME} ${PUBLIC_HEADERS})
    generate_nested_headers(${CMAKE_BINARY_DIR}/include/${SYSTEM}${NAME}/private ${PRIVATE_HEADERS})
endfunction()

function(generate_nested_headers destination)
    foreach (header ${ARGN})
        get_filename_component(filename ${header} NAME)

        if (TRUE OR NOT EXISTS ${destination}/${filename})
            file(RELATIVE_PATH filepath ${destination} ${header})
            file(WRITE ${destination}/${filename} "#include \"${filepath}\"")
        endif()
    endforeach()
endfunction()
