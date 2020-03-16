#################################
# cmake file for building MarlinMT
# @author Remi Ete, DESY
#################################

set( MARLIN_3RDPARTY ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty )

# MarlinBook and framework
add_subdirectory( book )
add_subdirectory( framework )

# ----- MarlinDD4hep 
if( MARLIN_DD4HEP )
  add_subdirectory( dd4hep )
endif()

# MarlinGear 
if( MARLIN_GEAR )
  add_subdirectory( gear )
endif()

# MarlinLCIO 
if( MARLIN_LCIO )
  add_subdirectory( lcio )
endif()

# TODO: Remove this shit!
# MarlinGUI 
set( MARLIN_GUI OFF )
if( MARLIN_GUI )
  add_subdirectory( gui )
endif()



