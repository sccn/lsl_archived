//////////////////////////////////////////////////////////////////////////////
// (C) Copyright John Maddock 2000.
// (C) Copyright Ion Gaztanaga 2005-2015.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)
//
// See http://www.lslboost.org/libs/container for documentation.
//
// The alignment and Type traits implementation comes from
// John Maddock's TypeTraits library.
//
// Some other tricks come from Howard Hinnant's papers and StackOverflow replies
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_CONTAINER_CONTAINER_DETAIL_TYPE_TRAITS_HPP
#define BOOST_CONTAINER_CONTAINER_DETAIL_TYPE_TRAITS_HPP

#ifndef BOOST_CONFIG_HPP
#  include <lslboost/config.hpp>
#endif

#if defined(BOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <lslboost/move/detail/type_traits.hpp>

namespace lslboost {
namespace container {
namespace container_detail {

using ::lslboost::move_detail::is_same;
using ::lslboost::move_detail::is_different;
using ::lslboost::move_detail::is_pointer;
using ::lslboost::move_detail::add_reference;
using ::lslboost::move_detail::add_const;
using ::lslboost::move_detail::add_const_reference;
using ::lslboost::move_detail::remove_const;
using ::lslboost::move_detail::remove_reference;
using ::lslboost::move_detail::make_unsigned;
using ::lslboost::move_detail::is_floating_point;
using ::lslboost::move_detail::is_integral;
using ::lslboost::move_detail::is_enum;
using ::lslboost::move_detail::is_pod;
using ::lslboost::move_detail::is_empty;
using ::lslboost::move_detail::is_trivially_destructible;
using ::lslboost::move_detail::is_trivially_default_constructible;
using ::lslboost::move_detail::is_trivially_copy_constructible;
using ::lslboost::move_detail::is_trivially_move_constructible;
using ::lslboost::move_detail::is_trivially_copy_assignable;
using ::lslboost::move_detail::is_trivially_move_assignable;
using ::lslboost::move_detail::is_nothrow_default_constructible;
using ::lslboost::move_detail::is_nothrow_copy_constructible;
using ::lslboost::move_detail::is_nothrow_move_constructible;
using ::lslboost::move_detail::is_nothrow_copy_assignable;
using ::lslboost::move_detail::is_nothrow_move_assignable;
using ::lslboost::move_detail::is_nothrow_swappable;
using ::lslboost::move_detail::alignment_of;
using ::lslboost::move_detail::aligned_storage;
using ::lslboost::move_detail::nat;
using ::lslboost::move_detail::max_align_t;

}  //namespace container_detail {
}  //namespace container {
}  //namespace lslboost {

#endif   //#ifndef BOOST_CONTAINER_CONTAINER_DETAIL_TYPE_TRAITS_HPP
