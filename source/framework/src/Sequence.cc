#include <marlinmt/Sequence.h>

// -- marlinmt headers
#include <marlinmt/Processor.h>
#include <marlinmt/Exceptions.h>
#include <marlinmt/EventExtensions.h>
#include <marlinmt/PluginManager.h>
#include <marlinmt/Configuration.h>

// -- std headers
#include <algorithm>
#include <iomanip>

namespace marlinmt {

  SequenceItem::SequenceItem( std::shared_ptr<Processor> proc ) :
    _processor(proc),
    _mutex(nullptr) {
    if( nullptr == _processor ) {
      throw Exception( "SequenceItem: got a nullptr for processor" ) ;
    }
  }

  //--------------------------------------------------------------------------

  SequenceItem::SequenceItem( std::shared_ptr<Processor> proc, std::shared_ptr<std::mutex> lock ) :
    _processor(proc),
    _mutex(lock) {
    if( nullptr == _processor ) {
      throw Exception( "SequenceItem: got a nullptr for processor" ) ;
    }
  }

  //--------------------------------------------------------------------------

  void SequenceItem::processRunHeader( std::shared_ptr<RunHeader> rhdr ) {
    if( nullptr != _mutex ) {
      std::lock_guard<std::mutex> lock( *_mutex ) ;
      _processor->processRunHeader( rhdr.get() ) ;
    }
    else {
      _processor->processRunHeader( rhdr.get() ) ;
    }
  }

  //--------------------------------------------------------------------------

  clock::pair SequenceItem::processEvent( std::shared_ptr<EventStore> event ) {
    if( nullptr != _mutex ) {
      auto start = clock::now() ;
      std::lock_guard<std::mutex> lock( *_mutex ) ;
      auto start2 = clock::now() ;
      _processor->processEvent( event.get() ) ;
      auto end = clock::now() ;
      return clock::pair(
        clock::time_difference<clock::seconds>(start, end),
        clock::time_difference<clock::seconds>(start2, end)) ;
    }
    else {
      auto start = clock::now() ;
      _processor->processEvent( event.get() ) ;
      auto end = clock::now() ;
      return clock::pair(
        clock::time_difference<clock::seconds>(start, end),
        clock::time_difference<clock::seconds>(start, end)) ;
    }
  }

  //--------------------------------------------------------------------------

  std::shared_ptr<Processor> SequenceItem::processor() const {
    return _processor ;
  }

  //--------------------------------------------------------------------------

  const std::string &SequenceItem::name() const {
    return _processor->name() ;
  }

  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  std::shared_ptr<SequenceItem> Sequence::createItem( std::shared_ptr<Processor> processor, std::shared_ptr<std::mutex> lock ) const {
    return std::make_shared<SequenceItem>( processor, lock ) ;
  }

  //--------------------------------------------------------------------------

  void Sequence::addItem( std::shared_ptr<SequenceItem> item ) {
    auto iter = std::find_if(_items.begin(), _items.end(), [&](std::shared_ptr<SequenceItem> i){
      return (i->name() == item->name()) ;
    });
    if( _items.end() != iter ) {
      throw Exception( "Sequence::addItem: processor '" + item->name() + "' already in sequence" ) ;
    }
    _items.push_back( item ) ;
    _clockMeasures[item->name()] = ClockMeasure() ;
  }

  //--------------------------------------------------------------------------

  std::shared_ptr<SequenceItem> Sequence::at( Index index ) const {
    return _items.at( index ) ;
  }

  //--------------------------------------------------------------------------

  Sequence::SizeType Sequence::size() const {
    return _items.size() ;
  }

  //--------------------------------------------------------------------------

  void Sequence::processRunHeader( std::shared_ptr<RunHeader> rhdr ) {
    for ( auto item : _items ) {
      item->processRunHeader( rhdr ) ;
    }
  }

  //--------------------------------------------------------------------------

  void Sequence::processEvent( std::shared_ptr<EventStore> event ) {
    try {
      auto extension = event->extensions().get<extensions::ProcessorConditions, ProcessorConditionsExtension>() ;
      for ( auto item : _items ) {
        if ( not extension->check( item->name() ) ) {
          continue ;
        }
        auto clockMeas = item->processEvent( event ) ;
        auto iter = _clockMeasures.find( item->name() ) ;
        iter->second._appClock += clockMeas.first ;
        iter->second._procClock += clockMeas.second ;
        iter->second._counter ++ ;
      }
    }
    catch ( SkipEventException& e ) {
      auto iter = _skipEventMap.find( e.what() ) ;
      if ( _skipEventMap.end() == iter ) {
        _skipEventMap.insert( SkippedEventMap::value_type( e.what() , 1 ) ) ;
      }
      else {
        iter->second ++;
      }
    }
  }

  //--------------------------------------------------------------------------

  ClockMeasure Sequence::clockMeasureSummary() const {
    ClockMeasure summary {} ;
    for ( auto t : _clockMeasures ) {
      summary._appClock += t.second._appClock ;
      summary._procClock += t.second._procClock ;
      summary._counter += t.second._counter ;
    }
    return summary ;
  }

  //--------------------------------------------------------------------------

  const Sequence::ClockMeasureMap &Sequence::clockMeasures() const {
    return _clockMeasures ;
  }

  //--------------------------------------------------------------------------

  const Sequence::SkippedEventMap &Sequence::skippedEvents() const {
    return _skipEventMap ;
  }

  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  SuperSequence::SuperSequence( std::size_t nseqs ) {
    if( 0 == nseqs ) {
      throw Exception( "SuperSequence: number of sequences must be > 0" ) ;
    }
    _sequences.resize(nseqs) ;
    for( std::size_t i=0 ; i<nseqs ; ++i ) {
      _sequences.at(i) = std::make_shared<Sequence>() ;
    }
  }

  //--------------------------------------------------------------------------

  void SuperSequence::init( Application *app ) {
    for( auto item : _uniqueItems ) {
      item->processor()->setup( app ) ;
    }
  }

  //--------------------------------------------------------------------------

  std::shared_ptr<Sequence> SuperSequence::sequence( Index index ) const {
    return _sequences.at( index ) ;
  }

  //--------------------------------------------------------------------------

  void SuperSequence::addProcessor( const ConfigSection &parameters ) {
    
    const bool cloneSet = parameters.hasParameter( "ProcessorClone" ) ;
    const bool criticalSet = parameters.hasParameter( "ProcessorCritical" ) ;
    bool clone = parameters.parameter<bool>( "ProcessorClone", true ) ;
    bool critical = parameters.parameter<bool>( "ProcessorCritical", false ) ;
    auto type = parameters.parameter<std::string>( "ProcessorType" ) ;
    auto name = parameters.parameter<std::string>( "ProcessorName" ) ;
    auto &pluginMgr = PluginManager::instance() ;
    std::shared_ptr<Processor> processor = pluginMgr.create<Processor>( type ) ;
    if( nullptr == processor ) {
      throw Exception( "Processor of type '" + type + "' doesn't exists !" ) ;
    }
    auto cloneOpt = processor->runtimeOption( Processor::ERuntimeOption::eClone ) ;
    auto criticalOpt = processor->runtimeOption( Processor::ERuntimeOption::eCritical ) ;
    if( cloneOpt.has_value() ) {
      if( cloneSet and (cloneOpt.value() != clone) ) {
        throw Exception( "Processor '" +
        type +
        "' clone option forced to " +
        (cloneOpt.value() ? "true" : "false") +
        "!") ;
      }
      clone = cloneOpt.value() ;
    }
    if( criticalOpt.has_value() ) {
      if( criticalSet and (criticalOpt.value() != critical) ) {
        throw Exception( "Processor '" +
        type +
        "' critical option forced to " +
        (criticalOpt.value() ? "true" : "false") +
        "!") ;
      }
      critical = criticalOpt.value() ;
    }
    processor->setName( name ) ;
    processor->setParameters( parameters ) ;
    std::shared_ptr<std::mutex> lock = critical ? std::make_shared<std::mutex>() : nullptr ;
    if( clone ) {
      // add the first but then create new processor instances and add them
      auto item = _sequences.at(0)->createItem( processor, lock ) ;
      _sequences.at(0)->addItem( item ) ;
      _uniqueItems.insert( item ) ;
      for( SizeType i=1 ; i<size() ; ++i ) {
        processor = pluginMgr.create<Processor>( type ) ;
        processor->setName( name ) ;
        processor->setParameters( parameters ) ;
        item = _sequences.at(i)->createItem( processor, lock ) ;
        _sequences.at(i)->addItem( item ) ;
        _uniqueItems.insert( item ) ;
      }
    }
    else {
      // add the first and re-use the same item
      auto item = _sequences.at(0)->createItem( processor, lock ) ;
      _sequences.at(0)->addItem( item ) ;
      _uniqueItems.insert( item ) ;
      for( SizeType i=1 ; i<size() ; ++i ) {
        _sequences.at(i)->addItem( item ) ;
      }
    }
  }

  //--------------------------------------------------------------------------

  SuperSequence::SizeType SuperSequence::size() const {
    return _sequences.size() ;
  }

  //--------------------------------------------------------------------------

  void SuperSequence::processRunHeader( std::shared_ptr<RunHeader> rhdr ) {
    for( auto item : _uniqueItems ) {
      item->processRunHeader( rhdr ) ;
    }
  }

  //--------------------------------------------------------------------------

  void SuperSequence::end() {
    for( auto item : _uniqueItems ) {
      item->processor()->end() ;
    }
  }

  //--------------------------------------------------------------------------

  void SuperSequence::printStatistics( Logging::Logger logger ) const {
    // first merge measurements from the different sequences
    Sequence::SkippedEventMap skippedEvents {} ;
    Sequence::ClockMeasureMap clockMeasures {} ;
    for( unsigned int i=0 ; i<size() ; ++i ) {
      auto skipped = sequence(i)->skippedEvents() ;
      auto clocks = sequence(i)->clockMeasures() ;
      // merge skipped events stats
      for( auto sk : skipped ) {
        auto iter = skippedEvents.find( sk.first ) ;
        if( skippedEvents.end() != iter ) {
          iter->second += sk.second ;
        }
        else {
          skippedEvents.insert( sk ) ;
        }
      }
      // merge clocks stats
      for( auto clk : clocks ) {
        auto iter = clockMeasures.find( clk.first ) ;
        if( clockMeasures.end() != iter ) {
          iter->second._appClock += clk.second._appClock ;
          iter->second._procClock += clk.second._procClock ;
          iter->second._counter += clk.second._counter ;
        }
        else {
          clockMeasures.insert( clk ) ;
        }
      }
    }
    logger->log<MESSAGE>() << "--------------------------------------------------------- " << std::endl ;
    logger->log<MESSAGE>() << "-- Events skipped by processors : " << std::endl ;
    unsigned int nSkipped = 0 ;
    for( auto skip : skippedEvents ) {
      logger->log<MESSAGE>() << "--       " << skip.first << ": \t" << skip.second << std::endl ;
      nSkipped += skip.second ;
    }
    logger->log<MESSAGE>() << "-- Total: " << nSkipped  << std::endl ;
    logger->log<MESSAGE>() << "--------------------------------------------------------- " << std::endl
          << std::endl ;
    logger->log<MESSAGE>() << "--------------------------------------------------------- " << std::endl
          << "      Time used by processors ( in processEvent() ) :      " << std::endl
          << std::endl ;
    std::list<Sequence::ClockMeasureMap::value_type> clockList( clockMeasures.begin() , clockMeasures.end() ) ;
    typedef std::list<Sequence::ClockMeasureMap::value_type>::value_type elt ;
    clockList.sort( [](const elt &lhs, const elt &rhs) {
      return ( lhs.second._procClock > rhs.second._procClock ) ;
    }) ;
    double clockTotal = 0.0 ;
    int eventTotal = 0 ;
    for( auto clockMeasure : clockList ) {
      std::string procName = clockMeasure.first ;
      procName.resize(40, ' ') ;
      clockTotal += clockMeasure.second._procClock ;
      int lockTimeFraction = static_cast<int>(((clockMeasure.second._appClock - clockMeasure.second._procClock) / clockMeasure.second._appClock) * 100.) ;
      if( clockMeasure.second._counter > eventTotal ){
        eventTotal = clockMeasure.second._counter ;
      }
      std::stringstream ss ;
      if ( clockMeasure.second._counter > 0 ) {
        ss << clockMeasure.second._procClock / static_cast<float>(clockMeasure.second._counter) ;
      }
      else {
        ss << "NaN" ;
      }
      std::stringstream lockPrint ;
      if( size() > 1 ) {
        lockPrint << "(lock: " << lockTimeFraction << " %)";
      }
      logger->log<MESSAGE>()
        << procName
        << std::setw(12) << std::scientific  << clockMeasure.second._procClock  << " s "
        << "in " << std::setw(12) << clockMeasure.second._counter
        << " events  ==> "
        << std::setw(12) << std::scientific << ss.str() << " [ s/evt.] "
        << lockPrint.str()
        << std::endl << std::endl ;
    }
    std::stringstream ss ;
    if ( eventTotal > 0 ) {
      ss << clockTotal / eventTotal ;
    }
    else {
      ss << "NaN" ;
    }
    logger->log<MESSAGE>()
      << "Total:                                  "
      <<  std::setw(12) << std::scientific  << clockTotal << " s in "
      <<  std::setw(12) << eventTotal << " events  ==> "
      <<  std::setw(12) << std::scientific << ss.str() << " [ s/evt.] "
      << std::endl << std::endl ;
    logger->log<MESSAGE>() << "--------------------------------------------------------- " << std::endl ;
  }

}
