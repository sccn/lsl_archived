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
#include <lslboost/preprocessor/repetition/enum_params_with_a_default.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace lslboost { namespace spirit
{
    template <
        BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT(
            BOOST_VARIANT_LIMIT_TYPES,
            typename T, lslboost::detail::variant::void_)
            // We should not be depending on detail::variant::void_
            // but I'm not sure if this can fixed. Any other way is
            // clumsy at best.
        >
    struct extended_variant
    {
        // tell spirit that this is an adapted variant
        struct adapted_variant_tag;

        typedef lslboost::variant<
            BOOST_VARIANT_ENUM_PARAMS(T)>
        variant_type;
        typedef typename variant_type::types types;

        typedef extended_variant<BOOST_VARIANT_ENUM_PARAMS(T)> base_type;

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
    template <typename T, BOOST_VARIANT_ENUM_PARAMS(typename T)>
    inline T const&
    get(lslboost::spirit::extended_variant<BOOST_VARIANT_ENUM_PARAMS(T)> const& x)
    {
        return lslboost::get<T>(x.get());
    }

    template <typename T, BOOST_VARIANT_ENUM_PARAMS(typename T)>
    inline T&
    get(lslboost::spirit::extended_variant<BOOST_VARIANT_ENUM_PARAMS(T)>& x)
    {
        return lslboost::get<T>(x.get());
    }

    template <typename T, BOOST_VARIANT_ENUM_PARAMS(typename T)>
    inline T const*
    get(lslboost::spirit::extended_variant<BOOST_VARIANT_ENUM_PARAMS(T)> const* x)
    {
        return lslboost::get<T>(&x->get());
    }

    template <typename T, BOOST_VARIANT_ENUM_PARAMS(typename T)>
    inline T*
    get(lslboost::spirit::extended_variant<BOOST_VARIANT_ENUM_PARAMS(T)>* x)
    {
        return lslboost::get<T>(&x->get());
    }
}

#endif
