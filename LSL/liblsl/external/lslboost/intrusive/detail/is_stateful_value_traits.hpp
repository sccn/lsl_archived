/////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga  2009-2013.
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.lslboost.org/LICENSE_1_0.txt)
//
// See http://www.lslboost.org/libs/intrusive for documentation.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTRUSIVE_DETAIL_IS_STATEFUL_VALUE_TRAITS_HPP
#define BOOST_INTRUSIVE_DETAIL_IS_STATEFUL_VALUE_TRAITS_HPP

#include <lslboost/intrusive/detail/config_begin.hpp>

#if defined(_MSC_VER) && (_MSC_VER <= 1310)

#include <lslboost/intrusive/detail/mpl.hpp>

namespace lslboost {
namespace intrusive {
namespace detail {

template<class ValueTraits>
struct is_stateful_value_traits
{
   static const bool value = !detail::is_empty_class<ValueTraits>::value;
};

}}}

#else

#include <lslboost/intrusive/detail/function_detector.hpp>

BOOST_INTRUSIVE_CREATE_FUNCTION_DETECTOR(to_node_ptr, lslboost_intrusive)
BOOST_INTRUSIVE_CREATE_FUNCTION_DETECTOR(to_value_ptr, lslboost_intrusive)

namespace lslboost {
namespace intrusive {
namespace detail {

template<class ValueTraits>
struct is_stateful_value_traits
{
   typedef typename ValueTraits::node_ptr       node_ptr;
   typedef typename ValueTraits::pointer        pointer;
   typedef typename ValueTraits::value_type     value_type;
   typedef typename ValueTraits::const_node_ptr const_node_ptr;
   typedef typename ValueTraits::const_pointer  const_pointer;

   typedef ValueTraits value_traits;

   static const bool value =
      (lslboost::intrusive::function_detector::NonStaticFunction ==
         (BOOST_INTRUSIVE_DETECT_FUNCTION(ValueTraits, lslboost_intrusive, node_ptr, to_node_ptr, (value_type&) )))
      ||
      (lslboost::intrusive::function_detector::NonStaticFunction ==
         (BOOST_INTRUSIVE_DETECT_FUNCTION(ValueTraits, lslboost_intrusive, pointer, to_value_ptr, (node_ptr) )))
      ||
      (lslboost::intrusive::function_detector::NonStaticFunction ==
         (BOOST_INTRUSIVE_DETECT_FUNCTION(ValueTraits, lslboost_intrusive, const_node_ptr, to_node_ptr, (const value_type&) )))
      ||
      (lslboost::intrusive::function_detector::NonStaticFunction ==
         (BOOST_INTRUSIVE_DETECT_FUNCTION(ValueTraits, lslboost_intrusive, const_pointer, to_value_ptr, (const_node_ptr) )))
      ;
};

}}}

#endif

#include <lslboost/intrusive/detail/config_end.hpp>

#endif   //@ifndef BOOST_INTRUSIVE_DETAIL_IS_STATEFUL_VALUE_TRAITS_HPP
