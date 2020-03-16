#include "marlin/GeometryManager.h"

// -- marlin headers
#include "marlin/PluginManager.h"
#include "marlin/Application.h"

namespace marlin {

  GeometryManager::GeometryManager() :
    Component( "GeometryManager" ) {
    setComponentName( "Geometry" ) ;
  }

  //--------------------------------------------------------------------------

  void GeometryManager::initComponent() {
    auto &config = application().configuration() ;
    if( config.hasSection("geometry") ) {
      const auto &section = config.section("geometry") ;
      setParameters( section ) ;
    }
    auto geomType = _geometryType.get() ;
    message() << "Loading geometry plugin: " << geomType << std::endl ;
    auto &mgr = PluginManager::instance() ;
    _plugin = mgr.create<GeometryPlugin>( geomType ) ;
    if ( nullptr == _plugin ) {
      MARLIN_THROW( "Couldn't find geometry plugin '" + geomType + "'" ) ;
    }
    if( config.hasSection("geometry") ) {
      const auto &section = config.section("geometry") ;
      _plugin->setParameters( section ) ;
    }
    _plugin->setup( &application() ) ;
    _plugin->print() ;
  }

  //--------------------------------------------------------------------------

  std::type_index GeometryManager::typeIndex() const {
    if ( nullptr == _plugin ) {
      MARLIN_THROW( "Geometry not initialized !" ) ;
    }
    return _plugin->typeIndex() ;
  }

  //--------------------------------------------------------------------------

  void GeometryManager::clear() {
    if ( nullptr != _plugin ) {
      _plugin->destroy() ;
      _plugin = nullptr ;
    }
  }

} // namespace marlin
