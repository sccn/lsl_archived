//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2013.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)
//
// See http://www.lslboost.org/libs/container for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_CONTAINER_CONTAINER_DETAIL_MPL_HPP
#define BOOST_CONTAINER_CONTAINER_DETAIL_MPL_HPP

#ifndef BOOST_CONFIG_HPP
#  include <lslboost/config.hpp>
#endif

#if defined(BOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <lslboost/container/detail/config_begin.hpp>
#include <lslboost/container/detail/workaround.hpp>
#include <lslboost/move/detail/type_traits.hpp>
#include <lslboost/intrusive/detail/mpl.hpp>

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


template <class Pair>
struct select1st
{
   typedef Pair                        argument_type;
   typedef typename Pair::first_type   result_type;

   template<class OtherPair>
   const typename Pair::first_type& operator()(const OtherPair& x) const
   {  return x.first;   }

   const typename Pair::first_type& operator()(const typename Pair::first_type& x) const
   {  return x;   }
};

}  //namespace container_detail {
}  //namespace container {
}  //namespace lslboost {

#include <lslboost/container/detail/config_end.hpp>

#endif   //#ifndef BOOST_CONTAINER_CONTAINER_DETAIL_MPL_HPP

