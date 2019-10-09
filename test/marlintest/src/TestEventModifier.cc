#include "TestEventModifier.h"

// ----- include for verbosity dependend logging ---------
#include "marlin/Logging.h"
#include "marlin/PluginManager.h"

#include "IMPL/LCEventImpl.h"
#include "IMPL/LCRunHeaderImpl.h"

using namespace lcio ;
using namespace marlin ;


// TestEventModifier aTestEventModifier ;


TestEventModifier::TestEventModifier() : Processor("TestEventModifier") {
  // modify processor description
  _description = "TestEventModifier make changes to the run header and the event data for testing" ;
}

void TestEventModifier::processRunHeader( LCRunHeader* run) {

  LCRunHeaderImpl* rHdr = dynamic_cast< LCRunHeaderImpl* >( run ) ;

  try {
    if( rHdr != 0 ) {
      rHdr->setRunNumber( _nRun + 42 ) ;
      streamlog_out(MESSAGE4)  << " processRunHeader modified run number to be : "
            << _nRun << " + 42 = " <<  run->getRunNumber() << std::endl ;
      _nRun++ ;
    }
  }
  catch( EVENT::Exception& ) {
    streamlog_out(ERROR)  << " processRunHeader LCRunHeader::runNumber not modified ! "  << std::endl;
  }
}

void TestEventModifier::processEvent( LCEvent * e ) {
  LCEventImpl* evt = dynamic_cast<LCEventImpl*>( e ) ;
  try {

    if( evt != 0 ){
      evt->setEventNumber(  _nEvt + 42 ) ;
      streamlog_out(MESSAGE4)  << " processEvent modified event number to be : "
            << _nEvt << " + 42 = " <<  evt->getEventNumber() << std::endl ;
      _nEvt++ ;
    }
  }
  catch( EVENT::Exception& ) {
    streamlog_out(ERROR)  << " processEvent LCEvent::eventNumber not modified ! "  << std::endl;
  }
}

void TestEventModifier::end() {
  streamlog_out(MESSAGE4) << name()
			  << " modified " << _nEvt << " events in " << _nRun << " run "
			  << std::endl ;
}


MARLIN_DECLARE_PROCESSOR( TestEventModifier )
