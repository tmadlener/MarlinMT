
// -- marlin headers
#include <marlin/IScheduler.h>
#include <marlin/Application.h>

namespace marlin {
  
  IScheduler::IScheduler() : 
    Component("Scheduler") {
    /* nop */
  }
  
  //--------------------------------------------------------------------------

  void IScheduler::initComponent() {
    auto &config = application().configuration() ;
    if( config.hasSection("scheduler") ) {
      setParameters( config.section("scheduler") ) ;
    }
  }
  
}


