#ifndef TestProcessorEventSeeder_h
#define TestProcessorEventSeeder_h 1

#include "marlin/Processor.h"

#include "lcio.h"
#include <string>

#include <map>
#include <vector>

using namespace lcio ;
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

  /** Input collection name.
   */
  std::string _colName="";

  std::map< unsigned long long, unsigned int>  _seeds{};

  int _nRun=-1;
  int _nEvt=-1;
} ;

#endif
