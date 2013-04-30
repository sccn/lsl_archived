//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2008-2011. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
//
// See http://www.lslboost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTERPROCESS_CONTAINERS_ALLOCATION_TYPE_HPP
#define BOOST_INTERPROCESS_CONTAINERS_ALLOCATION_TYPE_HPP

#if (defined _MSC_VER) && (_MSC_VER >= 1200)
#  pragma once
#endif

#include <lslboost/interprocess/detail/config_begin.hpp>
#include <lslboost/container/detail/allocation_type.hpp>

namespace lslboost {
namespace interprocess {

/// @cond
typedef int allocation_type;
/// @endcond
static const allocation_type allocate_new       = lslboost::container::allocate_new;
static const allocation_type expand_fwd         = lslboost::container::expand_fwd;
static const allocation_type expand_bwd         = lslboost::container::expand_bwd;
static const allocation_type shrink_in_place    = lslboost::container::shrink_in_place;
static const allocation_type try_shrink_in_place= lslboost::container::try_shrink_in_place;
static const allocation_type nothrow_allocation = lslboost::container::nothrow_allocation;
static const allocation_type zero_memory        = lslboost::container::zero_memory;

}  //namespace interprocess {
}  //namespace lslboost {

#include <lslboost/interprocess/detail/config_end.hpp>

#endif //   #ifndef  BOOST_INTERPROCESS_CONTAINERS_VERSION_TYPE_HPP
