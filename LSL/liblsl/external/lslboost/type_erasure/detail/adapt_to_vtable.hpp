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

#ifndef BOOST_TYPE_ERASURE_DETAIL_ADAPT_TO_VTABLE_HPP_INCLUDED
#define BOOST_TYPE_ERASURE_DETAIL_ADAPT_TO_VTABLE_HPP_INCLUDED

#include <lslboost/detail/workaround.hpp>
#include <lslboost/utility/addressof.hpp>
#include <lslboost/mpl/if.hpp>
#include <lslboost/mpl/eval_if.hpp>
#include <lslboost/mpl/has_xxx.hpp>
#include <lslboost/type_traits/function_traits.hpp>
#include <lslboost/type_traits/remove_cv.hpp>
#include <lslboost/preprocessor/cat.hpp>
#include <lslboost/preprocessor/iteration/iterate.hpp>
#include <lslboost/preprocessor/repetition/enum.hpp>
#include <lslboost/preprocessor/repetition/enum_params.hpp>
#include <lslboost/preprocessor/repetition/enum_trailing_params.hpp>
#include <lslboost/preprocessor/repetition/enum_binary_params.hpp>
#include <lslboost/type_erasure/detail/get_signature.hpp>
#include <lslboost/type_erasure/detail/storage.hpp>
#include <lslboost/type_erasure/is_placeholder.hpp>
#include <lslboost/type_erasure/config.hpp>

namespace lslboost {
namespace type_erasure {

namespace detail {

template<class T, class Bindings>
struct rebind_placeholders;

template<class T, class Bindings>
struct rebind_placeholders_in_argument;

template<class PrimitiveConcept, class Sig>
struct vtable_adapter;

template<class PrimitiveConcept, class Sig, class Bindings>
struct rebind_placeholders<vtable_adapter<PrimitiveConcept, Sig>, Bindings>
{
    typedef vtable_adapter<
        typename rebind_placeholders<PrimitiveConcept, Bindings>::type,
        typename rebind_placeholders_in_argument<Sig, Bindings>::type
    > type;
};

template<class T>
struct replace_param_for_vtable
{
    typedef typename ::lslboost::mpl::if_<
        ::lslboost::type_erasure::is_placeholder<typename ::lslboost::remove_cv<T>::type>,
        const ::lslboost::type_erasure::detail::storage&,
        T
    >::type type;
};

template<class T>
struct replace_param_for_vtable<T&>
{
    typedef typename ::lslboost::mpl::if_<
        ::lslboost::type_erasure::is_placeholder<typename ::lslboost::remove_cv<T>::type>,
        ::lslboost::type_erasure::detail::storage&,
        T&
    >::type type;
};

template<class T>
struct replace_param_for_vtable<const T&>
{
    typedef typename ::lslboost::mpl::if_<
        ::lslboost::type_erasure::is_placeholder<typename ::lslboost::remove_cv<T>::type>,
        const ::lslboost::type_erasure::detail::storage&,
        const T&
    >::type type;
};

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES

template<class T>
struct replace_param_for_vtable<T&&>
{
    typedef typename ::lslboost::mpl::if_<
        ::lslboost::type_erasure::is_placeholder<typename ::lslboost::remove_cv<T>::type>,
        ::lslboost::type_erasure::detail::storage&&,
        T&&
    >::type type;
};

#endif

template<class T>
struct replace_result_for_vtable
{
    typedef typename ::lslboost::mpl::if_<
        ::lslboost::type_erasure::is_placeholder<typename ::lslboost::remove_cv<T>::type>,
        ::lslboost::type_erasure::detail::storage,
        T
    >::type type;
};

template<class T>
struct replace_result_for_vtable<T&>
{
    typedef typename ::lslboost::mpl::if_<
        ::lslboost::type_erasure::is_placeholder<typename ::lslboost::remove_cv<T>::type>,
        ::lslboost::type_erasure::detail::storage&,
        T&
    >::type type;
};

template<class T>
struct replace_result_for_vtable<const T&>
{
    typedef typename ::lslboost::mpl::if_<
        ::lslboost::type_erasure::is_placeholder<typename ::lslboost::remove_cv<T>::type>,
        ::lslboost::type_erasure::detail::storage&,
        const T&
    >::type type;
};

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES

template<class T>
struct replace_result_for_vtable<T&&>
{
    typedef typename ::lslboost::mpl::if_<
        ::lslboost::type_erasure::is_placeholder<typename ::lslboost::remove_cv<T>::type>,
        ::lslboost::type_erasure::detail::storage&&,
        T&&
    >::type type;
};

#endif

template<class Sig>
struct get_vtable_signature;

BOOST_MPL_HAS_XXX_TRAIT_DEF(type)

template<class T>
struct is_internal_concept :
    ::lslboost::type_erasure::detail::has_type<T>
{};

template<class PrimitiveConcept>
struct adapt_to_vtable
{
    typedef ::lslboost::type_erasure::detail::vtable_adapter<
        PrimitiveConcept,
        typename ::lslboost::type_erasure::detail::get_vtable_signature<
            typename ::lslboost::type_erasure::detail::get_signature<
                PrimitiveConcept
            >::type
        >::type
    > type;
};

template<class Concept>
struct maybe_adapt_to_vtable
{
    typedef typename ::lslboost::mpl::eval_if<
        ::lslboost::type_erasure::detail::is_internal_concept<Concept>,
        ::lslboost::mpl::identity<Concept>,
        ::lslboost::type_erasure::detail::adapt_to_vtable<Concept>
    >::type type;
};

#if !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES) && \
    !defined(BOOST_NO_CXX11_RVALUE_REFERENCES) && \
    !BOOST_WORKAROUND(BOOST_MSVC, == 1800)

template<class PrimitiveConcept, class Sig, class ConceptSig>
struct vtable_adapter_impl;

template<class PrimitiveConcept, class R, class... T, class R2, class... U>
struct vtable_adapter_impl<PrimitiveConcept, R(T...), R2(U...)>
{
    typedef R (*type)(T...);
    static R value(T... arg)
    {
        return PrimitiveConcept::apply(
            ::lslboost::type_erasure::detail::extract<U>(std::forward<T>(arg))...);
    }
};

template<class PrimitiveConcept, class... T, class R2, class... U>
struct vtable_adapter_impl<PrimitiveConcept, ::lslboost::type_erasure::detail::storage&(T...), R2(U...)>
{
    typedef ::lslboost::type_erasure::detail::storage (*type)(T...);
    static ::lslboost::type_erasure::detail::storage value(T... arg)
    {
        ::lslboost::type_erasure::detail::storage result;
        typename ::lslboost::remove_reference<R2>::type* p =
            ::lslboost::addressof(
                PrimitiveConcept::apply(::lslboost::type_erasure::detail::extract<U>(std::forward<T>(arg))...));
        result.data = const_cast<void*>(static_cast<const void*>(p));
        return result;
    }
};

template<class PrimitiveConcept, class... T, class R2, class... U>
struct vtable_adapter_impl<PrimitiveConcept, ::lslboost::type_erasure::detail::storage&&(T...), R2(U...)>
{
    typedef ::lslboost::type_erasure::detail::storage (*type)(T...);
    static ::lslboost::type_erasure::detail::storage value(T... arg)
    {
        ::lslboost::type_erasure::detail::storage result;
        R2 tmp = PrimitiveConcept::apply(::lslboost::type_erasure::detail::extract<U>(std::forward<T>(arg))...);
        typename ::lslboost::remove_reference<R2>::type* p = ::lslboost::addressof(tmp);
        result.data = const_cast<void*>(static_cast<const void*>(p));
        return result;
    }
};

template<class PrimitiveConcept, class Sig>
struct vtable_adapter
    : vtable_adapter_impl<
        PrimitiveConcept,
        Sig,
        typename ::lslboost::type_erasure::detail::get_signature<
            PrimitiveConcept
        >::type
    >
{};

template<class R, class... T>
struct get_vtable_signature<R(T...)>
{
    typedef typename ::lslboost::type_erasure::detail::replace_result_for_vtable<
        R
    >::type type(typename ::lslboost::type_erasure::detail::replace_param_for_vtable<T>::type...);
};

#else

#define BOOST_PP_FILENAME_1 <lslboost/type_erasure/detail/adapt_to_vtable.hpp>
#define BOOST_PP_ITERATION_LIMITS (0, BOOST_TYPE_ERASURE_MAX_ARITY)
#include BOOST_PP_ITERATE()

#endif

}
}
}

#endif

#else

#define N BOOST_PP_ITERATION()

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES

#define BOOST_TYPE_ERASURE_EXTRACT(z, n, data)                  \
    ::lslboost::type_erasure::detail::extract<                     \
        typename traits::                                       \
        BOOST_PP_CAT(BOOST_PP_CAT(arg, BOOST_PP_INC(n)), _type) \
    >(std::forward<BOOST_PP_CAT(T, n)>(BOOST_PP_CAT(arg, n)))

#else

#define BOOST_TYPE_ERASURE_EXTRACT(z, n, data)                  \
    ::lslboost::type_erasure::detail::extract<                     \
        typename traits::                                       \
        BOOST_PP_CAT(BOOST_PP_CAT(arg, BOOST_PP_INC(n)), _type) \
    >(BOOST_PP_CAT(arg, n))

#endif

#define BOOST_TYPE_ERASURE_REPLACE_PARAM(z, n, data)                    \
    typename ::lslboost::type_erasure::detail::replace_param_for_vtable<   \
        BOOST_PP_CAT(T, n)>::type

template<class PrimitiveConcept, class R
    BOOST_PP_ENUM_TRAILING_PARAMS(N, class T)>
struct vtable_adapter<PrimitiveConcept, R(BOOST_PP_ENUM_PARAMS(N, T))>
{
    typedef R (*type)(BOOST_PP_ENUM_PARAMS(N, T));
    static R value(BOOST_PP_ENUM_BINARY_PARAMS(N, T, arg))
    {
        typedef typename ::lslboost::function_traits<
            typename ::lslboost::type_erasure::detail::get_signature<
                PrimitiveConcept
            >::type
        > traits;
        return PrimitiveConcept::apply(
            BOOST_PP_ENUM(N, BOOST_TYPE_ERASURE_EXTRACT, ~));
    }
};

template<class PrimitiveConcept
    BOOST_PP_ENUM_TRAILING_PARAMS(N, class T)>
struct vtable_adapter<PrimitiveConcept, ::lslboost::type_erasure::detail::storage&(BOOST_PP_ENUM_PARAMS(N, T))>
{
    typedef ::lslboost::type_erasure::detail::storage (*type)(BOOST_PP_ENUM_PARAMS(N, T));
    static ::lslboost::type_erasure::detail::storage value(BOOST_PP_ENUM_BINARY_PARAMS(N, T, arg))
    {
        typedef typename ::lslboost::function_traits<
            typename ::lslboost::type_erasure::detail::get_signature<
                PrimitiveConcept
            >::type
        > traits;
        ::lslboost::type_erasure::detail::storage result;
        typename ::lslboost::remove_reference<typename traits::result_type>::type* p =
            ::lslboost::addressof(
                PrimitiveConcept::apply(BOOST_PP_ENUM(N, BOOST_TYPE_ERASURE_EXTRACT, ~)));
        result.data = const_cast<void*>(static_cast<const void*>(p));
        return result;
    }
};

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES

template<class PrimitiveConcept
    BOOST_PP_ENUM_TRAILING_PARAMS(N, class T)>
struct vtable_adapter<PrimitiveConcept, ::lslboost::type_erasure::detail::storage&&(BOOST_PP_ENUM_PARAMS(N, T))>
{
    typedef ::lslboost::type_erasure::detail::storage (*type)(BOOST_PP_ENUM_PARAMS(N, T));
    static ::lslboost::type_erasure::detail::storage value(BOOST_PP_ENUM_BINARY_PARAMS(N, T, arg))
    {
        typedef typename ::lslboost::function_traits<
            typename ::lslboost::type_erasure::detail::get_signature<
                PrimitiveConcept
            >::type
        > traits;
        ::lslboost::type_erasure::detail::storage result;
        typename traits::result_type tmp =
            PrimitiveConcept::apply(BOOST_PP_ENUM(N, BOOST_TYPE_ERASURE_EXTRACT, ~));
        typename ::lslboost::remove_reference<typename traits::result_type>::type* p =
            ::lslboost::addressof(tmp);
        result.data = const_cast<void*>(static_cast<const void*>(p));
        return result;
    }
};

#endif

template<class R BOOST_PP_ENUM_TRAILING_PARAMS(N, class T)>
struct get_vtable_signature<R(BOOST_PP_ENUM_PARAMS(N, T))>
{
    typedef typename ::lslboost::type_erasure::detail::replace_result_for_vtable<
        R
    >::type type(BOOST_PP_ENUM(N, BOOST_TYPE_ERASURE_REPLACE_PARAM, ~));
};

#undef BOOST_TYPE_ERASURE_REPLACE_PARAM
#undef BOOST_TYPE_ERASURE_EXTRACT
#undef N

#endif
