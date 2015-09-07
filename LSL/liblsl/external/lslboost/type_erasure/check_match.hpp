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

#ifndef BOOST_TYPE_ERASURE_CHECK_MATCH_HPP_INCLUDED
#define BOOST_TYPE_ERASURE_CHECK_MATCH_HPP_INCLUDED

#include <lslboost/mpl/vector.hpp>
#include <lslboost/mpl/bool.hpp>
#include <lslboost/mpl/is_sequence.hpp>
#include <lslboost/mpl/find_if.hpp>
#include <lslboost/mpl/not.hpp>
#include <lslboost/mpl/end.hpp>
#include <lslboost/type_traits/is_same.hpp>
#include <lslboost/preprocessor/cat.hpp>
#include <lslboost/preprocessor/iteration/iterate.hpp>
#include <lslboost/preprocessor/repetition/repeat.hpp>
#include <lslboost/preprocessor/repetition/enum_params.hpp>
#include <lslboost/preprocessor/repetition/enum_trailing_params.hpp>
#include <lslboost/preprocessor/repetition/enum_trailing_binary_params.hpp>
#include <lslboost/type_erasure/detail/extract_concept.hpp>
#include <lslboost/type_erasure/relaxed.hpp>
#include <lslboost/type_erasure/detail/access.hpp>

namespace lslboost {
namespace type_erasure {

template<class T>
struct typeid_;

template<class Concept>
class binding;

namespace detail {

template<class P, class T, class Table>
bool maybe_check_table(const T& arg, const Table*& table, lslboost::mpl::true_)
{
    if(table == 0) {
        table = &::lslboost::type_erasure::detail::access::table(arg);
        return true;
    } else {
        return table->template find< ::lslboost::type_erasure::typeid_<P> >()() ==
            ::lslboost::type_erasure::detail::access::table(arg).
                template find< ::lslboost::type_erasure::typeid_<P> >()();
    }
}

template<class P, class T, class Table>
bool maybe_check_table(const T&, const Table*&, lslboost::mpl::false_)
{
    return true;
}

template<class Concept, class T>
struct should_check :
    lslboost::mpl::and_<
        ::lslboost::type_erasure::is_placeholder<T>,
        ::lslboost::type_erasure::is_relaxed<Concept>
    >
{};

}

#ifdef BOOST_TYPE_ERASURE_DOXYGEN

/**
 * If @ref relaxed is in @c Concept, checks whether the
 * arguments to @c f match the types specified by
 * @c binding.  If @ref relaxed is not in @c Concept,
 * returns true.  If @c binding is not specified, it will
 * be deduced from the arguments.
 */
template<class Concept, class Op, class... U>
bool check_match(const binding<Concept>& binding_arg, const Op& f, U&&... args);

/**
 * \overload
 */
template<class Op, class... U>
bool check_match(const Op& f, U&&... args);

#else

#if !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES) && !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)

namespace detail {

template<class Concept>
bool check_table(const ::lslboost::type_erasure::binding<Concept>* t, void(*)())
{
    return true;
}

template<class Concept, class R, class T0, class... T, class U0, class... U>
bool check_table(
    const ::lslboost::type_erasure::binding<Concept>* t,
    R(*)(T0, T...),
    const U0& arg0,
    const U&... arg)
{
    typedef typename ::lslboost::remove_cv<
        typename ::lslboost::remove_reference<T0>::type
    >::type t0;
    if(!::lslboost::type_erasure::detail::maybe_check_table<t0>(
        arg0,
        t,
        ::lslboost::type_erasure::detail::should_check<Concept, t0>()))
        return false;

    return check_table(t, static_cast<void(*)(T...)>(0), arg...);
}

}

template<class Concept, class Op, class... U>
bool check_match(
    const ::lslboost::type_erasure::binding<Concept>& table,
    const Op&,
    U&&... arg)
{

    return ::lslboost::type_erasure::detail::check_table(
        &table,
        static_cast<typename ::lslboost::type_erasure::detail::get_signature<Op>::type*>(0),
        arg...);
}

template<
    class Op,
    class... U
>
bool check_match(
    const Op&,
    U&&... arg)
{
    const ::lslboost::type_erasure::binding<
        typename ::lslboost::type_erasure::detail::extract_concept<
            typename ::lslboost::type_erasure::detail::get_signature<Op>::type, U...>::type>* p = 0;

    return ::lslboost::type_erasure::detail::check_table(
        p, static_cast<typename ::lslboost::type_erasure::detail::get_signature<Op>::type*>(0), arg...);
}

#else

#define BOOST_PP_FILENAME_1 <lslboost/type_erasure/check_match.hpp>
#define BOOST_PP_ITERATION_LIMITS (0, BOOST_TYPE_ERASURE_MAX_ARITY)
#include BOOST_PP_ITERATE()

#endif

#endif

}
}

#endif

#else

namespace detail {

#define N BOOST_PP_ITERATION()

#define BOOST_TYPE_ERASURE_CHECK_TABLE(z, n, data)                          \
    typedef typename ::lslboost::remove_cv<                                    \
        typename ::lslboost::remove_reference<BOOST_PP_CAT(T, n)>::type        \
    >::type BOOST_PP_CAT(t, n);                                             \
    if(!::lslboost::type_erasure::detail::maybe_check_table<BOOST_PP_CAT(t, n)>(   \
        BOOST_PP_CAT(arg, n),                                               \
        t,                                                                  \
        ::lslboost::type_erasure::detail::should_check<Concept, BOOST_PP_CAT(t, n)>())) \
        return false;

template<
    class Concept,
    class R
    BOOST_PP_ENUM_TRAILING_PARAMS(N, class T)
    BOOST_PP_ENUM_TRAILING_PARAMS(N, class U)>
bool
BOOST_PP_CAT(check_table, N)(
    const ::lslboost::type_erasure::binding<Concept>* t,
    R (*)(BOOST_PP_ENUM_PARAMS(N, T))
    BOOST_PP_ENUM_TRAILING_BINARY_PARAMS(N, const U, &arg))
{
    BOOST_PP_REPEAT(N, BOOST_TYPE_ERASURE_CHECK_TABLE, ~)
    return true;
}

#if N != 0

template<class Sig BOOST_PP_ENUM_TRAILING_PARAMS(N, class U)>
struct BOOST_PP_CAT(do_extract_concept, N);

template<
    class R
    BOOST_PP_ENUM_TRAILING_PARAMS(N, class T)
    BOOST_PP_ENUM_TRAILING_PARAMS(N, class U)
>
struct BOOST_PP_CAT(do_extract_concept, N)<
    R(BOOST_PP_ENUM_PARAMS(N, T))
    BOOST_PP_ENUM_TRAILING_PARAMS(N, U)
>
  : ::lslboost::type_erasure::detail::BOOST_PP_CAT(extract_concept, N)<
        BOOST_PP_ENUM_PARAMS(N, T)
        BOOST_PP_ENUM_TRAILING_PARAMS(N, U)>
{};

#endif

}

#ifdef BOOST_NO_CXX11_RVALUE_REFERENCES
#define RREF &
#else
#define RREF &&
#endif

template<
    class Concept,
    class Op
    BOOST_PP_ENUM_TRAILING_PARAMS(N, class U)
>
bool check_match(
    const ::lslboost::type_erasure::binding<Concept>& table,
    const Op&
    BOOST_PP_ENUM_TRAILING_BINARY_PARAMS(N, U, RREF arg))
{

    return ::lslboost::type_erasure::detail::BOOST_PP_CAT(check_table, N)(
        &table,
        (typename ::lslboost::type_erasure::detail::get_signature<Op>::type*)0
        BOOST_PP_ENUM_TRAILING_PARAMS(N, arg));
}

#if N != 0

template<
    class Op
    BOOST_PP_ENUM_TRAILING_PARAMS(N, class U)
>
bool check_match(
    const Op&
    BOOST_PP_ENUM_TRAILING_BINARY_PARAMS(N, U, RREF arg))
{
    const ::lslboost::type_erasure::binding<
        typename ::lslboost::type_erasure::detail::BOOST_PP_CAT(do_extract_concept, N)<
            typename ::lslboost::type_erasure::detail::get_signature<Op>::type,
            BOOST_PP_ENUM_PARAMS(N, U)>::type>* p = 0;

    return ::lslboost::type_erasure::detail::BOOST_PP_CAT(check_table, N)(
        p,
        (typename ::lslboost::type_erasure::detail::get_signature<Op>::type*)0
        BOOST_PP_ENUM_TRAILING_PARAMS(N, arg));
}

#undef RREF
#undef BOOST_TYPE_ERASURE_CHECK_TABLE
#undef N

#endif

#endif
