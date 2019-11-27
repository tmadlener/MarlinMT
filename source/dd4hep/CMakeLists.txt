#################################
# cmake file for building DD4hep component
# @author Remi Ete, DESY
#################################

# From v01-11, DD4hep uses imported targets
find_package( DD4hep 01.11 COMPONENTS DDRec DDParsers REQUIRED )

marlin_add_doxygen_sources( GLOB_RECURSE DIRECTORY plugins MATCHING_PATTERN *.cc )

# create library
aux_source_directory( plugins MarlinDD4hep_sources )
add_library( DD4hepPlugins MODULE ${MarlinDD4hep_sources} )
add_library( Marlin::DD4hepPlugins ALIAS DD4hepPlugins )
install( TARGETS DD4hepPlugins LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )
target_include_directories( 
  DD4hepPlugins SYSTEM PUBLIC
  $<BUILD_INTERFACE:$<TARGET_PROPERTY:ROOT::Core,INCLUDE_DIRECTORIES>>
  $<BUILD_INTERFACE:$<TARGET_PROPERTY:DD4hep::DDCore,INCLUDE_DIRECTORIES>> 
  $<BUILD_INTERFACE:$<TARGET_PROPERTY:DD4hep::DDRec,INCLUDE_DIRECTORIES>>
  $<BUILD_INTERFACE:$<TARGET_PROPERTY:DD4hep::DDParsers,INCLUDE_DIRECTORIES>>
)
target_link_libraries( 
  DD4hepPlugins PUBLIC
  Marlin::Core
  DD4hep::DDCore DD4hep::DDRec DD4hep::DDParsers 
)
set_target_properties( 
  DD4hepPlugins
  PROPERTIES 
    COMPILE_FLAGS ${MARLIN_COMPILE_OPTIONS}
    VERSION ${${PROJECT_NAME}_VERSION}
    CXX_STANDARD ${MARLIN_CXX_STANDARD}
)

# Add MarlinDD4hep library to list of MARLIN_DLL libraries
marlin_add_plugin_library( TARGETS DD4hepPlugins )
