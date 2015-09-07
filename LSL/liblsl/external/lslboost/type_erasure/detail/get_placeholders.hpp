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

#ifndef BOOST_TYPE_ERASURE_DETAIL_GET_PLACEHOLDERS_HPP_INCLUDED
#define BOOST_TYPE_ERASURE_DETAIL_GET_PLACEHOLDERS_HPP_INCLUDED

#include <lslboost/mpl/eval_if.hpp>
#include <lslboost/mpl/identity.hpp>
#include <lslboost/mpl/insert.hpp>
#include <lslboost/preprocessor/cat.hpp>
#include <lslboost/preprocessor/iteration/iterate.hpp>
#include <lslboost/preprocessor/repetition/enum.hpp>
#include <lslboost/preprocessor/repetition/enum_params.hpp>
#include <lslboost/preprocessor/repetition/enum_trailing_params.hpp>
#include <lslboost/type_erasure/config.hpp>
#include <lslboost/type_erasure/is_placeholder.hpp>

namespace lslboost {
namespace type_erasure {
namespace detail {

template<class T, class Out>
struct get_placeholders_in_argument
{
    typedef typename ::lslboost::mpl::eval_if<
        ::lslboost::type_erasure::is_placeholder<T>,
        ::lslboost::mpl::insert<Out, T>,
        ::lslboost::mpl::identity<Out>
    >::type type;
};

template<class T, class Out>
struct get_placeholders;

template<class T, class Out>
struct get_placeholders_in_argument<T&, Out>
{
    typedef typename ::lslboost::type_erasure::detail::get_placeholders_in_argument<
        T,
        Out
    >::type type;
};

template<class T, class Out>
struct get_placeholders_in_argument<const T, Out>
{
    typedef typename ::lslboost::type_erasure::detail::get_placeholders_in_argument<
        T,
        Out
    >::type type;
};

#if !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES)

template<class Out, class... T>
struct get_placeholders_impl;

template<class Out, class T0, class... T>
struct get_placeholders_impl<Out, T0, T...>
{
    typedef typename ::lslboost::type_erasure::detail::get_placeholders_in_argument<
        T0,
        typename get_placeholders_impl<Out, T...>::type
    >::type type;
};

template<class Out>
struct get_placeholders_impl<Out>
{
    typedef Out type;
};

template<template<class...> class T, class... U, class Out>
struct get_placeholders<T<U...>, Out>
{
    typedef typename get_placeholders_impl<Out, U...>::type type;
};

template<class R, class... T, class Out>
struct get_placeholders_in_argument<R(T...), Out>
{
    typedef typename ::lslboost::type_erasure::detail::get_placeholders_in_argument<
        R,
        Out
    >::type type0;
    typedef typename get_placeholders_impl<type0, T...>::type type;
};

#else

#define BOOST_PP_FILENAME_1 <lslboost/type_erasure/detail/get_placeholders.hpp>
#define BOOST_PP_ITERATION_LIMITS (0, BOOST_TYPE_ERASURE_MAX_ARITY)
#include BOOST_PP_ITERATE()

#endif

}
}
}

#endif

#else

#define N BOOST_PP_ITERATION()
#define BOOST_TYPE_ERASURE_GET_PLACEHOLDER(z, n, data)                              \
    typedef typename ::lslboost::type_erasure::detail::get_placeholders_in_argument<   \
        BOOST_PP_CAT(data, n), BOOST_PP_CAT(type, n)>::type                         \
        BOOST_PP_CAT(type, BOOST_PP_INC(n));

#if N != 0

template<template<BOOST_PP_ENUM_PARAMS(N, class T)> class T,
    BOOST_PP_ENUM_PARAMS(N, class T), class Out>
struct get_placeholders<T<BOOST_PP_ENUM_PARAMS(N, T)>, Out>
{
    typedef Out type0;
    BOOST_PP_REPEAT(N, BOOST_TYPE_ERASURE_GET_PLACEHOLDER, T)
    typedef BOOST_PP_CAT(type, N) type;
};

#endif

template<class R
    BOOST_PP_ENUM_TRAILING_PARAMS(N, class T), class Out>
struct get_placeholders_in_argument<R(BOOST_PP_ENUM_PARAMS(N, T)), Out>
{
    typedef typename ::lslboost::type_erasure::detail::get_placeholders_in_argument<
        R,
        Out
    >::type type0;
    BOOST_PP_REPEAT(N, BOOST_TYPE_ERASURE_GET_PLACEHOLDER, T)
    typedef BOOST_PP_CAT(type, N) type;
};

#undef BOOST_TYPE_ERASURE_GET_PLACEHOLDER
#undef N

#endif
