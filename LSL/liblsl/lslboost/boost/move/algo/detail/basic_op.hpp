//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2015-2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/move for documentation.
//
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_MOVE_ALGO_BASIC_OP
#define BOOST_MOVE_ALGO_BASIC_OP

#ifndef BOOST_CONFIG_HPP
#  include <boost/config.hpp>
#endif
#
#if defined(BOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <boost/move/utility_core.hpp>
#include <boost/move/adl_move_swap.hpp>
#include <boost/move/detail/iterator_traits.hpp>

namespace lslboost {
namespace movelib {

struct forward_t{};
struct backward_t{};
struct three_way_t{};
struct three_way_forward_t{};
struct four_way_t{};

struct move_op
{
   template <class SourceIt, class DestinationIt>
   BOOST_MOVE_FORCEINLINE void operator()(SourceIt source, DestinationIt dest)
   {  *dest = ::lslboost::move(*source);  }

   template <class SourceIt, class DestinationIt>
   BOOST_MOVE_FORCEINLINE DestinationIt operator()(forward_t, SourceIt first, SourceIt last, DestinationIt dest_begin)
   {  return ::lslboost::move(first, last, dest_begin);  }

   template <class SourceIt, class DestinationIt>
   BOOST_MOVE_FORCEINLINE DestinationIt operator()(backward_t, SourceIt first, SourceIt last, DestinationIt dest_last)
   {  return ::lslboost::move_backward(first, last, dest_last);  }

   template <class SourceIt, class DestinationIt1, class DestinationIt2>
   BOOST_MOVE_FORCEINLINE void operator()(three_way_t, SourceIt srcit, DestinationIt1 dest1it, DestinationIt2 dest2it)
   {
      *dest2it = lslboost::move(*dest1it);
      *dest1it = lslboost::move(*srcit);
   }

   template <class SourceIt, class DestinationIt1, class DestinationIt2>
   DestinationIt2 operator()(three_way_forward_t, SourceIt srcit, SourceIt srcitend, DestinationIt1 dest1it, DestinationIt2 dest2it)
   {
      //Destination2 range can overlap SourceIt range so avoid lslboost::move
      while(srcit != srcitend){
         this->operator()(three_way_t(), srcit++, dest1it++, dest2it++);
      }
      return dest2it;
   }

   template <class SourceIt, class DestinationIt1, class DestinationIt2, class DestinationIt3>
   BOOST_MOVE_FORCEINLINE void operator()(four_way_t, SourceIt srcit, DestinationIt1 dest1it, DestinationIt2 dest2it, DestinationIt3 dest3it)
   {
      *dest3it = lslboost::move(*dest2it);
      *dest2it = lslboost::move(*dest1it);
      *dest1it = lslboost::move(*srcit);
   }
};

struct swap_op
{
   template <class SourceIt, class DestinationIt>
   BOOST_MOVE_FORCEINLINE void operator()(SourceIt source, DestinationIt dest)
   {  lslboost::adl_move_swap(*dest, *source);  }

   template <class SourceIt, class DestinationIt>
   BOOST_MOVE_FORCEINLINE DestinationIt operator()(forward_t, SourceIt first, SourceIt last, DestinationIt dest_begin)
   {  return lslboost::adl_move_swap_ranges(first, last, dest_begin);  }

   template <class SourceIt, class DestinationIt>
   BOOST_MOVE_FORCEINLINE DestinationIt operator()(backward_t, SourceIt first, SourceIt last, DestinationIt dest_begin)
   {  return lslboost::adl_move_swap_ranges_backward(first, last, dest_begin);  }

   template <class SourceIt, class DestinationIt1, class DestinationIt2>
   BOOST_MOVE_FORCEINLINE void operator()(three_way_t, SourceIt srcit, DestinationIt1 dest1it, DestinationIt2 dest2it)
   {
      typename ::lslboost::movelib::iterator_traits<SourceIt>::value_type tmp(lslboost::move(*dest2it));
      *dest2it = lslboost::move(*dest1it);
      *dest1it = lslboost::move(*srcit);
      *srcit = lslboost::move(tmp);
   }

   template <class SourceIt, class DestinationIt1, class DestinationIt2>
   DestinationIt2 operator()(three_way_forward_t, SourceIt srcit, SourceIt srcitend, DestinationIt1 dest1it, DestinationIt2 dest2it)
   {
      while(srcit != srcitend){
         this->operator()(three_way_t(), srcit++, dest1it++, dest2it++);
      }
      return dest2it;
   }

   template <class SourceIt, class DestinationIt1, class DestinationIt2, class DestinationIt3>
   BOOST_MOVE_FORCEINLINE void operator()(four_way_t, SourceIt srcit, DestinationIt1 dest1it, DestinationIt2 dest2it, DestinationIt3 dest3it)
   {
      typename ::lslboost::movelib::iterator_traits<SourceIt>::value_type tmp(lslboost::move(*dest3it));
      *dest3it = lslboost::move(*dest2it);
      *dest2it = lslboost::move(*dest1it);
      *dest1it = lslboost::move(*srcit);
      *srcit = lslboost::move(tmp);
   }
};


}} //namespace lslboost::movelib

#endif   //BOOST_MOVE_ALGO_BASIC_OP
