#pragma once

// -- std includes
#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

// -- Marlin includes
// #include  "marlinmt/Exceptions.h"

// -- MarlinBook includes
#include "marlinmt/book/Condition.h"
#include "marlinmt/book/Entry.h"
#include "marlinmt/book/MemLayout.h"

namespace marlinmt {
  namespace book {

    // -- MarlinBook forward declaration
    class BookStore ;
    template < typename >
    class Entry ;
    class Selection;

    /**
     *  @brief Wrapper for weak pointer to Entry. Mainly used for Selections. 
     */
    class WeakEntry {
      friend Selection ;
      friend BookStore ;
        
      // constructor
      explicit WeakEntry( const std::shared_ptr< const details::Entry > &entry ) ;
      // constructor
      explicit WeakEntry( const std::shared_ptr< details::Entry > &entry ) ;
      WeakEntry() = default;
    public:

      /**
       *  @brief check if WeakEntry is usable.
       *  @return false when Entry referenced by WeakEntry no longer exist
       */
      [[nodiscard]] bool valid() const ;

      /**
       *  @brief get key from Entry.
       */
      [[nodiscard]] const EntryKey &key() const ;

      /**
       *  @brief create  new Handle for Entry.
       *  @attention don't use old Handle to the Entry after this.
       *  @throws BookStoreException when WeakEntry is invalid.
       */
      template <typename T>
      [[nodiscard]]
      Handle<Entry<T>> handle() const {
        if( ! valid() ) {
          MARLIN_BOOK_THROW( "Try to bind an expired WeakEntry");  
        }
        return Handle<Entry<T>>(_entry.lock());
      }

    private:
      /// wake reference to Entry
      std::weak_ptr< const details::Entry > _entry {};
    } ;

    /**
     *  @brief Contains references to entries.
     *  Which satisfy the condition.
     *  Used to doing action on a range of entries.
     */
    class Selection {

    public:

      /**
       *  @brief Construct Selection from range of Entries.
       *  @param begin,end range of Entries
       *  @param cond Condition to filter Entries.
       *  @tparam T iterator type
       */
      template < typename T >
      static Selection find( T begin, T end, const Condition &cond ) ;

      /**
       *  @brief Search for first Entry which matches condition.
       *  @param begin,end range of Entries.
       *  @param cond Condition to search.
       *  @tparam T iterator type
       */
      template < typename T >
      static WeakEntry findFirst( T begin, T end, const Condition &cond ) ;

      /**
       *  @brief random access const_iterator for Selections.
       */
      using const_iterator = typename std::vector< WeakEntry >::const_iterator ;

      /// Possibilities to compose Conditions when creating sub selections.
      /// Composed the new condition with the condition from the super
      /// selection.
      enum struct ComposeStrategy { AND, ONLY_CHILD, ONLY_PARENT } ;

      /// default constructor. Construct empty selection.
      Selection() = default ;

      Selection( const Selection & ) = delete ;
      Selection &operator=( const Selection & ) = delete ;

      /// move constructor. Default
      Selection( Selection && ) = default ;
      Selection &operator=( Selection && ) = default ;

      ~Selection() = default ;
      /**
       *  @brief construct sub selection.
       *  @param sel super selection
       *  @param cond condition for promotion in sub selection
       *  @param strategy to compose selection with sub selection condition.
       */
      Selection( const Selection &sel,
                 const Condition &cond,
                 ComposeStrategy  strategy = ComposeStrategy::AND ) ;

      /// getter for Condition which every Entry full fill.
      [[nodiscard]] const Condition &condition() const ;

      /// begin iterator to iterate through entries.
      [[nodiscard]] const_iterator begin() const ;

      /// end iterator for entries. First not valid iterator.
      [[nodiscard]] const_iterator end() const ;

      /// @return number of entries included in the selection.
      [[nodiscard]] std::size_t size() const ;

      /**
       *  @brief construct sub selection.
       *  @param cond condition for promotion in sub selection.
       *  @param strategy to compos selection with sub selection condition.
       */
      Selection find( const Condition &cond,
                      ComposeStrategy  strategy = ComposeStrategy::AND ) ;

      /**
       *  @brief get WeakEntry at position.
       *  @param i position of entry of interest.
       */
      const WeakEntry &get( std::size_t i ) { return _entries[i]; }

      /**
       *  @brief remove entry at position.
       *  @param i position of entry to remove.
       */
      void remove( std::size_t i ) ;

      /**
       *  @brief remove entry range.
       *  @param i position of first entry to remove.
       *  @param n number of entries to remove.
       */
      void remove( std::size_t i, std::size_t n ) ;

      /**
       *  @brief remove entry.
       *  @param itr iterator from entry which should be removed.
       */
      void remove( const const_iterator &itr ) ;

      /**
       *  @brief remove entry range.
       *  remove entries from begin to end, including begin, excluding end.
       *  @param begin first entry which should be removed.
       *  @param end first entry which should not be removed.
       */
      void remove( const const_iterator &begin, const const_iterator &end ) ;

    private:
      /// entries which included in selection.
      std::vector< WeakEntry > _entries{} ;
      /// condition which every entry fulfill.
      Condition _condition{} ;
    } ;

    //--------------------------------------------------------------------------

    template < typename T >
    Selection Selection::find( T begin, T end, const Condition &cond ) {
      Selection res{} ;
      res._condition = cond ;

      auto dst = std::back_inserter( res._entries ) ;

      auto fn = [&c = cond]( const typename T::value_type &i ) -> bool {
        const WeakEntry h = static_cast< WeakEntry >( i ) ;
        return h.valid() && c( h.key() ) ;
      } ;

      for ( auto itr = begin; itr != end; ++itr ) {
        if ( fn( *itr ) ) {
          *dst++ = static_cast< WeakEntry >( *itr ) ;
        }
      }
      return res ;
    }

    //--------------------------------------------------------------------------
    
    template < typename T >
    WeakEntry Selection::findFirst(T begin, T end, const Condition &cond ) {
      using value_type = std::remove_cv_t<std::remove_reference_t<decltype(*begin)>>;
      static_assert(
        std::is_same_v< value_type, std::shared_ptr<details::Entry>>
        || std::is_same_v<value_type, std::shared_ptr<const details::Entry>>, "needs container of shared pointer to Entry");
      for ( auto itr = begin; itr != end; ++itr ) {
        if(cond((*itr)->key())) {
          return WeakEntry(*itr);
        }
      }
      return WeakEntry();
    }



  } // end namespace book
} // end namespace marlinmt
