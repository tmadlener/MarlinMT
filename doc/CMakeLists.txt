########################################################
# cmake file for building Marlin documentation
# @author Remi Ete, Desy
########################################################

set( INSTALL_DOXYGEN_DIR ${CMAKE_INSTALL_DOCDIR}/${PROJECT_VERSION}/Doxygen )

if( MARLIN_DOXYGEN )
  find_package( Doxygen REQUIRED )

  # build documentation out-of-source
  marlin_get_doxygen_inputs( 
    VAR DOX_INPUT_FILES
  )
  # message( STATUS "Doxygen input sources: ${DOX_INPUT_FILES}" )
  set( DOC_SRC_DIR "${PROJECT_SOURCE_DIR}/doc" )
  set( DOC_BIN_DIR "${PROJECT_BINARY_DIR}/docbuild" )
  set( DOX_INPUT overview.txt ${DOX_INPUT_FILES} )
  # custom command to build documentation
  add_custom_command(
      OUTPUT  "${DOC_BIN_DIR}/html/index.html" #"${DOC_BIN_DIR}/latex/index.tex"
      COMMAND DOX_PROJECT_NAME=${PROJECT_NAME}
              DOX_PROJECT_NUMBER="${${PROJECT_NAME}_VERSION}"
              DOX_OUTPUT_DIRECTORY="${DOC_BIN_DIR}"
              DOX_INPUT="${DOX_INPUT}"
              "${DOXYGEN_EXECUTABLE}"
      WORKING_DIRECTORY "${DOC_SRC_DIR}"
      COMMENT "Building Doxygen documentation..."
      DEPENDS Doxyfile CMakeLists.txt ${DOX_INPUT_FILES}
  )
  add_custom_target( doxygen DEPENDS "${DOC_BIN_DIR}/html/index.html" ) # "${DOC_BIN_DIR}/latex/index.tex" )
  # build documentation before 'make install'
  # FIXME: This is ugly. There might be a better way to do it.
  # Look at other package for inspiration...
  install( CODE "EXECUTE_PROCESS( COMMAND ${CMAKE_BUILD_TOOL} doxygen)" )
  install( FILES marlin_overview.gif ReleaseNotes.md  DESTINATION ${INSTALL_DOXYGEN_DIR} )
  install( DIRECTORY ${DOC_BIN_DIR}/html DESTINATION ${INSTALL_DOXYGEN_DIR} )
endif()

#----------------------------------------------------------------------------

if( MARLIN_MKDOCS )
  find_package( MkDocs REQUIRED )

  # build documentation out-of-source
  set( MKDOCS_SITE_DIR "${PROJECT_BINARY_DIR}/MkDocs/site" )
  set( MKDOCS_CONFIG_FILE "${PROJECT_SOURCE_DIR}/mkdocs.yml" )
  file( GLOB_RECURSE MKDOCS_SOURCES ${PROJECT_SOURCE_DIR}/docs )

  # custom command to build documentation
  add_custom_command(
      OUTPUT  "${MKDOCS_SITE_DIR}/index.html"
      COMMAND ${MKDOCS_EXECUTABLE} build -c -f ${MKDOCS_CONFIG_FILE} -d ${MKDOCS_SITE_DIR}
      COMMENT "Building Mkdocs documentation..."
      WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
      DEPENDS ${MKDOCS_CONFIG_FILE} CMakeLists.txt ${MKDOCS_SOURCES}
  )
  add_custom_target( mkdocs DEPENDS "${MKDOCS_SITE_DIR}/index.html" )
  install( CODE "EXECUTE_PROCESS( COMMAND ${CMAKE_BUILD_TOOL} mkdocs)" )
  set( INSTALL_MKDOCS_DIR ${CMAKE_INSTALL_DOCDIR}/${PROJECT_VERSION}/MkDocs )
  install( DIRECTORY ${MKDOCS_SITE_DIR} DESTINATION ${INSTALL_MKDOCS_DIR} )
  if( MARLIN_DOXYGEN )
    install( CODE "
      if( NOT EXISTS ${MKDOCS_SITE_DIR}/doxygen )
        EXECUTE_PROCESS(
        COMMAND ${CMAKE_COMMAND} -E
        create_symlink
          ${CMAKE_INSTALL_PREFIX}/${INSTALL_DOXYGEN_DIR}/html
          ${MKDOCS_SITE_DIR}/doxygen)
      endif()"
    )
  endif()

endif()
