// -- marlin headers
#include <marlin/Application.h>
#include <marlin/PluginManager.h>
#include <marlin/Utils.h>

using namespace marlin ;

int main( int argc, char **argv ) {
  
  // load plugins first
  auto &mgr = PluginManager::instance() ;
  mgr.logger()->setLevel<MESSAGE>() ;
  auto libraries = details::split_string<std::string>( 
    details::getenv<std::string>( "MARLIN_DLL", "" ), 
    ":" 
  );
  mgr.loadLibraries( libraries ) ;
  
  // configure and run application
  Application application ;
  auto logger = application.logger() ;
  try {    
    application.init( argc, argv ) ;
    application.run() ;
  }
  catch ( marlin::Exception &e ) {
    logger->log<ERROR>() << "MarlinMT main, caught Marlin exception " << e.what() << std::endl ;
    logger->log<ERROR>() << "Exiting with status 1" << std::endl ;
    return 1 ;
  }
  catch ( std::exception &e ) {
    logger->log<ERROR>() << "MarlinMT main, caught std::exception " << e.what() << std::endl ;
    logger->log<ERROR>() << "Exiting with status 1" << std::endl ;
    return 1 ;
  }
  catch ( ... ) {
    logger->log<ERROR>() << "MarlinMT main, caught unknown exception" << std::endl ;
    logger->log<ERROR>() << "Exiting with status 2" << std::endl ;
    return 2 ;
  }

  return 0 ;
}
