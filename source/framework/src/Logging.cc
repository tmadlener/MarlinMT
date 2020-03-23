#include "marlin/Logging.h"

namespace marlin {

  Logging::Logger Logging::createLogger( const std::string &name ) {
    return streamlog::logstream::createLogger<Logging::mutex_type>( name ) ;
  }
  
  //--------------------------------------------------------------------------
  
  Logging::DefaultLoggerType &Logging::globalLogger() {
    return streamlog::logstream::global() ;
  }

}
