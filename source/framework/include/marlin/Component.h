#pragma once

// -- std headers
#include <typeindex>

// -- marlin headers
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
    
    /// Constructor with component type
    Component( const std::string &type ) ;
    
    /**
     *  @brief  Get the component name
     */
    const std::string &componentType() const ;
    
    /**
     *  @brief  Get the component name
     */
    const std::string &componentName() const ;
    
    /**
     *  @brief  Set the component name
     *  
     *  @param  name the component name
     */
    void setComponentName( const std::string &name ) ;
    
    /**
     *  @brief  Get the component description
     */
    const std::string &componentDescription() const ;
    
    /**
     *  @brief  Initialize the component
     */
    virtual void initComponent() { /* nop */ }
    
    /**
     *  @brief  Get the application in which the component is registered
     */
    const Application &application() const ;
    
    /**
     *  @brief  Get the application in which the component is registered
     */
    Application &application() ;
    
    /**
     *  @brief  Log a message with specific log level.
     *  Returns a stream object in which you can log.
     *  Usage:
     *  @code{.cpp}
     *  log<MESSAGE>() << "This is a message" << std::endl ;
     *  @endcode
     */
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
    
    /**
     *  @brief  Set the verbosity level
     * 
     *  @param  level the verbosity level to set
     */
    void setVerbosity( const std::string &level ) ;
    
  protected:
    /**
     *  @brief  Get the component description
     */
    void setComponentDescription( const std::string &description ) ;
    
  private:
    /// Setup the component. Called by the application before initialization
    void setup( Application *app ) ;
    
  protected:
    /// The component type
    std::string              _componentType {} ;
    /// The component name
    std::string              _componentName {} ;
    /// The component description
    std::string              _componentDescription {} ;
    /// The application in which the component has been registered
    Application             *_application {nullptr} ;
    /// The logger instance
    LoggerPtr                _logger {nullptr} ;
    /// The verbosity level of the logger (parameter)
    StringParameter          _verbosity { *this, "Verbosity", "The component verbosity level", "MESSAGE" } ;
  };
  
}