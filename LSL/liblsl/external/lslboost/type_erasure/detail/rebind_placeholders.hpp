// Boost.TypeErasure library
//
// Copyright 2011 Steven Watanabe
//
// Distributed under the Boost Software License Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)
//
// $Id$

#if !defined(BOOST_PP_IS_ITERATING)

#ifndef BOOST_TYPE_ERASURE_DETAIL_REBIND_PLACEHOLDERS_HPP_INCLUDED
#define BOOST_TYPE_ERASURE_DETAIL_REBIND_PLACEHOLDERS_HPP_INCLUDED

#include <lslboost/mpl/eval_if.hpp>
#include <lslboost/mpl/identity.hpp>
#include <lslboost/mpl/at.hpp>
#include <lslboost/mpl/has_key.hpp>
#include <lslboost/mpl/not.hpp>
#include <lslboost/mpl/assert.hpp>
#include <lslboost/preprocessor/cat.hpp>
#include <lslboost/preprocessor/iteration/iterate.hpp>
#include <lslboost/preprocessor/repetition/enum.hpp>
#include <lslboost/preprocessor/repetition/enum_params.hpp>
#include <lslboost/preprocessor/repetition/enum_trailing_params.hpp>
#include <lslboost/type_erasure/config.hpp>
#include <lslboost/type_erasure/is_placeholder.hpp>

namespace lslboost {
namespace type_erasure {

template<class F>
struct deduced;

namespace detail {

template<class T, class Bindings>
struct rebind_placeholders
{
    typedef void type;
};

template<class T>
struct identity
{
    typedef T type;
};

template<class T, class Bindings>
struct rebind_placeholders_in_argument
{
    BOOST_MPL_ASSERT((lslboost::mpl::or_<
        ::lslboost::mpl::not_< ::lslboost::type_erasure::is_placeholder<T> >,
        ::lslboost::mpl::has_key<Bindings, T>
    >));
    typedef typename ::lslboost::mpl::eval_if<
        ::lslboost::type_erasure::is_placeholder<T>,
        ::lslboost::mpl::at<Bindings, T>,
        ::lslboost::type_erasure::detail::identity<T>
    >::type type;
};

template<class T, class Bindings>
struct rebind_placeholders_in_argument<T&, Bindings>
{
    typedef typename ::lslboost::type_erasure::detail::rebind_placeholders_in_argument<
        T,
        Bindings
    >::type& type;
};

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES

template<class T, class Bindings>
struct rebind_placeholders_in_argument<T&&, Bindings>
{
    typedef typename ::lslboost::type_erasure::detail::rebind_placeholders_in_argument<
        T,
        Bindings
    >::type&& type;
};

#endif

template<class T, class Bindings>
struct rebind_placeholders_in_argument<const T, Bindings>
{
    typedef const typename ::lslboost::type_erasure::detail::rebind_placeholders_in_argument<
        T,
        Bindings
    >::type type;
};

template<class F, class Bindings>
struct rebind_placeholders_in_deduced
{
    typedef typename ::lslboost::type_erasure::deduced<
        typename ::lslboost::type_erasure::detail::rebind_placeholders<F, Bindings>::type
    >::type type;
};

template<class F, class Bindings>
struct rebind_placeholders_in_argument<
    ::lslboost::type_erasure::deduced<F>,
    Bindings
> 
{
    typedef typename ::lslboost::mpl::eval_if<
        ::lslboost::mpl::has_key<Bindings, ::lslboost::type_erasure::deduced<F> >,
        ::lslboost::mpl::at<Bindings, ::lslboost::type_erasure::deduced<F> >,
        ::lslboost::type_erasure::detail::rebind_placeholders_in_deduced<
            F,
            Bindings
        >
    >::type type;
};

#if !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES)

template<template<class...> class T, class... U, class Bindings>
struct rebind_placeholders<T<U...>, Bindings>
{
    typedef T<typename rebind_placeholders_in_argument<U, Bindings>::type...> type;
};

template<class R, class... T, class Bindings>
struct rebind_placeholders_in_argument<R(T...), Bindings>
{
    typedef typename ::lslboost::type_erasure::detail::rebind_placeholders_in_argument<
        R,
        Bindings
    >::type type(typename rebind_placeholders_in_argument<T, Bindings>::type...);
};

#else

#define BOOST_PP_FILENAME_1 <lslboost/type_erasure/detail/rebind_placeholders.hpp>
#define BOOST_PP_ITERATION_LIMITS (0, BOOST_TYPE_ERASURE_MAX_ARITY)
#include BOOST_PP_ITERATE()

#endif

}
}
}

#endif

#else

#define N BOOST_PP_ITERATION()
#define BOOST_TYPE_ERASURE_REBIND(z, n, data)                           \
    typename rebind_placeholders_in_argument<BOOST_PP_CAT(data, n), Bindings>::type

#if N != 0

template<template<BOOST_PP_ENUM_PARAMS(N, class T)> class T,
    BOOST_PP_ENUM_PARAMS(N, class T), class Bindings>
struct rebind_placeholders<T<BOOST_PP_ENUM_PARAMS(N, T)>, Bindings>
{
    typedef T<BOOST_PP_ENUM(N, BOOST_TYPE_ERASURE_REBIND, T)> type;
};

#endif

template<class R
    BOOST_PP_ENUM_TRAILING_PARAMS(N, class T), class Bindings>
struct rebind_placeholders_in_argument<R(BOOST_PP_ENUM_PARAMS(N, T)), Bindings>
{
    typedef typename ::lslboost::type_erasure::detail::rebind_placeholders_in_argument<
        R,
        Bindings
    >::type type(BOOST_PP_ENUM(N, BOOST_TYPE_ERASURE_REBIND, T));
};

#undef BOOST_TYPE_ERASURE_REBIND
#undef N

#endif
