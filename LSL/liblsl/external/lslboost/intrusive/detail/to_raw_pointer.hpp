/////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga  2014-2014
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.lslboost.org/LICENSE_1_0.txt)
//
// See http://www.lslboost.org/libs/intrusive for documentation.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTRUSIVE_DETAIL_TO_RAW_POINTER_HPP
#define BOOST_INTRUSIVE_DETAIL_TO_RAW_POINTER_HPP

#ifndef BOOST_CONFIG_HPP
#  include <lslboost/config.hpp>
#endif

#if defined(BOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <lslboost/intrusive/detail/config_begin.hpp>
#include <lslboost/intrusive/detail/pointer_element.hpp>

namespace lslboost {
namespace intrusive {
namespace detail {

template <class T>
inline T* to_raw_pointer(T* p)
{  return p; }

template <class Pointer>
inline typename lslboost::intrusive::pointer_element<Pointer>::type*
to_raw_pointer(const Pointer &p)
{  return lslboost::intrusive::detail::to_raw_pointer(p.operator->());  }

} //namespace detail
} //namespace intrusive
} //namespace lslboost

#include <lslboost/intrusive/detail/config_end.hpp>

#endif //BOOST_INTRUSIVE_DETAIL_UTILITIES_HPP
