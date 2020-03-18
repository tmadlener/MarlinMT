#include <marlin/DataSourcePlugin.h>

// -- marlin headers
#include <marlin/Application.h>
#include <marlin/EventStore.h>
#include <marlin/RunHeader.h>

namespace marlin {

  DataSourcePlugin::DataSourcePlugin( const std::string &dstype ) :
    Component(dstype) {
    /* nop */
  }

  //--------------------------------------------------------------------------

  const std::string &DataSourcePlugin::type() const {
    return componentName() ;
  }

  //--------------------------------------------------------------------------

  const std::string &DataSourcePlugin::description() const {
    return _description ;
  }

  //--------------------------------------------------------------------------

  void DataSourcePlugin::readAll() {
    while( readOne() ) ;
  }

  //--------------------------------------------------------------------------

  void DataSourcePlugin::onEventRead( EventFunction func ) {
    _onEventRead = func ;
  }

  //--------------------------------------------------------------------------

  void DataSourcePlugin::onRunHeaderRead( RunHeaderFunction func ) {
    _onRunHeaderRead = func ;
  }

  //--------------------------------------------------------------------------

  void DataSourcePlugin::processRunHeader( std::shared_ptr<RunHeader> rhdr ) {
    if( nullptr == _onRunHeaderRead ) {
      throw Exception( "DataSourcePlugin::processRunHeader: no callback function available" ) ;
    }
    _onRunHeaderRead( rhdr ) ;
  }

  //--------------------------------------------------------------------------

  void DataSourcePlugin::processEvent( std::shared_ptr<EventStore> event ) {
    if( nullptr == _onEventRead ) {
      throw Exception( "DataSourcePlugin::processEvent: no callback function available" ) ;
    }
    _onEventRead( event ) ;
  }
  
  //--------------------------------------------------------------------------
  
  void DataSourcePlugin::initComponent() {
    auto &config = application().configuration() ;
    if( config.hasSection("datasource") ) {
      setParameters( config.section("datasource") ) ;
    }
  }

}
