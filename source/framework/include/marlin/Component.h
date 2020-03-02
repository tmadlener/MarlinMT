#pragma once

#include <marlin/Logging.h>
#include <marlin/Parameter.h>

namespace marlin {
  
  class Application ;
  
  /**
   *  @brief  Component class.
   *  Base class for all application components. Groups a few feature common
   *  too all application components:
   *   - configuration via parameters
   *   - logger access
   *   - access to application
   */
  class Component : public Configurable {
  public:    
    friend class Application ; 
    using LoggerPtr = Logging::Logger ;
    
  public:
    /// No default constructor
    Component() = delete ;
    /// No copy or assignement
    Component(const Component &) = delete ;
    Component &operator=(const Component &) = delete ;

    /// Default destructor
    virtual ~Component() = default ;
    
    /// Constructor with component name
    Component( const std::string &name ) ;
    
    /**
     *  @brief  Get the application in which the component is registered
     */
    const Application &application() const ;
    
    /**
     *  @brief  Get the application in which the component is registered
     */
    Application &application() ;
    
    template <class T>
    inline Logging::StreamType log() const {
      return _logger->log<T>() ;
    }
    
    /// Shortcut for log<DEBUG>()
    Logging::StreamType debug() const ;
    
    /// Shortcut for log<MESSAGE>()
    Logging::StreamType message() const ;
    
    /// Shortcut for log<WARNING>()
    Logging::StreamType warning() const ;
    
    /// Shortcut for log<ERROR>()
    Logging::StreamType error() const ;
    
    inline const std::string &componentName() const {
      return _componentName ;
    }
    
  private:
    /// Setup the component. Called by the application before initialization
    void setup( Application *app ) ;
    
  private:
    std::string              _componentName {} ;
    Application             *_application {nullptr} ;
    LoggerPtr                _logger {nullptr} ;
  };
  
}