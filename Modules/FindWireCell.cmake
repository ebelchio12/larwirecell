#[============================================================[.rst:
FindWireCell
============
#]============================================================]

find_program(WIRE-CELL NAMES wire-cell HINTS ENV WIRECELL_FQ_DIR)
mark_as_advanced(WIRE-CELL)

set(_fwc_libs Ress Util Iface Apps Hio Pgraph Root Sig Tbb Aux Gen Img SigProc Sio)
set(_fwc_transitive_deps_Apps Iface Util)
set(_fwc_transitive_deps_Aux Iface Util)
set(_fwc_transitive_deps_Gen Aux Iface Util)
set(_fwc_transitive_deps_Hio Iface Util)
set(_fwc_transitive_deps_Iface Util)
set(_fwc_transitive_deps_Img Aux Iface Util)
set(_fwc_transitive_deps_Pgraph Iface Util)
set(_fwc_transitive_deps_Root Iface Util)
set(_fwc_transitive_deps_Sig Iface Util)
set(_fwc_transitive_deps_SigProc Aux Iface Util)
set(_fwc_transitive_deps_Sio Aux Iface Util)
set(_fwc_transitive_deps_Tbb Iface Util)
set(_fwc_fp_ROOT_args COMPONENTS Core Hist RIO Tree EXPORT)

if (WIRE-CELL)
  execute_process(COMMAND ${WIRE-CELL} -v
    OUTPUT_VARIABLE ${CMAKE_FIND_PACKAGE_NAME}_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
    )
  if (NOT ${CMAKE_FIND_PACKAGE_NAME}_VERSION)
    string(REGEX REPLACE "^v?([0-9._-]+).*$" "\\1" ${CMAKE_FIND_PACKAGE_NAME}_VERSION "$ENV{WIRECELL_VERSION}")
    string(REGEX REPLACE "[_-]" "." ${CMAKE_FIND_PACKAGE_NAME}_VERSION "${${CMAKE_FIND_PACKAGE_NAME}_VERSION}")
  endif()
  unset(${CMAKE_FIND_PACKAGE_NAME}_LIBRARIES)
  find_path(${CMAKE_FIND_PACKAGE_NAME}_INCLUDE_DIR ${CMAKE_FIND_PACKAGE_NAME}Apps/Main.h PATH_SUFFIXES include)
  mark_as_advanced(${CMAKE_FIND_PACKAGE_NAME}_INCLUDE_DIR)
  if (${CMAKE_FIND_PACKAGE_NAME}_INCLUDE_DIR)
    foreach (_fwc_lib IN LISTS _fwc_libs)
      find_library(${CMAKE_FIND_PACKAGE_NAME}_${_fwc_lib}_LIBRARY NAMES ${CMAKE_FIND_PACKAGE_NAME}${_fwc_lib})
      mark_as_advanced(${CMAKE_FIND_PACKAGE_NAME}_${_fwc_lib}_LIBRARY)
      if (${CMAKE_FIND_PACKAGE_NAME}_${_fwc_lib}_LIBRARY)
        list(APPEND ${CMAKE_FIND_PACKAGE_NAME}_LIBRARIES "${${CMAKE_FIND_PACKAGE_NAME}_${_fwc_lib}_LIBRARY}")
      endif()
    endforeach()
  endif()
endif()

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(${CMAKE_FIND_PACKAGE_NAME}
  REQUIRED_VARS ${CMAKE_FIND_PACKAGE_NAME}_INCLUDE_DIR ${CMAKE_FIND_PACKAGE_NAME}_LIBRARIES
  VERSION_VAR ${CMAKE_FIND_PACKAGE_NAME}_VERSION)

if (${CMAKE_FIND_PACKAGE_NAME}_FOUND)
  set(_cet_find${CMAKE_FIND_PACKAGE_NAME}_required ${${CMAKE_FIND_PACKAGE_NAME}_FIND_REQUIRED})
  set(${CMAKE_FIND_PACKAGE_NAME}_FIND_REQUIRED)
  set(_cet_find${CMAKE_FIND_PACKAGE_NAME}_quietly ${${CMAKE_FIND_PACKAGE_NAME}_FIND_QUIETLY})
  set(${CMAKE_FIND_PACKAGE_NAME}_FIND_QUIETLY TRUE)
  foreach (_cet_find${CMAKE_FIND_PACKAGE_NAME}_dep Eigen3 jsoncpp jsonnet spdlog ROOT)
    get_property(_cet_find${CMAKE_FIND_PACKAGE_NAME}_${_cet_find${CMAKE_FIND_PACKAGE_NAME}_dep}_alreadyTransitive GLOBAL PROPERTY
      _CMAKE_${_cet_find${CMAKE_FIND_PACKAGE_NAME}_dep}_TRANSITIVE_DEPENDENCY)
    find_package(${_cet_find${CMAKE_FIND_PACKAGE_NAME}_dep} ${_fwc_fp_${dep}_args} QUIET)
    if (NOT DEFINED cet_${_cet_find${CMAKE_FIND_PACKAGE_NAME}_dep}_alreadyTransitive OR cet_${_cet_find${CMAKE_FIND_PACKAGE_NAME}_dep}_alreadyTransitive)
      set_property(GLOBAL PROPERTY _CMAKE_${_cet_find${CMAKE_FIND_PACKAGE_NAME}_dep}_TRANSITIVE_DEPENDENCY TRUE)
    endif()
    unset(_cet_find${CMAKE_FIND_PACKAGE_NAME}_${_cet_find${CMAKE_FIND_PACKAGE_NAME}_dep}_alreadyTransitive)
    if (NOT ${_cet_find${CMAKE_FIND_PACKAGE_NAME}_dep}_FOUND)
      set(${CMAKE_FIND_PACKAGE_NAME}_NOT_FOUND_MESSAGE "${CMAKE_FIND_PACKAGE_NAME} could not be found because dependency ${_cet_find${CMAKE_FIND_PACKAGE_NAME}_dep} could not be found.")
      set(${CMAKE_FIND_PACKAGE_NAME}_FOUND False)
      break()
    endif()
  endforeach()
endif()

set(${CMAKE_FIND_PACKAGE_NAME}_FIND_REQUIRED ${_cet_find${CMAKE_FIND_PACKAGE_NAME}_required})
set(${CMAKE_FIND_PACKAGE_NAME}_FIND_QUIETLY ${_cet_find${CMAKE_FIND_PACKAGE_NAME}_required})
unset(_cet_find${CMAKE_FIND_PACKAGE_NAME}_required)
unset(_cet_find${CMAKE_FIND_PACKAGE_NAME}_quietly)

if (${CMAKE_FIND_PACKAGE_NAME}_FOUND)
  foreach (_fwc_lib ${_fwc_libs})
    if (${CMAKE_FIND_PACKAGE_NAME}_${_fwc_lib}_LIBRARY AND NOT TARGET ${CMAKE_FIND_PACKAGE_NAME}::${_fwc_lib})
      add_library(${CMAKE_FIND_PACKAGE_NAME}::${_fwc_lib} UNKNOWN IMPORTED)
      set_target_properties(${CMAKE_FIND_PACKAGE_NAME}::${_fwc_lib} PROPERTIES
        IMPORTED_LOCATION "${${CMAKE_FIND_PACKAGE_NAME}_${_fwc_lib}_LIBRARY}"
        INTERFACE_COMPILE_DEFINITIONS "${EIGEN3_DEFINITIONS}"
        INTERFACE_INCLUDE_DIRECTORIES "${WireCell_INCLUDE_DIR};${EIGEN3_INCLUDE_DIRS}")
      if (_fwc_lib STREQUAL "Util")
        set_property(TARGET ${CMAKE_FIND_PACKAGE_NAME}::${_fwc_lib}
          APPEND PROPERTY INTERFACE_LINK_LIBRARIES "spdlog::spdlog")
      endif()
      if (NOT _fwc_lib STREQUAL "Ress")
        set_property(TARGET ${CMAKE_FIND_PACKAGE_NAME}::${_fwc_lib}
          APPEND PROPERTY INTERFACE_LINK_LIBRARIES "jsoncpp_lib;jsonnet_lib")
      endif()
      if (_fwc_transitive_deps_${_fwc_lib})
        foreach (_fwc_tdep ${_fwc_transitive_deps_${_fwc_lib}})
          set_property(TARGET ${CMAKE_FIND_PACKAGE_NAME}::${_fwc_lib}
            APPEND PROPERTY INTERFACE_LINK_LIBRARIES "WireCell::${_fwc_tdep}")
        endforeach()
        unset(_fwc_transitive_deps_${_fwc_lib})
      endif()
    endif()
  endforeach()
  if (NOT TARGET ${CMAKE_FIND_PACKAGE_NAME}::IComponent)
    add_library(${CMAKE_FIND_PACKAGE_NAME}::IComponent INTERFACE IMPORTED
      INTERFACE_INCLUDE_DIRECTORIES "${Wirecell_INCLUDE_DIR};${EIGEN3_INCLUDE_DIRS}")
  endif()
endif()

unset(_fwc_lib)
unset(_fwc_libs)
unset(_fwc_tdep)
