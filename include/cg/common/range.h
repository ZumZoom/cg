#pragma once

#include <iterator>
#include <boost/range/iterator.hpp>

namespace cg {
namespace common
{
   template<class BidIter>
   struct range_circulator
   {
      typedef typename BidIter::value_type value_type;

      range_circulator(BidIter begin, BidIter end)
         : beg_(begin)
         , end_(end)
         , it_(begin)
      {}

      range_circulator(BidIter begin, BidIter end, BidIter it)
         : beg_(begin)
         , end_(end)
         , it_(it)
      {}

      value_type const & operator *  () const { return *it_;  }
      value_type const * operator -> () const { return &*it_; }

      BidIter iter() const { return it_; }

      range_circulator & operator ++ ()
      {
         ++it_;
         if (it_ == end_)
            it_ = beg_;
         return *this;
      }

      const range_circulator operator ++ (int)
      {
         range_circulator tmp = *this;
         ++(*this);
         return tmp;
      }

      range_circulator & operator -- ()
      {
         if (it_ == beg_)
            it_ = end_;
         --it_;
         return *this;
      }

      const range_circulator operator -- (int)
      {
         range_circulator tmp = *this;
         --(*this);
         return tmp;
      }

      const range_circulator operator + (int i)
      {
         range_circulator tmp = *this;
         while(i-- > 0)
            ++tmp;
         return tmp;
      }

      const range_circulator operator - (int i)
      {
         range_circulator tmp = *this;
         while(i-- > 0)
            --tmp;
         return tmp;
      }

      template<typename Range>
      friend bool operator == (   range_circulator<Range> const & a,
                                  range_circulator<Range> const & b  );

   private:
      BidIter beg_, end_, it_;
   };

   template<typename Range>
   bool operator == (  range_circulator<Range> const & a,
                       range_circulator<Range> const & b  )
   {
      return a.it_ == b.it_ && a.beg_ == b.beg_ && a.end_ == b.end_;
   }

   template<typename Range>
   bool operator != (  range_circulator<Range> const & a,
                       range_circulator<Range> const & b  )
   {
      return !(a == b);
   }
}}
