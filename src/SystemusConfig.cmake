
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
