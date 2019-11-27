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
target_link_libraries( 
  DD4hepPlugins PUBLIC
  Marlin::Core
  DD4hep::DDCore DD4hep::DDRec DD4hep::DDParsers 
)

# Add MarlinDD4hep library to list of MARLIN_DLL libraries
marlin_add_plugin_library( TARGETS DD4hepPlugins )
