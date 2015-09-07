#ifndef BOOST_INTERPROCESS_DETAIL_SWAP_HPP
#define BOOST_INTERPROCESS_DETAIL_SWAP_HPP
//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2014-2015. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
//
// See http://www.lslboost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_CONFIG_HPP
#  include <lslboost/config.hpp>
#endif
#
#if defined(BOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

namespace lslboost { namespace interprocess {

template<class T>
void simple_swap(T&x, T&y)
{  T tmp(x); x = y; y = tmp;  }

}}  //namespace lslboost{ namespace interprocess {

#endif //#ifndef BOOST_INTERPROCESS_DETAIL_SWAP_HPP
