#pragma once

// -- marlinmt headers
#include <marlinmt/MarlinMTBookConfig.h>
#include <marlinmt/Component.h>
#include <marlinmt/Logging.h>

// -- std includes
#include <unistd.h>
#include <set>

namespace marlinmt {
  
  // forward declaration
  class Application ;
  class Processor ;

  /**
   *  @brief  BookStoreManager class
   */
  class BookStoreManager : public Component {
  public:
    /// Constructor
    BookStoreManager() ;
    
    BookStoreManager( const BookStoreManager & ) = delete ;
    BookStoreManager &operator=( const BookStoreManager & ) = delete ;
    BookStoreManager( BookStoreManager && ) = delete ;
    BookStoreManager &operator=( BookStoreManager && ) = delete ;

    /**
     *  @brief reads output File from global StoreOutputFile. 
     *    if set to "" (empty) no output file will be generated. 
     */
    void writeToDisk() const ;
    
    /// Initialize the book store manager
    void initialize() override ;
    
    /**
     *  @brief  Book  a histogram 1D, float type
     *  
     *  @param  proc       the processor booking the histogram
     *  @param  path       the histogram entry path
     *  @param  name       the histogram name
     *  @param  title      the histogram title
     *  @param  axisconfig the histogram X axis config
     *  @param  flags      the book flag policy. If equal BookFlags::Default select defaults depending on steering file.
     */
    template<typename HistT>
    [[nodiscard]] book::Handle<book::Entry<HistT>> bookHist (
      const std::filesystem::path &path, 
      const std::string_view &name,
      const std::string_view &title,
      const std::array<
        const AxisConfig<typename HistT::Precision_t>*,
        HistT::Dimension> &axesconfig,
      const BookFlag_t &flags ) ;

    /**
     *  @brief add entry key to write list.
     */
    void addToWrite(const book::EntryKey& key) ;

    /**
     *  @brief remove entry key from write list. 
     */
    void removeFromWrite(const book::EntryKey& key) ;

    /**
     *  @brief thrown when try to access not existing object. 
     */
    struct ObjectNotFound : public std::exception {  
      ObjectNotFound(const std::string_view& msg) 
        : m_msg(msg) {}
      const char* what() const noexcept override { return m_msg.c_str(); }
    private:
      const std::string m_msg;
    };

    /**
     *  @brief receive key from Entry with given path and name.
     *  @param path absolute path of Object
     *  @param name of Object
     *  @throw ObjectNotFound if path don't lead to Object.
     *  @return nullptr if path not exist or is not unique
     *  @return EntryKey address else
     */
    const book::EntryKey& getKey(
        const std::filesystem::path &path,
        const std::string_view &name) const ;


    /**
     *  @brief access object managed by this store. For internal usage.
     *  @param key key for Entry to Object
     *  @return optional which contains on success a Handle for the Entry for the Object.
     */
    template<typename T>
    [[nodiscard]] book::Handle<book::Entry<T>> getObject( 
        const book::EntryKey &key) const ;

    
  private:
    /// The book store
    book::BookStore                      _bookStore {true} ;
    /// List of entry keys for Entries which should be stored at end of lifetime
    std::set<book::EntryKey>             _entriesToWrite {} ;
    /// Output file name to store objects
    StringParameter                      _outputFile {*this, "OutputFile", "The output file name for storage", "MarlinMT_"+details::convert<int>::to_string(::getpid())+".root"} ;
    /// Output file name to store objects
    StringParameter                      _defaultMemLayout {*this, "DefaultMemoryLayout", "The memory layout for objects", "Default"} ;
    /// default flag, used if flag == BookFlags::Default. 
    /// Default is shared, store. Change is steering file with: store::DefaultMemoryLayout and store::StoreByDefault.
    BookFlag_t                           _defaultFlag { 0 } ;
  };

  
}
