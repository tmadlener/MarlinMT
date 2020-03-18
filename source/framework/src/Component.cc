#include <marlin/Component.h>
#include <marlin/Exceptions.h>
#include <marlin/Application.h>
#include <marlin/Utils.h>

namespace marlin {
  
  Component::Component( const std::string &type ) :
    _componentType(type),
    _componentName(details::convert<void*>::to_string(this)) {
    _logger = Logging::createLogger( _componentType + "_" + _componentName ) ;
    _logger->setLevel( "MESSAGE" ) ;
  }
  
  //--------------------------------------------------------------------------
  
  const std::string &Component::componentType() const {
    return _componentType ;
  }
  
  //--------------------------------------------------------------------------
  
  const std::string &Component::componentName() const {
    return _componentName ;
  }
  
  //--------------------------------------------------------------------------
  
  void Component::setComponentName( const std::string &name ) {
    _componentName = name ;
  }
  
  //--------------------------------------------------------------------------
  
  const std::string &Component::componentDescription() const {
    return _componentDescription ;
  }
  
  //--------------------------------------------------------------------------
  
  void Component::setComponentDescription( const std::string &description ) {
    _componentDescription = description ;
  }
  
  //--------------------------------------------------------------------------
  
  Logging::StreamType Component::debug() const {
    return log<loglevel::DEBUG>() ;
  }
  
  //--------------------------------------------------------------------------
  
  Logging::StreamType Component::message() const {
    return log<loglevel::MESSAGE>() ;
  }
  
  //--------------------------------------------------------------------------
  
  Logging::StreamType Component::warning() const {
    return log<loglevel::WARNING>() ;
  }
  
  //--------------------------------------------------------------------------
  
  Logging::StreamType Component::error() const {
    return log<loglevel::ERROR>() ;
  }
  
  //--------------------------------------------------------------------------
  
  void Component::setVerbosity( const std::string &level ) {
    _logger->setLevel( level ) ;
  }
  
  //--------------------------------------------------------------------------
  
  const Application &Component::application() const {
    if( nullptr == _application ) {
      MARLIN_THROW( "Application not set" ) ;
    }
    return *_application ;
  }
  
  //--------------------------------------------------------------------------
  
  Application &Component::application() {
    if( nullptr == _application ) {
      MARLIN_THROW( "Application not set" ) ;
    }
    return *_application ;
  }
  
  //--------------------------------------------------------------------------
  
  void Component::setup( Application *app ) {
    _application = app ;
    _logger = application().createLogger( _componentName ) ;
    if( _verbosity.isSet() ) {
      _logger->setLevel( _verbosity.get() ) ;
    }
    message() << "----------------------------------------------------------" << std::endl ;
    message() << "Component type: '" << componentType() << "', name: '" << componentName() << "'" << std::endl ;
    message() << componentDescription() << std::endl ;
    initComponent() ;
    message() << "----------------------------------------------------------" << std::endl ;
  }
  
  //--------------------------------------------------------------------------
  
  bool Component::isInitialized() const {
    return ( nullptr == _application ) ;
  }
  
  //--------------------------------------------------------------------------
  
  void Component::printParameters() const {
    printParameters<MESSAGE>() ;
  }
  
}