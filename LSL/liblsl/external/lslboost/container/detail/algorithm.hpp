//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2014-2014.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)
//
// See http://www.lslboost.org/libs/container for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_CONTAINER_DETAIL_ALGORITHM_HPP
#define BOOST_CONTAINER_DETAIL_ALGORITHM_HPP

#ifndef BOOST_CONFIG_HPP
#  include <lslboost/config.hpp>
#endif

#if defined(BOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <lslboost/intrusive/detail/algorithm.hpp>

namespace lslboost {
namespace container {

using lslboost::intrusive::algo_equal;
using lslboost::intrusive::algo_lexicographical_compare;

}  //namespace container {
}  //namespace lslboost {

#endif   //#ifndef BOOST_CONTAINER_DETAIL_ALGORITHM_HPP
