#
# CMake macros and function used during the build process
# of Marlin. Stuff defined here is private. For public
# functions, see MarlinMacros.cmake
#
# @author: Remi Ete, DESY
# @date: Oct 2019
#


# Use GNU conventions for install paths
include( GNUInstallDirs )
include( MarlinMacros )

macro( MARLIN_OPTION opt )
  option( ${opt} ${ARGN} )
  list( APPEND MARLIN_CMAKE_OPTIONS ${opt} )
  list( REMOVE_DUPLICATES MARLIN_CMAKE_OPTIONS )
endmacro()

# CMake options
marlin_option( MARLIN_LCIO                  "Set to ON to build Marlin with LCIO support" ON )
marlin_option( MARLIN_BOOK                  "Set to ON to build Marlin with book store functionality (requires ROOT7 !)" OFF )
marlin_option( MARLIN_DD4HEP                "Set to ON to build Marlin with DD4hep" ON )
marlin_option( MARLIN_GEAR                  "Set to ON to build Marlin with Gear" OFF )
marlin_option( MARLIN_DOXYGEN               "Set to ON to build/install Doxygen documentation" OFF )
marlin_option( MARLIN_MKDOCS                "Set to ON to build/install mkdocs documentation" OFF )
marlin_option( MARLIN_WERROR                "Set to ON to compile Marlin with -Werror" OFF )

# List of compiler we want to compile Marlin with.
# The list is purified using MarlinSupportedCompilerFlags
list( APPEND MARLIN_CXX_FLAGS
  -Wall -Wextra -Wshadow -Weffc++ -pedantic -Wno-long-long -Wuninitialized -O3
  -fexceptions -fstack-clash-protection -Wsuggest-override -Wconversion -Wparentheses
)
if( MARLIN_WERROR )
  list( APPEND MARLIN_CXX_FLAGS -Werror )
endif()
if( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" )
 list( APPEND MARLIN_CXX_FLAGS -Wl,-no-undefined )
endif()
if( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.9 )
  list( APPEND MARLIN_CXX_FLAGS -fdiagnostics-color=auto )
endif()

marlin_supported_compiler_flags( MARLIN_SUPPORTED_CXX_FLAGS ${MARLIN_CXX_FLAGS} )
string( JOIN " " MARLIN_COMPILE_OPTIONS ${MARLIN_SUPPORTED_CXX_FLAGS} )
set( MARLIN_COMPILE_OPTIONS ${MARLIN_COMPILE_OPTIONS} CACHE STRING "The CXX flags used by Marlin" FORCE )

# RPATH treatment
if (APPLE)
  # use, i.e. don't skip the full RPATH for the build tree
  set( CMAKE_SKIP_BUILD_RPATH  FALSE )
  # when building, don't use the install RPATH already
  # (but later on when installing)
  set( CMAKE_BUILD_WITH_INSTALL_RPATH FALSE )
  set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_FULL_LIBDIR} )
  # add the automatically determined parts of the RPATH
  # which point to directories outside the build tree to the install RPATH
  set( CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE )
else()
  # add library install path to the rpath list
  set( CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_FULL_LIBDIR} )
  mark_as_advanced( CMAKE_INSTALL_RPATH )
  # append link pathes to rpath list
  set( CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE )
  mark_as_advanced( CMAKE_INSTALL_RPATH_USE_LINK_PATH )
endif()

#
# function: marlin_set_cxx_standard
# Check and set the CXX standard if not set
#
# Usage:
#    marlin_set_cxx_standard()
#
function( MARLIN_SET_CXX_STANDARD )
  set( MARLIN_UNSUPPORTED_CXX_STANDARDS 98 11 14 )
  if( NOT DEFINED CMAKE_CXX_STANDARD )
    set( MARLIN_CXX_STANDARD 17 )
  else()
    if( CMAKE_CXX_STANDARD IN_LIST MARLIN_SUPPORTED_COMPILER_FLAGS )
      message( FATAL_ERROR "CXX standard ${CMAKE_CXX_STANDARD} not supported. Must be >= 17" )
    endif()
    set( MARLIN_CXX_STANDARD ${CMAKE_CXX_STANDARD} )
  endif()
  set( CMAKE_CXX_STANDARD  ${MARLIN_CXX_STANDARD} CACHE STRING "The CXX standard" FORCE )
  set( MARLIN_CXX_STANDARD ${MARLIN_CXX_STANDARD} CACHE STRING "The CXX standard" FORCE )
  message( STATUS "Will use C++ standard ${MARLIN_CXX_STANDARD}" )
endfunction()


#
# function: marlin_check_root_requirements
# In case ROOT is used check the ROOT config.
# For example, we may require experimental components
# from ROOT for our BOOK interface...
#
# Usage:
#    marlin_check_root_requirements()
#
function( MARLIN_CHECK_ROOT_REQUIREMENTS )
  # check if find_package( ROOT ... ) was processed before
  if( NOT ROOT_FOUND OR NOT ROOT_INCLUDE_DIRS )
    message( FATAL_ERROR "ROOT not found. Couldn't check ROOT7 requirements. Please use find_package( ROOT ... ) before calling this function" )
  endif()
  if( NOT MARLIN_BOOK )
    return()
  endif()
  # Required ROOT options
  set( REQUIRED_OPTIONS root7 )
  foreach( opt ${REQUIRED_OPTIONS} )
    message( STATUS " => Checking for ROOT option ${opt}: ${ROOT_${opt}_FOUND}" )
    if( NOT ROOT_${opt}_FOUND )
      message( FATAL_ERROR "Marlin ROOT requirements: ROOT was not compiled with the option '${opt}'" )
    endif()
  endforeach()
  # Marlin book requires includes from ROOT 7
  list( APPEND REQUIRED_INCLUDES
    RHist.hxx
    RSpan.hxx
    RAxis.hxx
    RHistBufferedFill.hxx
    RHistConcurrentFill.hxx
  )
  foreach( include_dir ${ROOT_INCLUDE_DIRS} )
    foreach( include_file ${REQUIRED_INCLUDES} )
      string( REPLACE "." "_" include_file_var ${include_file} )
      find_file( file_found_${include_file_var} ${include_file} PATHS ${include_dir} PATH_SUFFIXES ROOT NO_DEFAULT_PATH )
      message( STATUS " => Checking for file ${include_file}: ${file_found_${include_file_var}}" )
      if( NOT file_found_${include_file_var} )
        message( FATAL_ERROR "Couldn't find ROOT header file: ${include_file}" )
      endif()
    endforeach()
  endforeach()
  message( STATUS "Marlin ROOT requirements OK ..." )
endfunction()


# 
# function: marlin_generate_thismarlin
# Generate and install thismarlin.sh
# 
# Usage:
#    marlin_generate_thismarlin()
#    
function( MARLIN_GENERATE_THISMARLIN )
  file( READ ${PROJECT_SOURCE_DIR}/cmake/thismarlin.sh.in FILE_CONTENT )
  get_property( marlin_dll_exports GLOBAL PROPERTY MARLIN_DLL_EXPORTS )
  set( MARLIN_DLLS )
  foreach( dll ${marlin_dll_exports} )
    list( APPEND MARLIN_DLLS "$MARLIN_DIR/@CMAKE_INSTALL_LIBDIR@/$<TARGET_FILE_NAME:${dll}>" )
  endforeach()
  list( JOIN MARLIN_DLLS ":" CMAKE_MARLIN_DLL )
  message( STATUS "Will export marlin dll for targets: ${marlin_dll_exports}" )
  string( CONFIGURE ${FILE_CONTENT} FILE_FINAL @ONLY )
  string( CONFIGURE ${FILE_FINAL} FILE_FINAL @ONLY )
  set( OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/thismarlin.sh )
  if( EXISTS ${OUTPUT_FILE} )
    file( REMOVE ${OUTPUT_FILE} )
  endif()
  message( STATUS "Generating file: ${OUTPUT_FILE} ..." )
  file(
    GENERATE
    OUTPUT ${OUTPUT_FILE}
    CONTENT "${FILE_FINAL}"
  )
  install( FILES ${OUTPUT_FILE} TYPE BIN )
endfunction()
