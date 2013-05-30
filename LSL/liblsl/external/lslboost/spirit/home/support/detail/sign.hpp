/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2001-2011 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(SPIRIT_SIGN_MAR_11_2009_0734PM)
#define SPIRIT_SIGN_MAR_11_2009_0734PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <lslboost/config/no_tr1/cmath.hpp>
#include <lslboost/version.hpp>
#if BOOST_VERSION < 104000 
#include <lslboost/spirit/home/support/detail/math/fpclassify.hpp>
#include <lslboost/spirit/home/support/detail/math/signbit.hpp>
#else
#include <lslboost/math/special_functions/fpclassify.hpp>
#include <lslboost/math/special_functions/sign.hpp>
#endif

namespace lslboost { namespace spirit { namespace detail
{
#if BOOST_VERSION < 104000
    // signbit(-NAN) is broken for versions of Boost earlier than 1.40.0
    // This routine has been taken and adapted from Johan Rade's fp_traits 
    // library
    template<typename T> 
    inline bool (signbit)(T x)
    {
        return (lslboost::spirit::math::signbit)(x);
    }

    template<typename T> 
    inline T (changesign)(T x)
    {
        return (lslboost::spirit::math::changesign)(x);
    }
#else
    template<typename T> 
    inline bool (signbit)(T x)
    {
        return (lslboost::math::signbit)(x) ? true : false;
    }

    // This routine has been taken and adapted from Johan Rade's fp_traits 
    // library
    template<typename T> 
    inline T (changesign)(T x)
    {
#if defined(BOOST_MATH_USE_STD_FPCLASSIFY) && !defined(BOOST_MATH_DISABLE_STD_FPCLASSIFY)
        return -x;
#else
        typedef typename math::detail::fp_traits<T>::type traits_type;

        typename traits_type::bits a;
        traits_type::get_bits(x, a);
        a ^= traits_type::sign;
        traits_type::set_bits(x, a);
        return x;
#endif
    }
#endif

}}}

#endif
