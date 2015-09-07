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

#ifndef BOOST_TYPE_ERASURE_CALL_HPP_INCLUDED
#define BOOST_TYPE_ERASURE_CALL_HPP_INCLUDED

#include <lslboost/assert.hpp>
#include <lslboost/mpl/bool.hpp>
#include <lslboost/mpl/eval_if.hpp>
#include <lslboost/mpl/identity.hpp>
#include <lslboost/type_traits/remove_cv.hpp>
#include <lslboost/type_traits/remove_reference.hpp>
#include <lslboost/preprocessor/cat.hpp>
#include <lslboost/preprocessor/inc.hpp>
#include <lslboost/preprocessor/iteration/iterate.hpp>
#include <lslboost/preprocessor/repetition/repeat.hpp>
#include <lslboost/preprocessor/repetition/enum.hpp>
#include <lslboost/preprocessor/repetition/enum_trailing.hpp>
#include <lslboost/preprocessor/repetition/enum_params.hpp>
#include <lslboost/preprocessor/repetition/enum_binary_params.hpp>
#include <lslboost/preprocessor/repetition/enum_trailing_params.hpp>
#include <lslboost/preprocessor/repetition/enum_trailing_binary_params.hpp>
#include <lslboost/type_erasure/detail/access.hpp>
#include <lslboost/type_erasure/detail/adapt_to_vtable.hpp>
#include <lslboost/type_erasure/detail/extract_concept.hpp>
#include <lslboost/type_erasure/detail/get_signature.hpp>
#include <lslboost/type_erasure/detail/check_call.hpp>
#include <lslboost/type_erasure/is_placeholder.hpp>
#include <lslboost/type_erasure/concept_of.hpp>
#include <lslboost/type_erasure/config.hpp>
#include <lslboost/type_erasure/require_match.hpp>

namespace lslboost {
namespace type_erasure {

template<class Concept, class Placeholder>
class any;

template<class Concept>
class binding;

namespace detail {

template<class T>
struct is_placeholder_arg :
    ::lslboost::type_erasure::is_placeholder<
        typename ::lslboost::remove_cv<
            typename ::lslboost::remove_reference<T>::type
        >::type
    >
{};

template<class T, class Table>
int maybe_get_table(const T& arg, const Table*& table, lslboost::mpl::true_)
{
    if(table == 0) {
        table = &::lslboost::type_erasure::detail::access::table(arg);
    }
    return 0;
}

template<class T, class Table>
int maybe_get_table(const T&, const Table*&, lslboost::mpl::false_) { return 0; }

template<class T>
::lslboost::type_erasure::detail::storage& convert_arg(any_base<T>& arg, lslboost::mpl::true_)
{
    return ::lslboost::type_erasure::detail::access::data(arg);
}

template<class Concept, class T>
const ::lslboost::type_erasure::detail::storage&
convert_arg(any_base<any<Concept, const T&> >& arg, lslboost::mpl::true_)
{
    return ::lslboost::type_erasure::detail::access::data(arg);
}

template<class T>
const ::lslboost::type_erasure::detail::storage&
convert_arg(const any_base<T>& arg, lslboost::mpl::true_)
{
    return ::lslboost::type_erasure::detail::access::data(arg);
}

template<class Concept, class T>
::lslboost::type_erasure::detail::storage&
convert_arg(const any_base<any<Concept, T&> >& arg, lslboost::mpl::true_)
{
    return ::lslboost::type_erasure::detail::access::data(arg);
}

template<class Concept, class T>
const ::lslboost::type_erasure::detail::storage&
convert_arg(const any_base<any<Concept, const T&> >& arg, lslboost::mpl::true_)
{
    return ::lslboost::type_erasure::detail::access::data(arg);
}

template<class Concept, class T>
::lslboost::type_erasure::detail::storage&
convert_arg(param<Concept, T>& arg, lslboost::mpl::true_)
{
    return ::lslboost::type_erasure::detail::access::data(arg);
}

template<class Concept, class T>
const ::lslboost::type_erasure::detail::storage&
convert_arg(param<Concept, const T&>& arg, lslboost::mpl::true_)
{
    return ::lslboost::type_erasure::detail::access::data(arg);
}

template<class Concept, class T>
const ::lslboost::type_erasure::detail::storage&
convert_arg(const param<Concept, T>& arg, lslboost::mpl::true_)
{
    return ::lslboost::type_erasure::detail::access::data(arg);
}

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES

template<class Concept, class T>
const ::lslboost::type_erasure::detail::storage&
convert_arg(any_base<any<Concept, const T&> >&& arg, lslboost::mpl::true_)
{
    return ::lslboost::type_erasure::detail::access::data(arg);
}

template<class Concept, class T>
::lslboost::type_erasure::detail::storage&
convert_arg(any_base<any<Concept, T&> >&& arg, lslboost::mpl::true_)
{
    return ::lslboost::type_erasure::detail::access::data(arg);
}

template<class Concept, class T>
::lslboost::type_erasure::detail::storage&&
convert_arg(any_base<any<Concept, T> >&& arg, lslboost::mpl::true_)
{
    return ::lslboost::type_erasure::detail::access::data(std::move(arg));
}

template<class Concept, class T>
::lslboost::type_erasure::detail::storage&&
convert_arg(any_base<any<Concept, T&&> >& arg, lslboost::mpl::true_)
{
    return ::lslboost::type_erasure::detail::access::data(arg);
}

template<class Concept, class T>
::lslboost::type_erasure::detail::storage&&
convert_arg(const any_base<any<Concept, T&&> >& arg, lslboost::mpl::true_)
{
    return ::lslboost::type_erasure::detail::access::data(arg);
}

template<class Concept, class T>
const ::lslboost::type_erasure::detail::storage&
convert_arg(param<Concept, const T&>&& arg, lslboost::mpl::true_)
{
    return ::lslboost::type_erasure::detail::access::data(arg);
}

template<class Concept, class T>
::lslboost::type_erasure::detail::storage&
convert_arg(param<Concept, T&>&& arg, lslboost::mpl::true_)
{
    return ::lslboost::type_erasure::detail::access::data(arg);
}

template<class Concept, class T>
::lslboost::type_erasure::detail::storage&&
convert_arg(param<Concept, T>&& arg, lslboost::mpl::true_)
{
    return ::lslboost::type_erasure::detail::access::data(std::move(arg));
}

template<class Concept, class T>
::lslboost::type_erasure::detail::storage&&
convert_arg(param<Concept, T&&>& arg, lslboost::mpl::true_)
{
    return ::lslboost::type_erasure::detail::access::data(arg);
}

template<class Concept, class T>
::lslboost::type_erasure::detail::storage&&
convert_arg(const param<Concept, T&&>& arg, lslboost::mpl::true_)
{
    return ::lslboost::type_erasure::detail::access::data(arg);
}

template<class T>
T&& convert_arg(T&& arg, lslboost::mpl::false_) { return std::forward<T>(arg); }

#else

template<class T>
T& convert_arg(T& arg, lslboost::mpl::false_) { return arg; }

#endif

}

#ifdef BOOST_TYPE_ERASURE_DOXYGEN

/**
 * Dispatches a type erased function.
 *
 * @c Op must be a primitive concept which is present in
 * @c Concept.  Its signature determines how the arguments of
 * \call are handled.  If the argument is a @ref placeholder,
 * \call expects an @ref any using that @ref placeholder.
 * This @ref any is unwrapped by \call.  The type that
 * it stores must be the same type specified by @c binding.
 * Any arguments that are not placeholders in the signature
 * of @c Op are passed through unchanged.
 *
 * If @c binding is not specified, it will be deduced from
 * the arguments.  Naturally this requires at least one
 * argument to be an @ref any.  In this case, all @ref any
 * arguments must have the same @ref binding.
 *
 * \return The result of the operation.  If the result type
 *         of the signature of @c Op is a placeholder, the
 *         result will be converted to the appropriate @ref
 *         any type.
 *
 * \throws bad_function_call if @ref relaxed is
 *         in @c Concept and there is a type mismatch.
 *
 * Example:
 *
 * @code
 * typedef mpl::vector<
 *   copy_constructible<_b>,
 *   addable<_a, int, _b> > concept;
 * any<concept, _a> a = ...;
 * any<concept, _b> b(call(addable<_a, int, _b>(), a, 10));
 * @endcode
 *
 * The signature of @ref addable is <code>_b(const _a&, const int&)</code>
 */
template<class Concept, class Op, class... U>
typename ::lslboost::type_erasure::detail::call_impl<Sig, U..., Concept>::type
call(const binding<Concept>& binding_arg, const Op&, U&&... args);

/**
 * \overload
 */
template<class Op, class... U>
typename ::lslboost::type_erasure::detail::call_impl<Sig, U...>::type
call(const Op&, U&&... args);

#else

namespace detail {
    
template<class Sig, class Args, class Concept = void,
    bool Check = ::lslboost::type_erasure::detail::check_call<Sig, Args>::type::value>
struct call_impl {};

template<class Op, class Args, class Concept = void>
struct call_result :
    call_impl<
        typename ::lslboost::type_erasure::detail::get_signature<Op>::type,
        Args,
        Concept>
{};

template<class C1, class Args, class Concept>
struct call_result<
    ::lslboost::type_erasure::binding<C1>,
    Args,
    Concept
>
{};

}

#if !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES) && !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)

namespace detail {

template<class... T>
void ignore(const T&...) {}

template<class R, class... T, class... U>
const ::lslboost::type_erasure::binding<
    typename ::lslboost::type_erasure::detail::extract_concept<void(T...), U...>::type>*
extract_table(R(*)(T...), const U&... arg)
{
    const ::lslboost::type_erasure::binding<
        typename ::lslboost::type_erasure::detail::extract_concept<
            void(T...), U...>::type>* result = 0;

    // the order that we run maybe_get_table in doesn't matter
    ignore(::lslboost::type_erasure::detail::maybe_get_table(
        arg,
        result,
        ::lslboost::type_erasure::detail::is_placeholder_arg<T>())...);

    BOOST_ASSERT(result != 0);
    return result;
}

template<class Sig, class Args, class Concept, bool ReturnsAny>
struct call_impl_dispatch;

template<class R, class... T, class... U, class Concept>
struct call_impl_dispatch<R(T...), void(U...), Concept, false>
{
    typedef R type;
    template<class F>
    static R apply(const ::lslboost::type_erasure::binding<Concept>* table, U... arg)
    {
        return table->template find<F>()(
            ::lslboost::type_erasure::detail::convert_arg(
                ::std::forward<U>(arg),
                ::lslboost::type_erasure::detail::is_placeholder_arg<T>())...);
    }
};

template<class R, class... T, class... U, class Concept>
struct call_impl_dispatch<R(T...), void(U...), Concept, true>
{
    typedef ::lslboost::type_erasure::any<Concept, R> type;
    template<class F>
    static type apply(const ::lslboost::type_erasure::binding<Concept>* table, U... arg)
    {
        return type(table->template find<F>()(
            ::lslboost::type_erasure::detail::convert_arg(
                ::std::forward<U>(arg),
                ::lslboost::type_erasure::detail::is_placeholder_arg<T>())...), *table);
    }
};

template<class R, class... T, class... U, class Concept>
struct call_impl<R(T...), void(U...), Concept, true> :
    ::lslboost::type_erasure::detail::call_impl_dispatch<
        R(T...),
        void(U...),
        Concept,
        ::lslboost::type_erasure::detail::is_placeholder_arg<R>::value
    >
{
};

template<class R, class... T, class... U>
struct call_impl<R(T...), void(U...), void, true> :
    ::lslboost::type_erasure::detail::call_impl_dispatch<
        R(T...),
        void(U...),
        typename ::lslboost::type_erasure::detail::extract_concept<
            void(T...),
            typename ::lslboost::remove_reference<U>::type...
        >::type,
        ::lslboost::type_erasure::detail::is_placeholder_arg<R>::value
    >
{
};

}

template<
    class Concept,
    class Op,
    class... U
>
typename ::lslboost::type_erasure::detail::call_result<
    Op,
    void(U&&...),
    Concept
>::type
unchecked_call(
    const ::lslboost::type_erasure::binding<Concept>& table,
    const Op&,
    U&&... arg)
{
    return ::lslboost::type_erasure::detail::call_impl<
        typename ::lslboost::type_erasure::detail::get_signature<Op>::type,
        void(U&&...),
        Concept
    >::template apply<
        typename ::lslboost::type_erasure::detail::adapt_to_vtable<Op>::type
    >(&table, std::forward<U>(arg)...);
}

template<class Concept, class Op, class... U>
typename ::lslboost::type_erasure::detail::call_result<
    Op,
    void(U&&...),
    Concept
>::type
call(
    const ::lslboost::type_erasure::binding<Concept>& table,
    const Op& f,
    U&&... arg)
{
    ::lslboost::type_erasure::require_match(table, f, std::forward<U>(arg)...);
    return ::lslboost::type_erasure::unchecked_call(table, f, std::forward<U>(arg)...);
}

template<class Op, class... U>
typename ::lslboost::type_erasure::detail::call_result<
    Op,
    void(U&&...)
>::type
unchecked_call(
    const Op&,
    U&&... arg)
{
    return ::lslboost::type_erasure::detail::call_impl<
        typename ::lslboost::type_erasure::detail::get_signature<Op>::type,
        void(U&&...)
    >::template apply<
        typename ::lslboost::type_erasure::detail::adapt_to_vtable<Op>::type
    >(::lslboost::type_erasure::detail::extract_table(
        static_cast<typename ::lslboost::type_erasure::detail::get_signature<Op>::type*>(0), arg...),
        std::forward<U>(arg)...);
}

template<class Op, class... U>
typename ::lslboost::type_erasure::detail::call_result<
    Op,
    void(U&&...)
>::type
call(
    const Op& f,
    U&&... arg)
{
    ::lslboost::type_erasure::require_match(f, std::forward<U>(arg)...);
    return ::lslboost::type_erasure::unchecked_call(f, std::forward<U>(arg)...);
}


#else

#define BOOST_PP_FILENAME_1 <lslboost/type_erasure/call.hpp>
#define BOOST_PP_ITERATION_LIMITS (0, BOOST_TYPE_ERASURE_MAX_ARITY)
#include BOOST_PP_ITERATE()

#endif

#endif

}
}

#endif

#else

#define N BOOST_PP_ITERATION()

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES

#define BOOST_TYPE_ERASURE_CONVERT_ARG(z, n, data)                      \
    ::lslboost::type_erasure::detail::convert_arg(                         \
        std::forward<BOOST_PP_CAT(U, n)>(BOOST_PP_CAT(arg, n)),         \
        ::lslboost::type_erasure::detail::is_placeholder_arg<BOOST_PP_CAT(T, n)>())

#else

#define BOOST_TYPE_ERASURE_CONVERT_ARG(z, n, data)                      \
    ::lslboost::type_erasure::detail::convert_arg(                         \
        BOOST_PP_CAT(arg, n),                                           \
        ::lslboost::type_erasure::detail::is_placeholder_arg<BOOST_PP_CAT(T, n)>())

#endif

#define BOOST_TYPE_ERASURE_GET_TABLE(z, n, data)                        \
    ::lslboost::type_erasure::detail::maybe_get_table(                     \
        BOOST_PP_CAT(arg, n),                                           \
        result,                                                         \
        ::lslboost::type_erasure::detail::is_placeholder_arg<BOOST_PP_CAT(T, n)>());

namespace detail {

#if N != 0

template<
    class R,
    BOOST_PP_ENUM_PARAMS(N, class T),
    BOOST_PP_ENUM_PARAMS(N, class U)>
const ::lslboost::type_erasure::binding<
    typename ::lslboost::type_erasure::detail::BOOST_PP_CAT(extract_concept, N)<
        BOOST_PP_ENUM_PARAMS(N, T),
        BOOST_PP_ENUM_PARAMS(N, U)>::type>*
BOOST_PP_CAT(extract_table, N)(R(*)(BOOST_PP_ENUM_PARAMS(N, T)), BOOST_PP_ENUM_BINARY_PARAMS(N, const U, &arg))
{
    const ::lslboost::type_erasure::binding<
        typename ::lslboost::type_erasure::detail::BOOST_PP_CAT(extract_concept, N)<
            BOOST_PP_ENUM_PARAMS(N, T),
            BOOST_PP_ENUM_PARAMS(N, U)>::type>* result = 0;
    BOOST_PP_REPEAT(N, BOOST_TYPE_ERASURE_GET_TABLE, ~)
    BOOST_ASSERT(result != 0);
    return result;
}

#endif

template<
    class R
    BOOST_PP_ENUM_TRAILING_PARAMS(N, class T)
    BOOST_PP_ENUM_TRAILING_PARAMS(N, class U),
    class Concept
#if N != 0
        = typename ::lslboost::type_erasure::detail::BOOST_PP_CAT(extract_concept, N)<
            BOOST_PP_ENUM_PARAMS(N, T),
            BOOST_PP_ENUM_PARAMS(N, U)
        >::type
#endif
    ,
    bool ReturnsAny = ::lslboost::type_erasure::detail::is_placeholder_arg<R>::value>
struct BOOST_PP_CAT(call_impl, N);

template<
    class R
    BOOST_PP_ENUM_TRAILING_PARAMS(N, class T)
    BOOST_PP_ENUM_TRAILING_PARAMS(N, class U),
    class Concept
>
struct BOOST_PP_CAT(call_impl, N)<
    R
    BOOST_PP_ENUM_TRAILING_PARAMS(N, T)
    BOOST_PP_ENUM_TRAILING_PARAMS(N, U),
    Concept,
    false
>
{
    typedef R type;
    template<class F>
    static R apply(const ::lslboost::type_erasure::binding<Concept>* table
        BOOST_PP_ENUM_TRAILING_BINARY_PARAMS(N, U, arg))
    {
        return table->template find<F>()(
            BOOST_PP_ENUM(N, BOOST_TYPE_ERASURE_CONVERT_ARG, ~));
    }
};

template<
    class R
    BOOST_PP_ENUM_TRAILING_PARAMS(N, class T)
    BOOST_PP_ENUM_TRAILING_PARAMS(N, class U),
    class Concept
>
struct BOOST_PP_CAT(call_impl, N)<
    R
    BOOST_PP_ENUM_TRAILING_PARAMS(N, T)
    BOOST_PP_ENUM_TRAILING_PARAMS(N, U),
    Concept,
    true
>
{
    typedef ::lslboost::type_erasure::any<Concept, R> type;
    template<class F>
    static type apply(const ::lslboost::type_erasure::binding<Concept>* table
        BOOST_PP_ENUM_TRAILING_BINARY_PARAMS(N, U, arg))
    {
        return type(table->template find<F>()(
            BOOST_PP_ENUM(N, BOOST_TYPE_ERASURE_CONVERT_ARG, ~)), *table);
    }
};

template<
    class R
    BOOST_PP_ENUM_TRAILING_PARAMS(N, class T)
    BOOST_PP_ENUM_TRAILING_PARAMS(N, class U),
    class Concept
>
struct call_impl<R(BOOST_PP_ENUM_PARAMS(N, T)), void(BOOST_PP_ENUM_BINARY_PARAMS(N, U, u)), Concept, true>
  : BOOST_PP_CAT(call_impl, N)<R BOOST_PP_ENUM_TRAILING_PARAMS(N, T) BOOST_PP_ENUM_TRAILING_PARAMS(N, U), Concept>
{};

#if N != 0

template<
    class R
    BOOST_PP_ENUM_TRAILING_PARAMS(N, class T)
    BOOST_PP_ENUM_TRAILING_PARAMS(N, class U)
>
struct call_impl<R(BOOST_PP_ENUM_PARAMS(N, T)), void(BOOST_PP_ENUM_BINARY_PARAMS(N, U, u)), void, true>
  : BOOST_PP_CAT(call_impl, N)<R BOOST_PP_ENUM_TRAILING_PARAMS(N, T) BOOST_PP_ENUM_TRAILING_PARAMS(N, U)>
{};

#endif

}

#ifdef BOOST_NO_CXX11_RVALUE_REFERENCES
#define RREF &
#define BOOST_TYPE_ERASURE_FORWARD_ARGS(N, X, x) BOOST_PP_ENUM_TRAILING_PARAMS(N, x)
#else
#define RREF &&
#define BOOST_TYPE_ERASURE_FORWARD_ARGS_I(z, n, data) std::forward<BOOST_PP_CAT(BOOST_PP_TUPLE_ELEM(2, 0, data), n)>(BOOST_PP_CAT(BOOST_PP_TUPLE_ELEM(2, 1, data), n))
#define BOOST_TYPE_ERASURE_FORWARD_ARGS(N, X, x) BOOST_PP_ENUM_TRAILING(N, BOOST_TYPE_ERASURE_FORWARD_ARGS_I, (X, x))
#endif

template<
    class Concept,
    class Op
    BOOST_PP_ENUM_TRAILING_PARAMS(N, class U)
>
typename ::lslboost::type_erasure::detail::call_result<
    Op,
    void(BOOST_PP_ENUM_BINARY_PARAMS(N, U, RREF u)),
    Concept
>::type
unchecked_call(
    const ::lslboost::type_erasure::binding<Concept>& table,
    const Op&
    BOOST_PP_ENUM_TRAILING_BINARY_PARAMS(N, U, RREF arg))
{
    return ::lslboost::type_erasure::detail::call_impl<
        typename ::lslboost::type_erasure::detail::get_signature<Op>::type,
        void(BOOST_PP_ENUM_BINARY_PARAMS(N, U, RREF u)),
        Concept
    >::template apply<
        typename ::lslboost::type_erasure::detail::adapt_to_vtable<Op>::type
    >(&table BOOST_TYPE_ERASURE_FORWARD_ARGS(N, U, arg));
}

template<
    class Concept,
    class Op
    BOOST_PP_ENUM_TRAILING_PARAMS(N, class U)
>
typename ::lslboost::type_erasure::detail::call_result<
    Op,
    void(BOOST_PP_ENUM_BINARY_PARAMS(N, U, RREF u)),
    Concept
>::type
call(
    const ::lslboost::type_erasure::binding<Concept>& table,
    const Op& f
    BOOST_PP_ENUM_TRAILING_BINARY_PARAMS(N, U, RREF arg))
{
    ::lslboost::type_erasure::require_match(table, f BOOST_TYPE_ERASURE_FORWARD_ARGS(N, U, arg));
    return ::lslboost::type_erasure::unchecked_call(table, f BOOST_TYPE_ERASURE_FORWARD_ARGS(N, U, arg));
}

#if N != 0

template<
    class Op
    BOOST_PP_ENUM_TRAILING_PARAMS(N, class U)
>
typename ::lslboost::type_erasure::detail::call_result<
    Op,
    void(BOOST_PP_ENUM_BINARY_PARAMS(N, U, RREF u))
>::type
unchecked_call(
    const Op&
    BOOST_PP_ENUM_TRAILING_BINARY_PARAMS(N, U, RREF arg))
{
    return ::lslboost::type_erasure::detail::call_impl<
        typename ::lslboost::type_erasure::detail::get_signature<Op>::type,
        void(BOOST_PP_ENUM_BINARY_PARAMS(N, U, RREF u))
    >::template apply<
        typename ::lslboost::type_erasure::detail::adapt_to_vtable<Op>::type
    >(
        ::lslboost::type_erasure::detail::BOOST_PP_CAT(extract_table, N)(
            (typename ::lslboost::type_erasure::detail::get_signature<Op>::type*)0,
            BOOST_PP_ENUM_PARAMS(N, arg))
        BOOST_TYPE_ERASURE_FORWARD_ARGS(N, U, arg)
    );
}

template<
    class Op
    BOOST_PP_ENUM_TRAILING_PARAMS(N, class U)
>
typename ::lslboost::type_erasure::detail::call_result<
    Op,
    void(BOOST_PP_ENUM_BINARY_PARAMS(N, U, RREF u))
>::type
call(
    const Op& f
    BOOST_PP_ENUM_TRAILING_BINARY_PARAMS(N, U, RREF arg))
{
    ::lslboost::type_erasure::require_match(f BOOST_TYPE_ERASURE_FORWARD_ARGS(N, U, arg));
    return ::lslboost::type_erasure::unchecked_call(f BOOST_TYPE_ERASURE_FORWARD_ARGS(N, U, arg));
}

#endif

#undef RREF
#undef BOOST_TYPE_ERASURE_FORWARD_ARGS
#undef BOOST_TYPE_ERASURE_FORWARD_ARGS_I

#undef BOOST_TYPE_ERASURE_GET_TABLE
#undef BOOST_TYPE_ERASURE_CONVERT_ARG
#undef N

#endif
