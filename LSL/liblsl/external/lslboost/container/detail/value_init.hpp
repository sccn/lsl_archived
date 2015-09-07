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

#ifndef BOOST_CONTAINER_DETAIL_VALUE_INIT_HPP
#define BOOST_CONTAINER_DETAIL_VALUE_INIT_HPP

#ifndef BOOST_CONFIG_HPP
#  include <lslboost/config.hpp>
#endif

#if defined(BOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <lslboost/container/detail/config_begin.hpp>
#include <lslboost/container/detail/workaround.hpp>

namespace lslboost {
namespace container {
namespace container_detail {

template<class T>
struct value_init
{
   value_init()
      : m_t()
   {}

   operator T &() { return m_t; }

   T m_t;
};

}  //namespace container_detail {
}  //namespace container {
}  //namespace lslboost {

#include <lslboost/container/detail/config_end.hpp>

#endif   //#ifndef BOOST_CONTAINER_DETAIL_VALUE_INIT_HPP
