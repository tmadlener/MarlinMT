// -- marlin headers
#include <marlin/Configuration.h>
#include <marlin/Parameter.h>
#include <marlin/PluginManager.h>

// -- std headers
#include <algorithm>
#include <filesystem>

namespace marlin {
  
  ConfigSection::ConfigSection( const std::string &name ) : 
    _name(name) {
    /* nop */
  }
  
  //--------------------------------------------------------------------------

  const std::string &ConfigSection::name() const {
    return _name ;
  }
  
  //--------------------------------------------------------------------------
  
  bool ConfigSection::empty() const {
    return (_subsections.empty() && _parameters.empty() ) ;
  }
  
  //--------------------------------------------------------------------------
  
  ConfigSection &ConfigSection::section( const std::string &n ) {
    auto iter = _subsections.find( n ) ;
    if( _subsections.end() == iter ) {
      MARLIN_THROW( "No sub-section '" + n + "' in section '" + name() + "'" ) ;
    }
    return iter->second ;
  }
  
  //--------------------------------------------------------------------------

  const ConfigSection &ConfigSection::section( const std::string &n ) const {
    auto iter = _subsections.find( n ) ;
    if( _subsections.end() == iter ) {
      MARLIN_THROW( "No sub-section '" + n + "' in section '" + name() + "'" ) ;
    }
    return iter->second ;
  }
  
  //--------------------------------------------------------------------------
  
  ConfigSection &ConfigSection::addSection( const std::string &n ) {
    auto iter = _subsections.find( n ) ;
    if( _subsections.end() != iter ) {
      MARLIN_THROW( "Sub-section '" + n + "' already present in section '" + name() + "'" ) ;
    }
    return _subsections.emplace( n, ConfigSection{n} ).first->second ;
  }
  
  //--------------------------------------------------------------------------

  bool ConfigSection::hasSection( const std::string &n ) const {
    return (_subsections.end() != _subsections.find( n )) ;
  }
  
  //--------------------------------------------------------------------------
  
  std::vector<std::string> ConfigSection::subsectionNames() const {
    std::vector<std::string> names( _subsections.size() ) ;
    std::transform( _subsections.begin(), _subsections.end(), names.begin(), [] (const auto &iter) { return iter.first ; } ) ;
    return names ;
  }
  
  //--------------------------------------------------------------------------

  bool ConfigSection::hasParameter( const std::string &n ) const {
    return (_parameters.end() != _parameters.find( n )) ;
  }
  
  //--------------------------------------------------------------------------
  
  std::vector<std::string> ConfigSection::parameterNames() const {
    std::vector<std::string> names( _parameters.size() ) ;
    std::transform( _parameters.begin(), _parameters.end(), names.begin(), [] (const auto &iter) { return iter.first ; } ) ;
    return names ;
  }
  
  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------
  
  bool Configuration::hasConstant( const std::string &cn ) const {
    return ( _constants.end() != _constants.find( cn ) ) ;
  }
  
  //--------------------------------------------------------------------------
  
  const Configuration::ConstantsMap &Configuration::constants() const {
    return _constants ;
  }
  
  //--------------------------------------------------------------------------
  
  ConfigSection &Configuration::createSection( const std::string &sn ) {
    auto iter = _sections.find( sn ) ;
    if( _sections.end() != iter ) {
      MARLIN_THROW( "Section '" + sn + "' already exists" ) ;
    }
    return _sections.emplace( sn, ConfigSection{sn} ).first->second ;
  }
  
  //--------------------------------------------------------------------------
  
  ConfigSection &Configuration::section( const std::string &sn ) {
    auto iter = _sections.find( sn ) ;
    if( _sections.end() == iter ) {
      MARLIN_THROW( "Section '" + sn + "' doesn't exists" ) ;
    }
    return iter->second ;
  }
  
  //--------------------------------------------------------------------------
  
  const ConfigSection &Configuration::section( const std::string &sn ) const {
    auto iter = _sections.find( sn ) ;
    if( _sections.end() == iter ) {
      MARLIN_THROW( "Section '" + sn + "' doesn't exists" ) ;
    }
    return iter->second ;
  }
  
  //--------------------------------------------------------------------------
  
  std::vector<std::string> Configuration::sections() const {
    return details::keys( _sections ) ;
  }
  
  //--------------------------------------------------------------------------
  
  void Configuration::replaceConstants( std::string& str ) const {
    size_t pos = str.find("${") ;
    while( pos != std::string::npos ) {
      size_t pos2 = str.find_first_of( "}", pos+2 ) ;
      if( pos2 == std::string::npos ) {
        MARLIN_THROW_T( ParseException, "Couldn't parse constant value in string '" + str + "'. It seems you have an open bracket '${' but it is not closed" ) ;
      }
      const std::string key( str.substr( pos+2 , pos2-pos-2 )) ;
      auto iter = _constants.find( key ) ;
      const std::string replacementValue( iter != _constants.end() ? iter->second : "" ) ;
      if( replacementValue.empty() ) {
        MARLIN_THROW_T( ParseException, "Constant '" + key + "' doesn't exists !" ) ;
      }
      str.replace( pos , (pos2+1-pos) , replacementValue ) ;
      pos2 = pos + replacementValue.size() ; // new end position after replace
      pos = str.find("${", pos2) ; // find next possible key to replace
    }
  }
  
  //--------------------------------------------------------------------------
  
  std::pair<std::string, std::string> ConfigHelper::splitPluginInput( const std::string &str ) {
    auto splitVals = details::split_string<std::string>( str, ":", 2 ) ;
    return (2 == splitVals.size()) ? std::pair{ splitVals[0], splitVals[1] } : std::pair{ "", splitVals[0] } ;
  }
  
  //--------------------------------------------------------------------------
  
  void ConfigHelper::readConfig( const std::string &str, Configuration &cfg, const ConfigReader::ReplacementParametersMap &params ) {
    auto pluginInput = ConfigHelper::splitPluginInput( str ) ;
    // if no plugin guess it from the input string
    if( pluginInput.first.empty() ) {
      std::filesystem::path filepath = pluginInput.second ;
      if( filepath.extension().string() == ".xml" ) {
        pluginInput.first = "XMLConfigReader" ;
      }
    }
    if( pluginInput.first.empty() ) {
      MARLIN_THROW( "No config reader plugin found in config string. Please specify your config as 'plugin_name:input'" ) ;
    }
    auto reader = PluginManager::instance().create<ConfigReader>( pluginInput.first ) ;
    if( nullptr == reader ) {
      MARLIN_THROW( "No ConfigReader plugin found for type '" + pluginInput.first + "'" ) ;
    }
    reader->init( pluginInput.second ) ;
    reader->read( cfg, params ) ;
  }
  
  //--------------------------------------------------------------------------
  
  void ConfigHelper::writeConfig( const std::string &str, Configuration &cfg ) {
    auto pluginInput = ConfigHelper::splitPluginInput( str ) ;
    // if no plugin guess it from the input string
    if( pluginInput.first.empty() ) {
      std::filesystem::path filepath = pluginInput.second ;
      if( filepath.extension().string() == ".xml" ) {
        pluginInput.first = "XMLConfigWriter" ;
      }
    }
    if( pluginInput.first.empty() ) {
      MARLIN_THROW( "No config writer plugin found in config string. Please specify your config as 'plugin_name:input'" ) ;
    }
    auto writer = PluginManager::instance().create<ConfigWriter>( pluginInput.first ) ;
    if( nullptr == writer ) {
      MARLIN_THROW( "No ConfigWriter plugin found for type '" + pluginInput.first + "'" ) ;
    }
    writer->init( pluginInput.second ) ;
    writer->write( cfg ) ;
  }
  
}