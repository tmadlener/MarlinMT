#pragma once

// -- marlin headers
#include <marlin/Exceptions.h>
#include <marlin/Logging.h>
#include <marlin/GeometryManager.h>
#include <marlin/LoggerManager.h>
#include <marlin/RandomSeedManager.h>
#include <marlin/BookStoreManager.h>
#include <marlin/Configuration.h>
#include <marlin/CmdLineParser.h>

namespace marlin {

  class IScheduler ;
  class DataSourcePlugin ;
  class RunHeader ;
  class EventStore ;

  /**
   *  @brief  Application class
   *  Base application interface for running a Marlin application.
   *  See daughter classes for details.
   */
  class Application {
  public:
    using Logger = Logging::Logger ;
    using CmdLineParseResult = CmdLineParser::ParseResult ;
    // using Scheduler = std::shared_ptr<IScheduler> ;
    // using EventList = std::vector<std::shared_ptr<EventStore>> ;
    using DataSource = std::shared_ptr<DataSourcePlugin> ;
    using ConditionsMap = std::map<std::string, std::string> ;

  public:
    Application() = default ;
    ~Application() = default ;

  public:
    /**
     *  @brief  Initialize the application
     *
     *  @param  argc argc from main function
     *  @param  argv argv from main function
     */
    void init( int argc, char **argv ) ;

    /**
     *  @brief  Run the application
     */
    void run() ;
    
    const CmdLineParseResult &cmdLineParseResult() const ;
    
    /**
     *  @brief  Get the program name
     */
    const std::string &programName() const ;

    /**
     *  @brief  Whether the application has been initialized
     */
    bool isInitialized() const ;

    /**
     *  @brief  Get the application logger instance
     */
    Logger logger() const ;

    /**
     *  @brief  Create a new logger instance.
     *  If the logger manager is initialized, the logger
     *  will share the same sinks as the main logger,
     *  else a fresh new logger is created.
     *
     *  @param  name the logger name
     */
    Logger createLogger( const std::string &name ) const ;

    /**
     *  @brief  Get the geometry manager
     */
    const GeometryManager &geometryManager() const ;

    /**
     *  @brief  Get the random seed manager
     */
    const RandomSeedManager &randomSeedManager() const ;

    /**
     *  @brief  Get the random seed manager
     */
    RandomSeedManager &randomSeedManager() ;

    /**
     *  @brief Get book store manager
     */
    BookStoreManager &bookStoreManager() ;

    /**
     *  @brief Get book store manager. For const object.
     *  @return const ref to book store manager.
     */
    const BookStoreManager &bookStoreManager() const ;
    
    /**
     *  @brief  Get the main application configuration object
     */
    const Configuration &configuration() const ;

    /**
     *  @brief  Set the scheduler instance to use in this application.
     *  Must be called before init(argc, argv)
     *
     *  @param  scheduler the sceduler instance to use
     */
    // void setScheduler( Scheduler scheduler ) ;

    /**
     *  @brief get number of concurrency with which the application runs. 
     */
    // std::size_t getConcurrency() const ; 

  protected:
    /**
     *  @brief  Get the parser instance
     */
    // std::shared_ptr<IParser> parser() const ;

  private:
    /**
     *  @brief  Print the Marlin banner
     *
     *  @param  out the out stream for printout
     */
    void printBanner( std::ostream &out = std::cout ) const ;
    
    /**
     *  @brief  Parse the command line arguments
     */
    // void parseCommandLine() ;
    // 
    // void storeCommandLine( int argc, const char **argv ) ;

    /**
     *  @brief  Create the parser instance based on the steering file extension
     */
    // std::shared_ptr<IParser> createParser() const ;

    /**
     *  @brief  Callback function to process an event received from the data source
     *
     *  @param  event the event to process
     */
    // void onEventRead( std::shared_ptr<EventStore> event ) ;
    // 
    // /**
    //  *  @brief  Callback function to process a run header received from the data source
    //  *
    //  *  @param  rhdr the run header to process
    //  */
    // void onRunHeaderRead( std::shared_ptr<RunHeader> rhdr ) ;
    // 
    // /**
    //  *  @brief  Processed finished events from the output queue
    //  *
    //  *  @param  events the list of finished events
    //  */
    // void processFinishedEvents( const EventList &events ) const ;

    /**
     *  @brief  determines concurrency number based on steering file and hardware.
     */
    // std::size_t readConcurrency() ; 

  protected:
    /// The command line parse result
    CmdLineParser::ParseResult _parseResult {} ;
    ///< The main application configuration
    Configuration              _configuration {} ;
    
    
  private:
    /// The geometry manager
    GeometryManager            _geometryMgr {} ;
    /// The random seed manager
    RandomSeedManager          _randomSeedMgr {} ;
    /// The logger manager
    LoggerManager              _loggerMgr {} ;
    ///< Managed data object shared between threads.
    BookStoreManager           _bookStoreManager {} ;

  private:
    /// Whether the application has been initialized
    bool                       _initialized {false} ;
    // ///< The event processing scheduler instance
    // Scheduler                  _scheduler {nullptr} ;
    // ///< The data source plugin
    // DataSource                 _dataSource {nullptr} ;
    // ///< Initial processor runtime conditions from steering file
    // ConditionsMap              _conditions {} ;

    // ///< Whether the currently pushed event is the first one
    // bool                       _isFirstEvent {true} ;
  };

} // end namespace marlin
