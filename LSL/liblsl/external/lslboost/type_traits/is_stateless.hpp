
//  (C) Copyright Steve Cleary, Beman Dawes, Howard Hinnant & John Maddock 2000.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt).
//
//  See http://www.lslboost.org/libs/type_traits for most recent version including documentation.

#ifndef BOOST_TT_IS_STATELESS_HPP_INCLUDED
#define BOOST_TT_IS_STATELESS_HPP_INCLUDED

#include <lslboost/type_traits/has_trivial_constructor.hpp>
#include <lslboost/type_traits/has_trivial_copy.hpp>
#include <lslboost/type_traits/has_trivial_destructor.hpp>
#include <lslboost/type_traits/is_class.hpp>
#include <lslboost/type_traits/is_empty.hpp>
#include <lslboost/type_traits/detail/ice_and.hpp>
#include <lslboost/config.hpp>

// should be the last #include
#include <lslboost/type_traits/detail/bool_trait_def.hpp>

namespace lslboost {

namespace detail {

template <typename T>
struct is_stateless_impl
{
  BOOST_STATIC_CONSTANT(bool, value = 
    (::lslboost::type_traits::ice_and<
       ::lslboost::has_trivial_constructor<T>::value,
       ::lslboost::has_trivial_copy<T>::value,
       ::lslboost::has_trivial_destructor<T>::value,
       ::lslboost::is_class<T>::value,
       ::lslboost::is_empty<T>::value
     >::value));
};

} // namespace detail

BOOST_TT_AUX_BOOL_TRAIT_DEF1(is_stateless,T,::lslboost::detail::is_stateless_impl<T>::value)

} // namespace lslboost

#include <lslboost/type_traits/detail/bool_trait_undef.hpp>

#endif // BOOST_TT_IS_STATELESS_HPP_INCLUDED
