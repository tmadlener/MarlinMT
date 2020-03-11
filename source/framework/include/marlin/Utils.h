#ifndef MARLIN_UTILS_h
#define MARLIN_UTILS_h 1

// -- std headers
#include <map>
#include <cmath>
#include <string>
#include <sstream>
#include <typeinfo>
#include <chrono>
#include <vector>
#include <algorithm>

// -- marlin headers
#include <marlin/Exceptions.h>

namespace marlin {

  /**
   *  @brief  clock class
   *  Provide a wrapper around a certain clock type in std
   *  to perform safe clock measurement in multi-threading
   *  environement
   */
  class clock {
  public:
    using clock_type = std::chrono::steady_clock ;
    using time_point = clock_type::time_point ;
    using duration_rep = float ;
    using pair = std::pair<duration_rep, duration_rep> ;
    // helper definitions
    using nanoseconds = std::chrono::duration<duration_rep, std::nano> ;
    using microseconds = std::chrono::duration<duration_rep, std::micro> ;
    using milliseconds = std::chrono::duration<duration_rep, std::milli> ;
    using seconds = std::chrono::duration<duration_rep> ;
    using minutes = std::chrono::duration<duration_rep, std::ratio<60>> ;

  public:
    // static API only
    clock() = delete ;
    ~clock() = delete ;

  public:
    /**
     *  @brief  Get the current time
     */
    static time_point now() {
      return clock_type::now() ;
    }

    /**
     *  @brief  Get the elapsed time since a previous time point.
     *  The template parameter 'unit' allows for casting the result
     *  to a specific unit (default seconds). Use the duration types
     *  defined in this class to get a correct calculation.
     *  The result is floating type.
     *
     *  @param  since an earlier time point
     */
    template <class unit = seconds>
    static duration_rep elapsed_since( const time_point &since ) {
      auto current = now() ;
      return std::chrono::duration_cast<unit>( current - since ).count() ;
    }

    /**
     *  @brief  Get the time difference between two time points.
     *  The template parameter 'unit' allows for casting the result
     *  to a specific unit (default seconds). Use the duration types
     *  defined in this class to get a correct calculation.
     *  The result is floating type.
     *  Example:
     *  @code{cpp}
     *  auto start = clock::now();
     *  // do stuff
     *  auto end = clock::now();
     *  // get the time difference in milliseconds as a float
     *  auto ms_spent = clock::time_difference<clock::milliseconds>( start, end );
     *  @endcode
     *
     *  @param  older the oldest time point
     *  @param  ealier the earliest time point
     */
    template <class unit = seconds>
    static duration_rep time_difference( const time_point &older, const time_point &ealier ) {
      return std::chrono::duration_cast<unit>( ealier - older ).count() ;
    }

    /**
     *  @brief  Crunch numbers for some time.
     *  The template parameter specifies the unit
     *
     *  @param crunchTime the time duration
     */
    template <class unit = seconds>
    static void crunchFor( duration_rep crunchTime ) {
      auto start = clock::now() ;
      auto now = start ;
      clock::duration_rep timediff = 0 ;
      while ( timediff < crunchTime ) {
        (void)std::sqrt(2) ;
        now = clock::now() ;
        timediff = clock::time_difference<unit>(start, now) ;
      }
    }
  };

  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  /**
   *  @brief  HashHelper class
   *  Helper class to generate hash 64 id
   */
  class HashHelper {
  private:
    /// Init value for hash 64
    static const unsigned long long int hashinit = 14695981039346656037ull ;

  public:
    static constexpr unsigned long long int doByte(unsigned long long int hash, unsigned char val) {
      return (hash ^ val) * 1099511628211ull ;
    }

    /**
     *  @brief  Generate a hash 64 from a string
     *
     *  @param  key the input string key
     */
    static unsigned long long int hash64( const char *key ) {
      unsigned char* str = (unsigned char*)key ;
      unsigned long long int hash = hashinit ;
      for ( ; *str; ++str) hash = doByte(hash, *str) ;
      return hash ;
    }

    /**
     *  @brief  Generate a hash 64 from the typeid name
     */
    template <typename T>
    static unsigned long long int typeHash64() {
      static unsigned long long int code = hash64(typeid(T).name()) ;
      return code ;
    }
  };

  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  /**
   *  @brief  StringUtil class
   *          Simple utility class for string operations
   */
  class StringUtil {
  public:
    /**
     *  @brief  Convert a type to string
     *
     *  @param  var the value to convert
     */
    template <typename T>
    static std::string typeToString( const T &var ) ;

    /**
     *  @brief  Convert a vector of type to vector of string
     *
     *  @param  vars the values to convert
     */
    template <typename T>
    static std::vector<std::string> typeToString( const std::vector<T> &vars ) ;

    /**
     *  @brief  Convert a variable to string
     *
     *  @param  str the string to convert
     */
    template <typename T>
    static T stringToType( const std::string &str ) ;

    /**
     *  @brief  Convert a string vector to a vector of specified type
     *
     *  @param  strs the string vector to convert
     */
    template <typename T>
    static std::vector<T> stringToType( const std::vector<std::string> &strs ) ;

    /**
     *  @brief  Split the string with the corresponding delimiter
     *
     *  @param  inputString the input string to split
     *  @param  delimiter the string delimiter
     */
    template <typename T>
    static std::vector<T> split( const std::string &inputString, const std::string &delimiter = " " ) ;

    /**
     *  @brief  Weird overload for scalar types.
     *  Just returns the conversion to string.
     *  See overload version with vector instead.
     *
     *  @param  input the input value
     *  @param  delimiter the string delimiter (unused here)
     */
    template <typename T>
    static std::string join( const T &input, const std::string &delimiter = " " ) ;

    /**
     *  @brief  Join the input values from the vector with the corresponding delimiter
     *  and returns a string representation of it.
     *
     *  @param  input the input vector or values to join
     *  @param  delimiter the string delimiter
     */
    template <typename T>
    static std::string join( const std::vector<T> &input, const std::string &delimiter = " " ) ;
  };

  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  template <typename T>
  inline std::string StringUtil::typeToString( const T &var ) {
    std::ostringstream oss ;
    if ((oss << var).fail()) {
      throw Exception( "Couldn't convert value to string" ) ;
    }
    return oss.str() ;
  }

  //--------------------------------------------------------------------------

  template <>
  inline std::string StringUtil::typeToString( const std::string &var ) {
    return var ;
  }

  //--------------------------------------------------------------------------

  template <>
  inline std::string StringUtil::typeToString( const bool &var ) {
    return (var ? "true" : "false") ;
  }

  //--------------------------------------------------------------------------

  template <typename T>
  inline std::vector<std::string> StringUtil::typeToString( const std::vector<T> &vars ) {
    std::vector<std::string> result ;
    result.reserve( vars.size() ) ;
    for( auto var : vars ) {
      result.push_back( typeToString<T>( var ) ) ;
    }
    return result ;
  }

  //--------------------------------------------------------------------------

  template <typename T>
  inline T StringUtil::stringToType( const std::string &str ) {
    T t ;
    std::istringstream iss(str) ;
    if ((iss >> t).fail()) {
      throw Exception( "Couldn't convert string to value" ) ;
    }
    return t ;
  }

  //--------------------------------------------------------------------------

  template <>
  inline std::string StringUtil::stringToType( const std::string &str ) {
    return str;
  }

  //--------------------------------------------------------------------------

  template <>
  inline bool StringUtil::stringToType( const std::string &str ) {
    if ( str == "true"  || str == "1" || str == "on"  ) return true ;
    if ( str == "false" || str == "0" || str == "off" ) return false ;
    throw Exception( "Couldn't convert value to string" ) ;
  }

  //--------------------------------------------------------------------------

  template <typename T>
  inline std::vector<T> StringUtil::stringToType( const std::vector<std::string> &strs ) {
    std::vector<T> result ;
    result.reserve( strs.size() ) ;
    for( auto str : strs ) {
      result.push_back( stringToType<T>( str ) ) ;
    }
    return result ;
  }

  //--------------------------------------------------------------------------

  template <typename T>
  inline std::vector<T> StringUtil::split(const std::string &inputString, const std::string &delimiter) {
    std::string::size_type lastPos = inputString.find_first_not_of(delimiter, 0);
    std::string::size_type pos = inputString.find_first_of(delimiter, lastPos);
    typename std::vector<T> tokens ;
    while ((std::string::npos != pos) || (std::string::npos != lastPos)) {
      T value = stringToType<T>(inputString.substr(lastPos, pos - lastPos)) ;
      tokens.push_back(value) ;
      lastPos = inputString.find_first_not_of(delimiter, pos) ;
      pos = inputString.find_first_of(delimiter, lastPos) ;
    }
    return tokens ;
  }

  //--------------------------------------------------------------------------

  // weird overloading function that just converts the value to string ...
  template <typename T>
  inline std::string StringUtil::join( const T &input, const std::string &/*delimiter*/ ) {
    return typeToString( input ) ;
  }

  //--------------------------------------------------------------------------

  template <typename T>
  inline std::string StringUtil::join( const std::vector<T> &input, const std::string &delimiter ) {
    std::stringstream ss ;
    for( auto iter = input.begin() ; iter != input.end() ; ++iter ) {
      ss << typeToString( *iter ) ;
      if( std::next(iter) != input.end() ) {
        ss << delimiter ;
      }
    }
    return ss.str() ;
  }
  
  namespace details {
    
    template <typename T>
    struct type_info {
      static const char* type ;
    };
    
    template <typename T> const char* type_info<T>::type(typeid(T).name());

#define TYPE_INFO_IMPL( type_id, str ) \
    template <> const char* type_info<type_id>::type(str);

    TYPE_INFO_IMPL( std::nullptr_t, "null" )
    TYPE_INFO_IMPL( bool, "bool" )
    TYPE_INFO_IMPL( short, "short" )
    TYPE_INFO_IMPL( int, "int" )    
    TYPE_INFO_IMPL( unsigned int, "unsigned int" )
    TYPE_INFO_IMPL( float, "float" )    
    TYPE_INFO_IMPL( double, "double" )
    TYPE_INFO_IMPL( char, "char" )
    TYPE_INFO_IMPL( std::string, "string" )
    TYPE_INFO_IMPL( std::vector<short>, "vector<short>" )
    TYPE_INFO_IMPL( std::vector<int>, "vector<int>" )
    TYPE_INFO_IMPL( std::vector<unsigned int>, "vector<unsigned int>" )
    TYPE_INFO_IMPL( std::vector<float>, "vector<float>" )
    TYPE_INFO_IMPL( std::vector<double>, "vector<double>" )
    TYPE_INFO_IMPL( std::vector<char>, "vector<char>" )
    TYPE_INFO_IMPL( std::vector<std::string>, "vector<string>" )

#undef TYPE_INFO_IMPL

    //--------------------------------------------------------------------------

    
    template <typename T>
    struct convert {
      static std::string to_string( const T &value ) {
        std::ostringstream oss ;
        if ((oss << value).fail()) {
          MARLIN_THROW( "to_string failure: " + std::string(details::type_info<T>::type) ) ;
        }
        return oss.str() ;
      }
      
      static T from_string( const std::string &str ) {
        T t ;
        std::istringstream iss(str) ;
        if ((iss >> t).fail()) {
          MARLIN_THROW( "from_string failure: " + std::string(details::type_info<T>::type) ) ;
        }
        return t ;
      }
    };
    
    //--------------------------------------------------------------------------
    
    template <>
    struct convert<std::string> {
      static std::string to_string( const std::string &value ) {
        return value ;
      }
      
      static std::string from_string( const std::string &str ) {
        return str ;
      }
    };
    
    //--------------------------------------------------------------------------
    
    
    template <>
    struct convert<bool> {
      static std::string to_string( const bool &value ) {
        return value ? "true" : "false" ;
      }
      
      static bool from_string( const std::string &str ) {
        static const auto true_list = { "true", "1", "on" } ;
        static const auto false_list = { "false", "0", "off" } ;
        auto strcp = str ;
        std::transform(str.begin(), str.end(), strcp.begin(),
          [](unsigned char c){ return std::tolower(c); } ) ;
        if( std::find( std::begin(true_list), std::end(true_list), strcp ) != std::end(true_list) ) {
          return true ;
        }
        if( std::find( std::begin(false_list), std::end(false_list), strcp ) != std::end(false_list) ) {
          return false ;
        }
        MARLIN_THROW( "from_string failure: bool" ) ;
      }
    };
    
    //--------------------------------------------------------------------------
    
    template <typename T>
    struct convert<std::vector<T>> {
      static std::string to_string( const std::vector<T> &value ) {
        std::stringstream ss ;
        for( auto iter = value.begin() ; iter != value.end() ; ++iter ) {
          ss << details::convert<T>::to_string( *iter ) ;
          if( std::next(iter) != value.end() ) {
            ss << " " ;
          }
        }
        return ss.str() ;
      }
      
      static std::vector<T> from_string( const std::string &str ) {
        std::string::size_type lastPos = str.find_first_not_of(" ", 0);
        std::string::size_type pos = str.find_first_of(" ", lastPos);
        typename std::vector<T> tokens ;
        while ((std::string::npos != pos) || (std::string::npos != lastPos)) {
          tokens.emplace_back(
            details::convert<T>::from_string(str.substr(lastPos, pos - lastPos))
          );
          lastPos = str.find_first_not_of(" ", pos) ;
          pos = str.find_first_of(" ", lastPos) ;
        }
        return tokens ;
      }
    };
    
    //--------------------------------------------------------------------------
    
    template <>
    struct convert<std::vector<std::string>> {
      static std::string to_string( const std::vector<std::string> &value ) {
        std::stringstream ss ;
        for( auto iter = value.begin() ; iter != value.end() ; ++iter ) {
          ss << *iter ;
          if( std::next(iter) != value.end() ) {
            ss << " " ;
          }
        }
        return ss.str() ;
      }
      
      static std::vector<std::string> from_string( const std::string &str ) {
        std::string::size_type lastPos = str.find_first_not_of(" ", 0);
        std::string::size_type pos = str.find_first_of(" ", lastPos);
        std::vector<std::string> tokens ;
        while ((std::string::npos != pos) || (std::string::npos != lastPos)) {
          tokens.emplace_back( str.substr(lastPos, pos - lastPos) ) ;
          lastPos = str.find_first_not_of(" ", pos) ;
          pos = str.find_first_of(" ", lastPos) ;
        }
        return tokens ;
      }
    };
    
    //--------------------------------------------------------------------------
    
    template <typename T>
    inline std::vector<T> split_string(const std::string &str, const std::string &delimiter, size_t maxTokens = std::numeric_limits<std::size_t>::max()) {
      if( 0 == maxTokens ) {
        MARLIN_THROW( "maxTokens can't be 0" ) ;
      }
      std::string::size_type lastPos = str.find_first_not_of(delimiter, 0) ;
      std::string::size_type pos = str.find_first_of(delimiter, lastPos) ;
      typename std::vector<T> tokens ;
      while ((std::string::npos != pos) || (std::string::npos != lastPos)) {
        if( tokens.size()+1 >= maxTokens ) {
          tokens.emplace_back(
            details::convert<T>::from_string(str.substr(lastPos))
          );
          break ;
        }
        tokens.emplace_back(
          details::convert<T>::from_string(str.substr(lastPos, pos - lastPos))
        );
        lastPos = str.find_first_not_of(delimiter, pos) ;
        pos = str.find_first_of(delimiter, lastPos) ;
      }
      return tokens ;
    }
    
    //--------------------------------------------------------------------------
    
    template <>
    inline std::vector<std::string> split_string(const std::string &str, const std::string &delimiter, size_t maxTokens) {
      if( 0 == maxTokens ) {
        MARLIN_THROW( "maxTokens can't be 0" ) ;
      }
      std::string::size_type lastPos = str.find_first_not_of(delimiter, 0) ;
      std::string::size_type pos = str.find_first_of(delimiter, lastPos) ;
      std::vector<std::string> tokens ;
      while ((std::string::npos != pos) || (std::string::npos != lastPos)) {
        if( tokens.size()+1 >= maxTokens ) {
          tokens.emplace_back(
            str.substr(lastPos)
          );
          break ;
        }
        tokens.emplace_back(
          str.substr(lastPos, pos - lastPos)
        );
        lastPos = str.find_first_not_of(delimiter, pos) ;
        pos = str.find_first_of(delimiter, lastPos) ;
      }
      return tokens ;
    }
    
  }

} // end namespace marlin

#endif
