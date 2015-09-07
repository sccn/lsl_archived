//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2008-2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
//
// See http://www.lslboost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTERPROCESS_DETAIL_VARIADIC_TEMPLATES_TOOLS_HPP
#define BOOST_INTERPROCESS_DETAIL_VARIADIC_TEMPLATES_TOOLS_HPP

#ifndef BOOST_CONFIG_HPP
#  include <lslboost/config.hpp>
#endif
#
#if defined(BOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <lslboost/container/detail/variadic_templates_tools.hpp>

namespace lslboost {
namespace interprocess {
namespace ipcdetail {

using lslboost::container::container_detail::tuple;
using lslboost::container::container_detail::build_number_seq;
using lslboost::container::container_detail::index_tuple;
using lslboost::container::container_detail::get;

}}}   //namespace lslboost { namespace interprocess { namespace ipcdetail {

#endif   //#ifndef BOOST_INTERPROCESS_DETAIL_VARIADIC_TEMPLATES_TOOLS_HPP
