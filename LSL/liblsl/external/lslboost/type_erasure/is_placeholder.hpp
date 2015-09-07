// Boost.TypeErasure library
//
// Copyright 2011 Steven Watanabe
//
// Distributed under the Boost Software License Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)
//
// $Id$

#ifndef BOOST_TYPE_ERASURE_DETAIL_IS_PLACEHOLDER_HPP_INCLUDED
#define BOOST_TYPE_ERASURE_DETAIL_IS_PLACEHOLDER_HPP_INCLUDED

#include <lslboost/mpl/bool.hpp>
#include <lslboost/type_traits/is_base_and_derived.hpp>
#include <lslboost/type_erasure/placeholder.hpp>

namespace lslboost {

/** INTERNAL ONLY */
struct use_default;

namespace type_erasure {

/** A metafunction that indicates whether a type is a @ref placeholder. */
template<class T>
struct is_placeholder : ::lslboost::is_base_and_derived<placeholder, T> {};

/** INTERNAL ONLY */
template<>
struct is_placeholder< ::lslboost::use_default> : ::lslboost::mpl::false_ {};

}
}

#endif
