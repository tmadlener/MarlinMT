
// -- marlinmt headers
#include <marlinmt/Processor.h>
#include <marlinmt/ProcessorApi.h>
#include <marlinmt/Logging.h>
#include <marlinmt/PluginManager.h>

// -- std headers
#include <cmath>
#include <ctime>
#include <random>

namespace marlinmt {

  /** Simple processor crunching CPU time for n milliseconds.
   *
   *  <h4>Input - Prerequisites</h4>
   *  none
   *  <h4>Output</h4>
   *  none
   * @parameter CrunchTime the time in millisecond to crunch CPU
   *
   * @author R.Ete, DESY
   */
  class CPUCrunchingProcessor : public Processor {

   public:
    CPUCrunchingProcessor() ;
    void init() override ;
    void processEvent( EventStore * evt ) override ;

  private:
    IntParameter _crunchTime {*this, "CrunchTime",
             "The crunching time (unit ms)", 200 } ;

    FloatParameter _crunchSigma {*this, "CrunchSigma",
             "Smearing factor on the crunching time using a gaussian generator (unit ms)", 0 } ;
  };

  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  CPUCrunchingProcessor::CPUCrunchingProcessor() :
    Processor("CPUCrunching") {
    // modify processor description
    setDescription( "CPUCrunchingProcessor crunch CPU time for n milliseconds" ) ;
    // parameters validation
    _crunchTime.setValidator( validator::greaterEqual<int>(0) ) ;
    _crunchSigma.setValidator( validator::greaterEqual<float>(0) ) ;
  }

  //--------------------------------------------------------------------------

  void CPUCrunchingProcessor::init() {
    log<DEBUG>() << "CPUCrunchingProcessor::init() called" << std::endl ;
    // usually a good idea to
    printParameters() ;
    ProcessorApi::registerForRandomSeeds( this ) ;
  }

  //--------------------------------------------------------------------------

  void CPUCrunchingProcessor::processEvent( EventStore *event ) {
    auto randomSeed = ProcessorApi::getRandomSeed( this, event ) ;
    std::default_random_engine generator( randomSeed );
    std::normal_distribution<clock::duration_rep> distribution(0, _crunchSigma);
    clock::duration_rep totalCrunchTime = static_cast<clock::duration_rep>(_crunchTime) + distribution(generator) ;
    log<MESSAGE>() << "Will use total crunch time of " << totalCrunchTime << " ms" << std::endl ;
    // crunch for n milliseconds !
    clock::crunchFor<clock::milliseconds>(totalCrunchTime) ;
  }

  // processor declaration
  MARLINMT_DECLARE_PROCESSOR( CPUCrunchingProcessor )
}
