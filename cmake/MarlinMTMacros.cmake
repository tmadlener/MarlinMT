

#
# function: marlinmt_supported_compiler_flags
# Check the given compiler flags and set
# the output variable with the supported flags
#
# Usage:
#    marlinmt_supported_compiler_flags( <output variable> <flag list> )
#
function( MARLINMT_SUPPORTED_COMPILER_FLAGS outvar )
  include( CheckCXXCompilerFlag )
  if( "${ARGN}" STREQUAL "" )
    message( FATAL_ERROR "marlinmt_supported_compiler_flags: no compiler flag given" )
  endif()
  set( SUPPORTED_FLAGS )
  foreach( FLAG ${ARGN} )
    ## meed to replace the minus or plus signs from the variables, because it is passed
    ## as a macro to g++ which causes a warning about no whitespace after macro
    ## definition
    string( REPLACE "-" "_" FLAG_WORD ${FLAG} )
    string( REPLACE "+" "P" FLAG_WORD ${FLAG_WORD} )
    check_cxx_compiler_flag( "${FLAG}" CXX_FLAG_WORKS_${FLAG_WORD} )
    if( ${CXX_FLAG_WORKS_${FLAG_WORD}} )
      ### We prepend the flag, so they are overwritten by whatever the user sets in his own configuration
      list( APPEND SUPPORTED_FLAGS ${FLAG} )
    endif()
  endforeach()
  if( SUPPORTED_FLAGS )
    set( ${outvar} ${SUPPORTED_FLAGS} PARENT_SCOPE )
  endif()
endfunction()

#
# function: marlinmt_cmake_summary
# helper function to display cmake variables and force write to cache
# otherwise outdated values may appear in ccmake gui
#
# Usage:
#    marlinmt_cmake_summary()
#
function( MARLINMT_CMAKE_SUMMARY )
  message( STATUS )
  message( STATUS "-------------------------------------------------------------------------------" )
  message( STATUS "Change values with: cmake -D<Variable>=<Value>" )
  if( DEFINED CMAKE_INSTALL_PREFIX )
    message( STATUS "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}" )
  endif()
  if( DEFINED CMAKE_BUILD_TYPE )
    message( STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}" )
  ENDIF()
  if( DEFINED BUILD_TESTING )
    message( STATUS "BUILD_TESTING = ${BUILD_TESTING}" )
  endif()
  message( STATUS "CMAKE_CXX_STANDARD = ${CMAKE_CXX_STANDARD}" )
  if( DEFINED CMAKE_PREFIX_PATH )
    list( REMOVE_DUPLICATES CMAKE_PREFIX_PATH )
    message( STATUS "CMAKE_PREFIX_PATH =" )
    foreach( _path ${CMAKE_PREFIX_PATH} )
      message( STATUS "   ${_path};" )
    endforeach()
  endif()
  if( DEFINED CMAKE_MODULE_PATH )
    list( REMOVE_DUPLICATES CMAKE_MODULE_PATH )
    message( STATUS "CMAKE_MODULE_PATH =" )
    foreach( _path ${CMAKE_MODULE_PATH} )
      message( STATUS "   ${_path};" )
    endforeach()
    set( CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" CACHE PATH "CMAKE_MODULE_PATH" FORCE )
  endif()
  if( MARLINMT_SUPPORTED_CXX_FLAGS )
    message( STATUS "MARLINMT_CXX_FLAGS =" )
    foreach( flag ${MARLINMT_SUPPORTED_CXX_FLAGS} )
      message( STATUS "   ${flag}" )
    endforeach()
  endif()
  get_property( marlinmt_dll_exports GLOBAL PROPERTY MARLINMT_DLL_EXPORTS )
  if( marlinmt_dll_exports )
    message( STATUS "MARLINMT_DLL:" )
    foreach( dll ${marlinmt_dll_exports} )
      message( STATUS "   ${dll};" )
    endforeach()
  endif()
  message( STATUS "OPTIONS:" )
  foreach( opt ${MARLINMT_CMAKE_OPTIONS} )
    message( STATUS "   ${opt} = ${${opt}}" )
  endforeach()
  message( STATUS "-------------------------------------------------------------------------------" )
  message( STATUS )
endfunction()

#
# function: marlinmt_set_max
# Set <variable> to 'max( <variable>, <test value> )'
#
# Usage:
#    marlinmt_set_max( <variable> <test value> )
#
function( MARLINMT_SET_MAX var value )
  if( NOT DEFINED ${var} )
    set( ${var} ${value} PARENT_SCOPE )
  else()
    if( "${value}" GREATER "${${var}}"  )
      set( ${var} ${value} PARENT_SCOPE )
    endif()
  endif()
endfunction()


#
# function: marlinmt_add_test
# Function to add a unit test from the src directory
#
# Usage:
#    marlinmt_add_test( <name>
#       [BUILD_EXEC]
#       [COMMAND <cmd>]
#       [DEPENDS <dependencies>]
#       [EXEC_ARGS <args>]
#       [REGEX_PASS <regex>]
#       [REGEX_FAIL <regex>]
#       [REQUIRES <target(s)>]
#    )
#
function ( MARLINMT_ADD_TEST test_name )
  cmake_parse_arguments(ARG "BUILD_EXEC" "" "COMMAND;DEPENDS;EXEC_ARGS;REGEX_PASS;REGEX_FAIL;REQUIRES;COMPONENTS;MARLINMT_DLL" ${ARGN} )
  set ( missing )
  set ( use_test 1 )

  if ( ${ARG_BUILD_EXEC} )
    list( APPEND ARG_COMPONENTS MarlinMT::Core )
    list( REMOVE_DUPLICATES ARG_COMPONENTS )
    foreach( component ${ARG_COMPONENTS} )
      if( NOT TARGET ${component} )
        list( APPEND MISSING_TEST_COMPONENTS ${component} )
      endif()
    endforeach()
    if( MISSING_TEST_COMPONENTS )
      message( STATUS "Skipping unit test ${test_name}. Missing components: ${MISSING_TEST_COMPONENTS}" )
      return()
    endif()
    add_executable( ${test_name} tests/${test_name}.cc )
    set_target_properties(
      ${test_name}
      PROPERTIES
        CXX_STANDARD ${MARLINMT_CXX_STANDARD}
        COMPILE_FLAGS ${MARLINMT_COMPILE_OPTIONS}
    )
    foreach( component ${ARG_COMPONENTS} )
      target_include_directories( ${test_name} SYSTEM BEFORE PUBLIC $<TARGET_PROPERTY:${component},INCLUDE_DIRECTORIES> )
      target_link_libraries( ${test_name} ${component} )
    endforeach()
    target_include_directories( ${test_name} BEFORE PUBLIC include ) 
    set_target_properties( ${test_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests" )
  endif()

  set ( cmd ${ARG_COMMAND} )
  if ( "${cmd}" STREQUAL "" )
    if( ${ARG_BUILD_EXEC} )
      set ( cmd ${CMAKE_BINARY_DIR}/tests/${test_name} )
    else()
      message( SEND_ERROR "No command no executable provided" )
    endif()
  endif()

  set ( passed ${ARG_REGEX_PASS} )
  if ( "${passed}" STREQUAL "NONE" )
    unset ( passed )
  elseif ( "${passed}" STREQUAL "" )
    set ( passed "TEST_PASSED" )
  endif()

  set ( failed ${ARG_REGEX_FAIL} )
  if ( "${failed}" STREQUAL "NONE" )
    unset ( failed )
  endif()

  set( marlinmt_dll )
  foreach( dll ${ARG_MARLINMT_DLL} )
    get_target_property( target_type ${dll} TYPE )
    if( NOT "${target_type}" STREQUAL "MODULE_LIBRARY" )
      message( FATAL_ERROR "MARLINMT_ADD_TEST: Trying to use DLL libraries which are not module libraries" )
    endif()
    list( APPEND marlinmt_dll "$<TARGET_FILE:${dll}>" )
  endforeach()

  set ( args ${ARG_EXEC_ARGS} )
  add_test(NAME t_${test_name} COMMAND ${cmd} ${args} )
  if ( NOT "${passed}" STREQUAL "" )
    set_tests_properties( t_${test_name} PROPERTIES PASS_REGULAR_EXPRESSION "${passed}" )
  endif()
  if ( NOT "${failed}" STREQUAL "" )
    set_tests_properties( t_${test_name} PROPERTIES FAIL_REGULAR_EXPRESSION "${failed}" )
  endif()
  if( marlinmt_dll )
    list( JOIN marlinmt_dll ":" marlinmt_dll )
    set_tests_properties( t_${test_name} PROPERTIES ENVIRONMENT "MARLINMT_DLL=${marlinmt_dll}" )
  endif()
  # Set test dependencies if present
  foreach ( _dep ${ARG_DEPENDS} )
    set_tests_properties( t_${test_name} PROPERTIES DEPENDS t_${_dep} )
  endforeach()
endfunction()


#
# function: marlinmt_add_processor_test
# Add a unit test to run a processor and for a fail/pass regex
#
# Usage:
#     marlinmt_add_processor_test( <name>
#        STEERING_FILE <file>
#        [REGEX_PASS <regex>]
#        [REGEX_FAIL <regex>]
#        [INPUT_FILES <files>]
#        [MARLINMT_ARGS <args>]
#     )
#
function( MARLINMT_ADD_PROCESSOR_TEST test_name )
	cmake_parse_arguments(ARG "" "STEERING_FILE;REGEX_PASS;REGEX_FAIL" "INPUT_FILES;MARLINMT_ARGS;MARLINMT_DLL;COMPONENTS" ${ARGN} )
  if( NOT test_name )
    message( FATAL_ERROR "[UNIT_TESTS] Configuring processor test without name" )
  endif()
  if( NOT ARG_STEERING_FILE )
    message( FATAL_ERROR "[UNIT_TESTS ${test_name}] Configuring processor test without steering file" )
  endif()
  list( APPEND ARG_INPUT_FILES ${ARG_STEERING_FILE} )
  foreach( ifile ${ARG_INPUT_FILES} )
    if( NOT EXISTS ${ifile} )
      message( FATAL_ERROR "[UNIT_TEST ${test_name}] Input files doesn't exists ${ifile}" )
    endif()
  endforeach()
  list( APPEND ARG_COMPONENTS MarlinMT::Core ${ARG_MARLINMT_DLL} )
  list( REMOVE_DUPLICATES ARG_COMPONENTS )
  foreach( component ${ARG_COMPONENTS} )
    if( NOT TARGET ${component} )
      list( APPEND MISSING_TEST_COMPONENTS ${component} )
    endif()
  endforeach()
  if( MISSING_TEST_COMPONENTS )
    message( STATUS "Skipping unit test ${test_name}. Missing components: ${MISSING_TEST_COMPONENTS}" )
    return()
  endif()
  set( MARLINMT_INPUT_FILES ${ARG_INPUT_FILES} )
  get_filename_component( MARLINMT_STEERING_FILE ${ARG_STEERING_FILE} NAME )
  # set( MARLINMT_STEERING_FILE ${ARG_STEERING_FILE} )
  foreach( marg ${ARG_MARLINMT_ARGS} )
    set( MARLINMT_ARGS "${MARLINMT_ARGS} ${marg}" )
  endforeach()
  set( MARLINMT_DLL "$<TARGET_FILE:MarlinMT::UnitTest>" )
  foreach( dll ${ARG_MARLINMT_DLL} )
    get_target_property( target_type ${dll} TYPE )
    if( NOT "${target_type}" STREQUAL "MODULE_LIBRARY" )
      message( FATAL_ERROR "MARLINMT_ADD_PROCESSOR_TEST: Trying to using DLL libraries with are not module libraries" )
    endif()
    list( APPEND MARLINMT_DLL "$<TARGET_FILE:${dll}>" )
  endforeach()
  list( JOIN MARLINMT_DLL ":" MARLINMT_DLL )
  # configure cmake test file.
  # Read the file content and use file( GENERATE )
  # because the MARLINMT_DLL contains generator expressions
	set (RUN_INPUT_FILE "${PROJECT_SOURCE_DIR}/cmake/runmarlinmt.cmake.in")
	file( READ ${RUN_INPUT_FILE} TEST_FILE_CONTENT )
	set (MARLINMT_BIN_TARGET bin_MarlinMT)
  string( CONFIGURE ${TEST_FILE_CONTENT} TEST_FILE_FINAL @ONLY )
  set( TEST_CMAKE_FILE ${CMAKE_CURRENT_BINARY_DIR}/test_processor_${test_name}.cmake )
  if( EXISTS ${TEST_CMAKE_FILE} )
    file( REMOVE ${TEST_CMAKE_FILE} )
  endif()
  message( STATUS "Generating test file: ${TEST_CMAKE_FILE} ..." )
  file(
    GENERATE
    OUTPUT ${TEST_CMAKE_FILE}
    CONTENT "${TEST_FILE_FINAL}"
  )
  # add the unit test
  add_test( t_processor_${test_name} "${CMAKE_COMMAND}" -P ${TEST_CMAKE_FILE} )
  if( NOT "${ARG_REGEX_PASS}" STREQUAL "" )
    set_tests_properties( t_processor_${test_name} PROPERTIES PASS_REGULAR_EXPRESSION "${ARG_REGEX_PASS}" )
  endif()
  if( NOT "${ARG_REGEX_FAIL}" STREQUAL "" )
    set_tests_properties( t_processor_${test_name} PROPERTIES FAIL_REGULAR_EXPRESSION "${ARG_REGEX_FAIL}" )
  endif()
endfunction()

#
# function: marlinmt_add_doxygen_sources
# Add a directory to the doxygen documentation process
#
# Usage:
#     marlinmt_add_doxygen_sources(
#        [GLOB_RECURSE]
#        DIRECTORY <dir>
#        [MATCHING_PATTERN <patterns>]
#     )
#
function( MARLINMT_ADD_DOXYGEN_SOURCES )
  cmake_parse_arguments(ARG "GLOB_RECURSE" "DIRECTORY" "MATCHING_PATTERN" ${ARGN} )
  if( NOT ARG_DIRECTORY )
    message( FATAL_ERROR "MARLINMT_ADD_DOXYGEN_SOURCES: No source directory provided" )
  endif()
  foreach( pattern ${ARG_MATCHING_PATTERN} )
    if( ARG_GLOB_RECURSE )
      file( GLOB_RECURSE DOXYGEN_SOURCE_FILES ${ARG_DIRECTORY}/${pattern} )
    else()
      file( GLOB DOXYGEN_SOURCE_FILES LIST_DIRECTORIES false ${ARG_DIRECTORY}/${pattern} )
    endif()
  endforeach()
  # message( "Doxygen: from directory ${ARG_DIRECTORY}, collected source files: ${DOXYGEN_SOURCE_FILES}" )
  get_property( local_files GLOBAL PROPERTY DOXYGEN_SOURCE_FILES )
  list( APPEND local_files ${DOXYGEN_SOURCE_FILES} )
  list( REMOVE_DUPLICATES local_files )
  set_property( GLOBAL PROPERTY DOXYGEN_SOURCE_FILES ${local_files} )
endfunction()


#
# function: marlinmt_get_doxygen_inputs
# Get the source files to use in the doxygen documentation process
#
# Usage:
#     marlinmt_get_doxygen_inputs(
#        VAR <var>
#     )
#
function( MARLINMT_GET_DOXYGEN_INPUTS )
  cmake_parse_arguments(ARG "" "VAR" "" ${ARGN} )
  get_property( local_files GLOBAL PROPERTY DOXYGEN_SOURCE_FILES )
  set( ${ARG_VAR} ${local_files} PARENT_SCOPE )
endfunction()
  

#
# function: marlinmt_add_plugin_library
# Add the library target to the list of MARLINMT_DLL to export
#
# Usage:
#     marlinmt_add_plugin_library(
#        TARGETS <targets>
#     )
#
function( marlinmt_add_plugin_library )
  cmake_parse_arguments(ARG "" "" "TARGETS" ${ARGN} )
  if( NOT ARG_TARGETS )
    message( FATAL_ERROR "MARLINMT_ADD_PLUGIN_LIBRARY: no target specified" )
    return()
  endif()
  get_property( local_marlinmt_dll GLOBAL PROPERTY MARLINMT_DLL_EXPORTS )
  foreach( target ${ARG_TARGETS} )
    if( NOT TARGET ${target} )
      message( FATAL_ERROR "MARLINMT_ADD_PLUGIN_LIBRARY: specified target ${target} is not a target" )
      return()
    endif()
    list( APPEND local_marlinmt_dll ${target} )
  endforeach()
  list( REMOVE_DUPLICATES local_marlinmt_dll )
  set_property( GLOBAL PROPERTY MARLINMT_DLL_EXPORTS ${local_marlinmt_dll} )
endfunction()


#
