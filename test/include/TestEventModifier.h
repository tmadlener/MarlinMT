#ifndef TestEventModifier_h
#define TestEventModifier_h 1

#include "marlin/Processor.h"

#include "lcio.h"
#include <string>


using namespace lcio ;
using namespace marlin ;


/**  Example processor for marlin.
 *
 *  If compiled with MARLIN_USE_AIDA
 *  it creates a histogram (cloud) of the MCParticle energies.
 *
 *  <h4>Input - Prerequisites</h4>
 *  Needs the collection of MCParticles.
 *
 *  <h4>Output</h4>
 *  A histogram.
 *
 * @param CollectionName Name of the MCParticle collection
 *
 * @author F. Gaede, DESY
 * @version $Id: TestEventModifier.h,v 1.4 2005-10-11 12:57:39 gaede Exp $
 */

class TestEventModifier : public Processor {
 public:
  TestEventModifier() ;

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
  int _nRun = {0} ;
  int _nEvt = {0} ;
} ;

#endif
