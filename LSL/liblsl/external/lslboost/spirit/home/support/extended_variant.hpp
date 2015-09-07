/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_EXTENDED_VARIANT_AUGUST_6_2011_0859AM)
#define BOOST_SPIRIT_EXTENDED_VARIANT_AUGUST_6_2011_0859AM

#if defined(_MSC_VER)
#pragma once
#endif

#include <lslboost/variant.hpp>
#include <lslboost/mpl/limits/list.hpp>
#include <lslboost/preprocessor/repetition/enum_params.hpp>
#include <lslboost/preprocessor/repetition/enum_params_with_a_default.hpp>

#if !defined(BOOST_VARIANT_DO_NOT_USE_VARIADIC_TEMPLATES)
#define BOOST_SPIRIT_EXTENDED_VARIANT_LIMIT_TYPES BOOST_MPL_LIMIT_LIST_SIZE
#else
#define BOOST_SPIRIT_EXTENDED_VARIANT_LIMIT_TYPES BOOST_VARIANT_LIMIT_TYPES
#endif

#define BOOST_SPIRIT_EXTENDED_VARIANT_ENUM_PARAMS(T)                          \
    BOOST_PP_ENUM_PARAMS(BOOST_SPIRIT_EXTENDED_VARIANT_LIMIT_TYPES, T)        \
    /**/

///////////////////////////////////////////////////////////////////////////////
namespace lslboost { namespace spirit
{
#if defined(BOOST_VARIANT_DO_NOT_USE_VARIADIC_TEMPLATES)
    template <
        BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT(
            BOOST_SPIRIT_EXTENDED_VARIANT_LIMIT_TYPES,
            typename T, lslboost::detail::variant::void_)
            // We should not be depending on detail::variant::void_
            // but I'm not sure if this can fixed. Any other way is
            // clumsy at best.
        >
#else
    template <typename... Types>
#endif
    struct extended_variant
    {
        // tell spirit that this is an adapted variant
        struct adapted_variant_tag;

#if defined(BOOST_VARIANT_DO_NOT_USE_VARIADIC_TEMPLATES)
        typedef lslboost::variant<
            BOOST_SPIRIT_EXTENDED_VARIANT_ENUM_PARAMS(T)>
        variant_type;
        typedef typename variant_type::types types;

        typedef extended_variant<
            BOOST_SPIRIT_EXTENDED_VARIANT_ENUM_PARAMS(T)
        > base_type;
#else
        typedef lslboost::variant<Types...> variant_type;
        typedef typename variant_type::types types;
        typedef extended_variant<Types...> base_type;
#endif

        extended_variant() : var() {}

        template <typename T>
        extended_variant(T const& var)
            : var(var) {}

        template <typename T>
        extended_variant(T& var)
            : var(var) {}

        template <typename F>
        typename F::result_type apply_visitor(F const& v)
        {
            return var.apply_visitor(v);
        }

        template <typename F>
        typename F::result_type apply_visitor(F const& v) const
        {
            return var.apply_visitor(v);
        }

        template <typename F>
        typename F::result_type apply_visitor(F& v)
        {
            return var.apply_visitor(v);
        }

        template <typename F>
        typename F::result_type apply_visitor(F& v) const
        {
            return var.apply_visitor(v);
        }

        variant_type const& get() const
        {
            return var;
        }

        variant_type& get()
        {
            return var;
        }

        variant_type var;
    };
}}

namespace lslboost
{
#if defined(BOOST_VARIANT_DO_NOT_USE_VARIADIC_TEMPLATES)
    template <typename T, BOOST_SPIRIT_EXTENDED_VARIANT_ENUM_PARAMS(typename T)>
    inline T const&
    get(lslboost::spirit::extended_variant<
        BOOST_SPIRIT_EXTENDED_VARIANT_ENUM_PARAMS(T)> const& x)
    {
        return lslboost::get<T>(x.get());
    }

    template <typename T, BOOST_SPIRIT_EXTENDED_VARIANT_ENUM_PARAMS(typename T)>
    inline T&
    get(lslboost::spirit::extended_variant<
        BOOST_SPIRIT_EXTENDED_VARIANT_ENUM_PARAMS(T)>& x)
    {
        return lslboost::get<T>(x.get());
    }

    template <typename T, BOOST_SPIRIT_EXTENDED_VARIANT_ENUM_PARAMS(typename T)>
    inline T const*
    get(lslboost::spirit::extended_variant<
        BOOST_SPIRIT_EXTENDED_VARIANT_ENUM_PARAMS(T)> const* x)
    {
        return lslboost::get<T>(&x->get());
    }

    template <typename T, BOOST_SPIRIT_EXTENDED_VARIANT_ENUM_PARAMS(typename T)>
    inline T*
    get(lslboost::spirit::extended_variant<
        BOOST_SPIRIT_EXTENDED_VARIANT_ENUM_PARAMS(T)>* x)
    {
        return lslboost::get<T>(&x->get());
    }
#else
    template <typename T, typename... Types>
    inline T const&
    get(lslboost::spirit::extended_variant<Types...> const& x)
    {
        return lslboost::get<T>(x.get());
    }

    template <typename T, typename... Types>
    inline T&
    get(lslboost::spirit::extended_variant<Types...>& x)
    {
        return lslboost::get<T>(x.get());
    }

    template <typename T, typename... Types>
    inline T const*
    get(lslboost::spirit::extended_variant<Types...> const* x)
    {
        return lslboost::get<T>(&x->get());
    }

    template <typename T, typename... Types>
    inline T*
    get(lslboost::spirit::extended_variant<Types...>* x)
    {
        return lslboost::get<T>(&x->get());
    }
#endif
}

#undef BOOST_SPIRIT_EXTENDED_VARIANT_ENUM_PARAMS
#undef BOOST_SPIRIT_EXTENDED_VARIANT_LIMIT_TYPES

#endif
