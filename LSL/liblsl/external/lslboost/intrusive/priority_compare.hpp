/////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2008
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.lslboost.org/LICENSE_1_0.txt)
//
// See http://www.lslboost.org/libs/intrusive for documentation.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTRUSIVE_PRIORITY_COMPARE_HPP
#define BOOST_INTRUSIVE_PRIORITY_COMPARE_HPP

#include <lslboost/intrusive/detail/config_begin.hpp>
#include <lslboost/intrusive/intrusive_fwd.hpp>

#include <functional>

namespace lslboost {
namespace intrusive {

template <class T>
struct priority_compare
   : public std::binary_function<T, T, bool>
{
   bool operator()(const T &val, const T &val2) const
   {
      return priority_order(val, val2);
   }
};

/// @cond

template<class Less, class T>
struct get_prio
{
   typedef Less type;
};


template<class T>
struct get_prio<void, T>
{
   typedef ::lslboost::intrusive::priority_compare<T> type;
};

/// @endcond

} //namespace intrusive 
} //namespace lslboost 

#include <lslboost/intrusive/detail/config_end.hpp>

#endif //BOOST_INTRUSIVE_PRIORITY_COMPARE_HPP
