# 
# CMake macro(s) to check if the detected ROOT 
# version fulfill our requirements 
# 
# @author: Remi Ete, DESY
# @date: Oct 2019
# 


function( MarlinCheckROOTRequirements )
  # check if find_package( ROOT ... ) was processed before
  if( NOT ROOT_FOUND OR NOT ROOT_INCLUDE_DIRS )
    message( FATAL_ERROR "ROOT not found. Couldn't check ROOT7 requirements. Please use find_package( ROOT ... ) before calling this function" )
  endif()
  if( NOT MARLIN_BOOK )
    return()
  endif()
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
  # Required ROOT options
  set( REQUIRED_OPTIONS root7 )
  foreach( opt ${REQUIRED_OPTIONS} )
    message( STATUS " => Checking for ROOT option ${opt}: ${ROOT_${opt}_FOUND}" )
    if( NOT ROOT_${opt}_FOUND )
      message( FATAL_ERROR "Marlin ROOT requirements: ROOT was not compiled with the option '${opt}'" )
    endif()
  endforeach()
  message( STATUS "Marlin ROOT requirements OK ..." )
endfunction()








