
// -- marlin headers
#include <marlin/CmdLineParser.h>
#include <marlin/MarlinConfig.h>
#include <marlin/Exceptions.h>
#include <marlin/Logging.h>
#include <marlin/Utils.h>

// -- TCLAP headers
#include <tclap/CmdLine.h>
#include <tclap/ValueArg.h>

namespace marlin {
  
  class Output : public TCLAP::StdOutput {
  	public:
  		void failure(TCLAP::CmdLineInterface& c, TCLAP::ArgException& e) override { 
        std::string progName = c.getProgramName();
        auto &logger = Logging::globalLogger() ;
      	logger.log<ERROR>() << "PARSE ERROR: " << e.argId() << std::endl
      		      << "             " << e.error() << std::endl << std::endl;
      	if ( c.hasHelpAndVersion() )
      		{
            
      			logger.log<ERROR>() << "Brief USAGE: " << std::endl;
            {
              auto stream = logger.log<ERROR>() ;
        			_shortUsage( c, stream );	              
            }


      			logger.log<ERROR>() << std::endl << "For complete USAGE and HELP type: " 
      					  << std::endl << "   " << progName << " "
      					  << TCLAP::Arg::nameStartString() << "help"
      					  << std::endl << std::endl;
      		}
      	else
      		usage(c);

      	throw TCLAP::ExitException(1);
  		}

  		void usage(TCLAP::CmdLineInterface& c) override {
        auto &logger = Logging::globalLogger() ;
        logger.log<MESSAGE>() << std::endl << "USAGE: " << std::endl << std::endl; 
        {
          auto stream = logger.log<MESSAGE>() ;
        	_shortUsage( c, stream );          
        }
      	logger.log<MESSAGE>() << std::endl << std::endl << "Where: " << std::endl << std::endl;
        {
          auto stream = logger.log<MESSAGE>() ;
      	  _longUsage( c, stream );
        }
      	logger.log<MESSAGE>() << std::endl; 
  		}

  		void version(TCLAP::CmdLineInterface& c) override {
        std::string progName = c.getProgramName();
        std::string xversion = c.getVersion();
        auto &logger = Logging::globalLogger() ;
        logger.log<MESSAGE>() << std::endl << progName << "  version: " 
              << xversion << std::endl << std::endl;
  		}
  };
  
  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------
  
  void CmdLineParser::setOptionalArgs( bool opt ) {
    _optionalArgs = opt ;
  }
  
  //--------------------------------------------------------------------------
  
  std::vector<std::string> CmdLineParser::getStandardOptions() const {
    return { "-c", "--config", 
      "-x", "--example", 
      "-j", "--nthreads",
      "--help", "--version" };
  }
  
  //--------------------------------------------------------------------------
  
  CmdLineParser::ParseResult CmdLineParser::parse( int argc, char**argv ) {
    ParseResult parseResult {} ;
    parseResult._arguments.reserve(argc) ;
    // Create command line object
    TCLAP::CmdLine cmdLine( "MarlinMT", ' ', MARLIN_RELEASE , true ) ;
    Output output ;
    cmdLine.setOutput( &output ) ;
    // Config argument
    TCLAP::ValueArg<std::string> configArg( "c", "config", "The input configuration settings (plugin:descriptor)", (!_optionalArgs), "", "string" ) ;
    // Dump example config argument
    TCLAP::ValueArg<std::string> exampleArg( "x", "example", "Dump an example configuration (plugin:descriptor)", false, "", "string" ) ;
    cmdLine.xorAdd( configArg, exampleArg ) ;
    // number of cores
    TCLAP::ValueArg<std::string> nthreadsArg( "j", "nthreads", "The number of worker threads (only in multi-threaded mode). 'auto' will use all available cores", false, "auto", "unsigned int / string" ) ;
    cmdLine.add( nthreadsArg ) ;
    // parse additional args
    std::vector<std::string> parserArgs{} ;
    parserArgs.reserve(argc) ; 
    parserArgs.push_back( argv[0] ) ; // for the program name
    for(int i=1 ; i<argc ; i++) {
      parseResult._arguments.push_back( argv[i] ) ;
    }
    auto &args = cmdLine.getArgList() ;
    // analyze the arguments.
    for( auto &arg : parseResult._arguments ) {
      if( arg.substr( 0, 2 ) == "--" ) {
        auto argName = arg.substr( 2 ) ;
        auto iter = std::find_if( args.begin(), args.end(), [&](auto val){
          return (val->getName() == argName) ;
        }) ;
        // Not an additional arg. Add it for later parsing
        if( args.end() != iter ) {
          parserArgs.push_back( arg ) ;
          continue ;
        }
        // Parse the additional argument
        auto tokens = details::split_string<std::string>( argName, "=" ) ;
        if( 2 != tokens.size() ) {
          MARLIN_THROW( "Additional argument '" + arg + "' with wrong syntax. Expected '--name=value' format" ) ;
        }
        auto res = parseResult._additionalArgs.insert( { tokens[0], tokens[1] } ) ;
        if( not res.second ) {
          MARLIN_THROW( "Additional argument '" + arg + "' set twice on command line" ) ;
        }
      }
      else {
        parserArgs.push_back( arg ) ;
      }
    }
    // parse the command line
    cmdLine.parse( parserArgs ) ;
    // extract command line values
    parseResult._programName = cmdLine.getProgramName() ;
    if( configArg.isSet() ) {
      parseResult._config = configArg.getValue() ;
      parseResult._dumpExample = false ;
    }
    else if( exampleArg.isSet() ) {
      parseResult._config = exampleArg.getValue() ;
      parseResult._dumpExample = true ;
    }
    parseResult._nthreads = details::nthreads( nthreadsArg.getValue() ) ;
    return parseResult ;
  }
  
}

