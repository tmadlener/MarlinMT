#include <marlin/Component.h>
#include <marlin/Exceptions.h>
#include <marlin/Application.h>
#include <marlin/Utils.h>

namespace marlin {
  
  Component::Component( const std::string &type ) :
    _type(type),
    _name(details::convert<void*>::to_string(this)) {
    _logger = Logging::createLogger( this->type() + "_" + this->name() ) ;
    _logger->setLevel( "MESSAGE" ) ;
  }
  
  //--------------------------------------------------------------------------
  
  const std::string &Component::type() const {
    return _type ;
  }
  
  //--------------------------------------------------------------------------
  
  const std::string &Component::name() const {
    return _name ;
  }
  
  //--------------------------------------------------------------------------
  
  void Component::setName( const std::string &n ) {
    _name = n ;
  }
  
  //--------------------------------------------------------------------------
  
  const std::string &Component::description() const {
    return _description ;
  }
  
  //--------------------------------------------------------------------------
  
  void Component::setDescription( const std::string &desc ) {
    _description = desc ;
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
  
  const std::string &Component::verbosity() const {
    return _logger->levelName() ;
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
    _logger = application().createLogger( _name ) ;
    if( _verbosity.isSet() ) {
      _logger->setLevel( _verbosity.get() ) ;
    }
    message() << "----------------------------------------------------------" << std::endl ;
    message() << "Component type: '" << type() << "', name: '" << name() << "'" << std::endl ;
    message() << description() << std::endl ;
    initialize() ;
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