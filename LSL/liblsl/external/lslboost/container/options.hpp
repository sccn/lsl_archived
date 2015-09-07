/////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga  2013-2013
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.lslboost.org/LICENSE_1_0.txt)
//
// See http://www.lslboost.org/libs/container for documentation.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_CONTAINER_OPTIONS_HPP
#define BOOST_CONTAINER_OPTIONS_HPP

#ifndef BOOST_CONFIG_HPP
#  include <lslboost/config.hpp>
#endif

#if defined(BOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <lslboost/container/detail/config_begin.hpp>
#include <lslboost/container/container_fwd.hpp>
#include <lslboost/intrusive/pack_options.hpp>

namespace lslboost {
namespace container {

#if !defined(BOOST_CONTAINER_DOXYGEN_INVOKED)

template<tree_type_enum TreeType, bool OptimizeSize>
struct tree_opt
{
   static const lslboost::container::tree_type_enum tree_type = TreeType;
   static const bool optimize_size = OptimizeSize;
};

#endif   //!defined(BOOST_CONTAINER_DOXYGEN_INVOKED)

//!This option setter specifies the underlying tree type
//!(red-black, AVL, Scapegoat or Splay) for ordered associative containers
BOOST_INTRUSIVE_OPTION_CONSTANT(tree_type, tree_type_enum, TreeType, tree_type)

//!This option setter specifies if node size is optimized
//!storing rebalancing data masked into pointers for ordered associative containers
BOOST_INTRUSIVE_OPTION_CONSTANT(optimize_size, bool, Enabled, optimize_size)

//! Helper metafunction to combine options into a single type to be used
//! by \c lslboost::container::set, \c lslboost::container::multiset
//! \c lslboost::container::map and \c lslboost::container::multimap.
//! Supported options are: \c lslboost::container::optimize_size and \c lslboost::container::tree_type
#if defined(BOOST_CONTAINER_DOXYGEN_INVOKED) || defined(BOOST_CONTAINER_VARIADIC_TEMPLATES)
template<class ...Options>
#else
template<class O1 = void, class O2 = void, class O3 = void, class O4 = void>
#endif
struct tree_assoc_options
{
   /// @cond
   typedef typename ::lslboost::intrusive::pack_options
      < tree_assoc_defaults,
      #if !defined(BOOST_CONTAINER_VARIADIC_TEMPLATES)
      O1, O2, O3, O4
      #else
      Options...
      #endif
      >::type packed_options;
   typedef tree_opt<packed_options::tree_type, packed_options::optimize_size> implementation_defined;
   /// @endcond
   typedef implementation_defined type;
};

}  //namespace container {
}  //namespace lslboost {

#include <lslboost/container/detail/config_end.hpp>

#endif   //#ifndef BOOST_CONTAINER_OPTIONS_HPP
