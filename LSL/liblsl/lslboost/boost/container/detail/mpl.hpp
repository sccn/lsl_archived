//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2013.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/container for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_CONTAINER_CONTAINER_DETAIL_MPL_HPP
#define BOOST_CONTAINER_CONTAINER_DETAIL_MPL_HPP

#ifndef BOOST_CONFIG_HPP
#  include <boost/config.hpp>
#endif

#if defined(BOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <boost/container/detail/config_begin.hpp>
#include <boost/container/detail/workaround.hpp>
#include <boost/move/detail/type_traits.hpp>
#include <boost/intrusive/detail/mpl.hpp>

#include <cstddef>

namespace lslboost {
namespace container {
namespace container_detail {

using lslboost::move_detail::integral_constant;
using lslboost::move_detail::true_type;
using lslboost::move_detail::false_type;
using lslboost::move_detail::enable_if_c;
using lslboost::move_detail::enable_if;
using lslboost::move_detail::enable_if_convertible;
using lslboost::move_detail::disable_if_c;
using lslboost::move_detail::disable_if;
using lslboost::move_detail::disable_if_convertible;
using lslboost::move_detail::is_convertible;
using lslboost::move_detail::if_c;
using lslboost::move_detail::if_;
using lslboost::move_detail::identity;
using lslboost::move_detail::bool_;
using lslboost::move_detail::true_;
using lslboost::move_detail::false_;
using lslboost::move_detail::yes_type;
using lslboost::move_detail::no_type;
using lslboost::move_detail::bool_;
using lslboost::move_detail::true_;
using lslboost::move_detail::false_;
using lslboost::move_detail::unvoid_ref;
using lslboost::move_detail::and_;
using lslboost::move_detail::or_;
using lslboost::move_detail::not_;
using lslboost::move_detail::enable_if_and;
using lslboost::move_detail::disable_if_and;
using lslboost::move_detail::enable_if_or;
using lslboost::move_detail::disable_if_or;

template <class FirstType>
struct select1st
{
   typedef FirstType type;

   template<class T>
   const type& operator()(const T& x) const
   {  return x.first;   }

   template<class T>
   type& operator()(T& x)
   {  return const_cast<type&>(x.first);   }
};

}  //namespace container_detail {
}  //namespace container {
}  //namespace lslboost {

#include <boost/container/detail/config_end.hpp>

#endif   //#ifndef BOOST_CONTAINER_CONTAINER_DETAIL_MPL_HPP

