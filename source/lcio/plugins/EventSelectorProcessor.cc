
// -- marlinmt headers
#include <marlinmt/Processor.h>
#include <marlinmt/ProcessorApi.h>
#include <marlinmt/PluginManager.h>
#include <marlinmt/EventExtensions.h>

// -- lcio headers
#include <lcio.h>

// -- std headers
#include <set>
#include <map>

namespace marlinmt {

  /** Simple event selector processor. Returns true if the given event
   *  was specified in the EvenList parameter.
   *
   *  <h4>Output</h4>
   *  returns true or false
   *
   * @param  EventList:   pairs of: EventNumber RunNumber
   *
   * @author F. Gaede, DESY
   * @version $Id:$
   */
  class EventSelectorProcessor : public Processor {
    using EventNumberSet = std::set< std::pair< int, int > > ;

  public:
    /**
     *  @brief  Constructor
     */
    EventSelectorProcessor() ;

    // from Processor
    void init() ;
    void processEvent( EventStore * evt ) ;

  protected:
    IntVectorParameter _evtList {*this, "EventList",
              "event list - pairs of Eventnumber RunNumber" } ;
    ///< The event list as a set
    EventNumberSet        _evtSet {} ;
  };

  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  EventSelectorProcessor::EventSelectorProcessor() :
    Processor("EventSelector") {
    // modify processor description
    setDescription( "EventSelectorProcessor returns true if given event was specified in EventList" ) ;
  }

  //--------------------------------------------------------------------------

  void EventSelectorProcessor::init() {
    // usually a good idea to
    printParameters() ;
    unsigned int nEvts = _evtList.size() ;
    if( nEvts % 2 != 0 ) {
      throw Exception( "EventSelectorProcessor: event list size should be even (list of run / event ids)" ) ;
    }
    for( unsigned i=0 ; i <nEvts ; i+=2 ) {
      _evtSet.insert( std::make_pair( _evtList.at(i) , _evtList.at( i+1 ) ) ) ;
    }
  }

  //--------------------------------------------------------------------------

  void EventSelectorProcessor::processEvent( EventStore * evt ) {
    auto lcevent = evt->event<EVENT::LCEvent>() ;
    // if no events specified - always return true
    if( _evtList.size() == 0 ) {
      ProcessorApi::setReturnValue( this, evt, true ) ;
      return ;
    }
    auto iter = _evtSet.find( std::make_pair( lcevent->getEventNumber() , lcevent->getRunNumber() ) ) ;
    const bool isInList = (iter != _evtSet.end() ) ;
    //-- note: this will not be printed if compiled w/o MARLINMTDEBUG=1 !
    log<DEBUG>() << "   processing event: " << lcevent->getEventNumber()
  		       << "   in run:  " << lcevent->getRunNumber()
  		       << " - in event list : " << isInList
  		       << std::endl ;
    ProcessorApi::setReturnValue( this, evt, isInList ) ;
  }

  // plugin declaration
  MARLINMT_DECLARE_PROCESSOR( EventSelectorProcessor )
}
