#################################
# cmake file for building Gear component
# @author Remi Ete, DESY
#################################

find_package( GEAR REQUIRED )

marlin_add_doxygen_sources( GLOB_RECURSE DIRECTORY plugins MATCHING_PATTERN *.cc )

# create library
aux_source_directory( plugins MarlinGear_sources )
add_library( GearPlugins MODULE ${MarlinGear_sources} )
add_library( Marlin::GearPlugins ALIAS GearPlugins )
install( TARGETS GearPlugins LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )
# For any stupid reason, Gear has two different include directories 
# exported: ${GEAR_DIR}/include and ${GEAR_DIR}/include/gear
# Need to go through a for loop... 
foreach( dir ${GEAR_INCLUDE_DIRS} )
  target_include_directories( 
    GearPlugins SYSTEM PUBLIC 
    $<BUILD_INTERFACE:${dir}>
  )
endforeach()
target_link_libraries( GearPlugins PUBLIC Marlin::Core ${GEAR_LIBRARIES} )
set_target_properties( 
  GearPlugins
  PROPERTIES 
    COMPILE_FLAGS ${MARLIN_COMPILE_OPTIONS}
    VERSION ${${PROJECT_NAME}_VERSION}
    CXX_STANDARD ${MARLIN_CXX_STANDARD}
)

# Add MarlinGear library to list of MARLIN_DLL libraries
marlin_add_plugin_library( TARGETS GearPlugins )
