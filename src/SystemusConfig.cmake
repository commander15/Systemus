
####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was SystemusConfig.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

macro(check_required_components _NAME)
  foreach(comp ${${_NAME}_FIND_COMPONENTS})
    if(NOT ${_NAME}_${comp}_FOUND)
      if(${_NAME}_FIND_REQUIRED_${comp})
        set(${_NAME}_FOUND FALSE)
      endif()
    endif()
  endforeach()
endmacro()

####################################################################################

if (EXISTS ${CMAKE_CURRENT_LIST_DIR}/SystemusExtraTargets.cmake)
    include(${CMAKE_CURRENT_LIST_DIR}/SystemusExtraTargets.cmake)
endif()

include(${CMAKE_CURRENT_LIST_DIR}/SystemusMacros.cmake)

# Core
set(Systemus_PRIMARY_NAME "id"  CACHE STRING "Default primary property name")
set(Systemus_PRIMARY_TYPE "int" CACHE STRING "Default primary property type")

# Ensure variables are set and not empty
if(NOT Systemus_PRIMARY_NAME STREQUAL "" AND NOT Systemus_PRIMARY_TYPE STREQUAL "")

    # Convert the variable names to upper case
    string(TOUPPER "Systemus_PRIMARY_NAME" SYSTEMUS_PRIMARY_NAME_UPPER)
    string(TOUPPER "Systemus_PRIMARY_TYPE" SYSTEMUS_PRIMARY_TYPE_UPPER)

    # Convert the first letter of Systemus_PRIMARY_NAME to upper case to form SYSTEMUS_PRIMARY_NAME2
    string(SUBSTRING "${Systemus_PRIMARY_NAME}" 0  1 FIRST_CHAR)
    string(SUBSTRING "${Systemus_PRIMARY_NAME}" 1 -1 REST_OF_NAME)
    string(TOUPPER "${FIRST_CHAR}" FIRST_CHAR)
    set(SYSTEMUS_PRIMARY_NAME2 "${FIRST_CHAR}${REST_OF_NAME}")

    # Add global compile definitions
    add_compile_definitions(
        SYSTEMUS_PRIMARY_NAME1=${Systemus_PRIMARY_NAME}
        SYSTEMUS_PRIMARY_NAME2=${SYSTEMUS_PRIMARY_NAME2}
        SYSTEMUS_PRIMARY_TYPE=${Systemus_PRIMARY_TYPE}
    )
endif()

include(${CMAKE_CURRENT_LIST_DIR}/SystemusCoreTargets.cmake)
set(Systemus_Qt_COMPONENTS Core Sql)

# Widgets
if (EXISTS ${CMAKE_CURRENT_LIST_DIR}/SystemusWidgetsTargets.cmake)
    include(${CMAKE_CURRENT_LIST_DIR}/SystemusWidgetsTargets.cmake)
    list(APPEND Systemus_Qt_COMPONENTS Widgets)
endif()

macro(find_qt version)
    find_package(Qt${version} COMPONENTS ${Systemus_Qt_COMPONENTS} ${ARGN})
endmacro()
