// Boost.TypeErasure library
//
// Copyright 2011 Steven Watanabe
//
// Distributed under the Boost Software License Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)
//
// $Id$

#ifndef BOOST_TYPE_ERASURE_TYPEID_OF_HPP_INCLUDED
#define BOOST_TYPE_ERASURE_TYPEID_OF_HPP_INCLUDED

#include <lslboost/type_traits/remove_cv.hpp>
#include <lslboost/type_traits/remove_reference.hpp>
#include <lslboost/type_erasure/detail/access.hpp>
#include <lslboost/type_erasure/any.hpp>
#include <lslboost/type_erasure/binding.hpp>

namespace lslboost {
namespace type_erasure {

/**
 * The first form returns the type currently stored in an @ref any.
 *
 * The second form returns the type corresponding to a
 * placeholder in @c binding.
 *
 * \pre @c Concept includes @ref typeid_ "typeid_<T>".
 * \pre @c T is a non-reference, CV-unqualified @ref placeholder.
 */
template<class Concept, class T>
const std::type_info& typeid_of(const any<Concept, T>& arg)
{
    return ::lslboost::type_erasure::detail::access::table(arg).template find<
        ::lslboost::type_erasure::typeid_<
            typename ::lslboost::remove_cv<
                typename ::lslboost::remove_reference<T>::type
            >::type
        >
    >()();
}

#ifndef BOOST_TYPE_ERASURE_DOXYGEN
template<class Concept, class T>
const std::type_info& typeid_of(const param<Concept, T>& arg)
{
    return ::lslboost::type_erasure::detail::access::table(arg).template find<
        ::lslboost::type_erasure::typeid_<
            typename ::lslboost::remove_cv<
                typename ::lslboost::remove_reference<T>::type
            >::type
        >
    >()();
}
#endif

/**
 * \overload
 */
template<class T, class Concept>
const std::type_info& typeid_of(const binding<Concept>& binding_arg)
{
    return binding_arg.template find< ::lslboost::type_erasure::typeid_<T> >()();
}

}
}

#endif
