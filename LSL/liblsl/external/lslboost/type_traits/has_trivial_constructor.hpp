
//  (C) Copyright Steve Cleary, Beman Dawes, Howard Hinnant & John Maddock 2000.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt).
//
//  See http://www.lslboost.org/libs/type_traits for most recent version including documentation.

#ifndef BOOST_TT_HAS_TRIVIAL_CONSTRUCTOR_HPP_INCLUDED
#define BOOST_TT_HAS_TRIVIAL_CONSTRUCTOR_HPP_INCLUDED

#include <lslboost/type_traits/config.hpp>
#include <lslboost/type_traits/intrinsics.hpp>
#include <lslboost/type_traits/is_pod.hpp>
#include <lslboost/type_traits/detail/ice_or.hpp>

// should be the last #include
#include <lslboost/type_traits/detail/bool_trait_def.hpp>

namespace lslboost {

namespace detail {

template <typename T>
struct has_trivial_ctor_impl
{
#ifdef BOOST_HAS_TRIVIAL_CONSTRUCTOR
   BOOST_STATIC_CONSTANT(bool, value =
      (::lslboost::type_traits::ice_or<
         ::lslboost::is_pod<T>::value,
         BOOST_HAS_TRIVIAL_CONSTRUCTOR(T)
      >::value));
#else
   BOOST_STATIC_CONSTANT(bool, value =
      (::lslboost::type_traits::ice_or<
         ::lslboost::is_pod<T>::value,
         false
      >::value));
#endif
};

} // namespace detail

BOOST_TT_AUX_BOOL_TRAIT_DEF1(has_trivial_constructor,T,::lslboost::detail::has_trivial_ctor_impl<T>::value)
BOOST_TT_AUX_BOOL_TRAIT_DEF1(has_trivial_default_constructor,T,::lslboost::detail::has_trivial_ctor_impl<T>::value)

} // namespace lslboost

#include <lslboost/type_traits/detail/bool_trait_undef.hpp>

#endif // BOOST_TT_HAS_TRIVIAL_CONSTRUCTOR_HPP_INCLUDED
