#include <marlin/Component.h>
#include <marlin/Exceptions.h>
#include <marlin/Application.h>

namespace marlin {
  
  Component::Component( const std::string &name ) :
    _componentName(name) {
    _logger = Logging::createLogger( _componentName ) ;
  }
  
  Logging::StreamType Component::debug() const {
    return log<loglevel::DEBUG>() ;
  }
  
  Logging::StreamType Component::message() const {
    return log<loglevel::MESSAGE>() ;
  }
  
  Logging::StreamType Component::warning() const {
    return log<loglevel::WARNING>() ;
  }
  
  Logging::StreamType Component::error() const {
    return log<loglevel::ERROR>() ;
  }
  
  const Application &Component::application() const {
    if( nullptr == _application ) {
      MARLIN_THROW( "Application not set" ) ;
    }
    return *_application ;
  }
  
  Application &Component::application() {
    if( nullptr == _application ) {
      MARLIN_THROW( "Application not set" ) ;
    }
    return *_application ;
  }
  
  void Component::setup( Application *app ) {
    _application = app ;
  }
  
}