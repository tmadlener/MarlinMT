
// -- marlin headers
#include "marlin/Processor.h"
#include "marlin/Logging.h"
#include "marlin/ProcessorApi.h"
#include "marlin/PluginManager.h"

// -- lcio headers
#include "IMPL/LCEventImpl.h"
#include "IMPL/LCRunHeaderImpl.h"

using namespace EVENT ;
using namespace IMPL ;
using namespace marlin ;


/**  test processor for testing the uniquie event seeding functionality of the ProcessorEventSeeder service in Marlin.
 *
 *
 * @author S. J. Aplin, DESY
 */

class TestProcessorEventSeeder : public Processor {
 public:

  TestProcessorEventSeeder() ;

 /** Called at the begin of the job before anything is read.
   * Use to initialize the processor, e.g. book histograms.
   */
  void init() ;

  /** Called for every run.
   */
  void processRunHeader( LCRunHeader* run ) ;

  /** Called for every event - the working horse.
   */
  void processEvent( LCEvent * evt ) ;

  /** Called after data processing for clean up.
   */
  void end() ;

protected:
  std::map< unsigned long long, unsigned int>  _seeds {} ;
  int _nRun = {0} ;
  int _nEvt = {0} ;
} ;

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

TestProcessorEventSeeder::TestProcessorEventSeeder() : Processor("TestProcessorEventSeeder") {

  // modify processor description
  _description = "TestProcessorEventSeeder test the uniquie event seeding functionality of the ProcessorEventSeeder service in Marlin" ;

  _nRun = -1 ;
  _nEvt = -1 ;

}

//--------------------------------------------------------------------------

void TestProcessorEventSeeder::init() {
  ProcessorApi::registerForRandomSeeds( this ) ;
}

//--------------------------------------------------------------------------

void TestProcessorEventSeeder::processRunHeader( EVENT::LCRunHeader* ) {
  ++_nRun ;
}

//--------------------------------------------------------------------------

void TestProcessorEventSeeder::processEvent( EVENT::LCEvent * evt ) {

  streamlog_out(DEBUG) << "   processing event: " << evt->getEventNumber()
		       << "   in run:  " << evt->getRunNumber()
		       << std::endl ;

  unsigned int seed = ProcessorApi::getRandomSeed( this, evt ) ;

  streamlog_out( DEBUG ) << "seed set to "
			    << seed
			    << " for event "
			    << evt->getEventNumber()
			    << std::endl;

  try {
    ProcessorApi::registerForRandomSeeds( this ) ;
  }
  catch( marlin::Exception& ) {
    log<ERROR>() << name() << " failed to register processor to event seed generator (TEST is OK)" << std::endl ;
  }

  unsigned long long runnum_and_eventnum = evt->getRunNumber() ;
  runnum_and_eventnum = runnum_and_eventnum << 32  ;

  runnum_and_eventnum += evt->getEventNumber() ;

  std::map< unsigned long long, unsigned int>::iterator it ;

  it = _seeds.find(runnum_and_eventnum);

  if( it == _seeds.end() ){
    _seeds[runnum_and_eventnum] = seed ;
  }
  else {

    if( seed != _seeds[runnum_and_eventnum] ) {
      streamlog_out(ERROR) << " Seeds don't match for"
			   << " run " <<   evt->getRunNumber()
			   << " event " << evt->getEventNumber()
			   << " old seed = " << _seeds[runnum_and_eventnum]
			   << " new seed = " << seed
			   << std::endl ;
    }
    else{
      streamlog_out(DEBUG) << " Seeds match for"
			   << " run " <<   evt->getRunNumber()
			   << " event " << evt->getEventNumber()
			   << " old seed = " << _seeds[runnum_and_eventnum]
			   << " new seed = " << seed
			   << std::endl ;

    }
  }
  ++_nEvt ;
}

//--------------------------------------------------------------------------

void TestProcessorEventSeeder::end(){

  streamlog_out(MESSAGE4) << name()
			  << " random seeds received for " << _nEvt << " events in " << _nRun << " run "
			  << std::endl ;

}

MARLIN_DECLARE_PROCESSOR( TestProcessorEventSeeder )
