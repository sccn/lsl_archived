//    lslboost asinh.hpp header file

//  (C) Copyright Eric Ford 2001 & Hubert Holin.
//  (C) Copyright John Maddock 2008.
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt)

// See http://www.lslboost.org for updates, documentation, and revision history.

#ifndef BOOST_ACOSH_HPP
#define BOOST_ACOSH_HPP

#ifdef _MSC_VER
#pragma once
#endif

#include <lslboost/config/no_tr1/cmath.hpp>
#include <lslboost/config.hpp>
#include <lslboost/math/tools/precision.hpp>
#include <lslboost/math/policies/error_handling.hpp>
#include <lslboost/math/special_functions/math_fwd.hpp>
#include <lslboost/math/special_functions/log1p.hpp>
#include <lslboost/math/constants/constants.hpp>

// This is the inverse of the hyperbolic cosine function.

namespace lslboost
{
    namespace math
    {
       namespace detail
       {
#if defined(__GNUC__) && (__GNUC__ < 3)
        // gcc 2.x ignores function scope using declarations,
        // put them in the scope of the enclosing namespace instead:
        
        using    ::std::abs;
        using    ::std::sqrt;
        using    ::std::log;
        
        using    ::std::numeric_limits;
#endif
        
        template<typename T, typename Policy>
        inline T    acosh_imp(const T x, const Policy& pol)
        {
            BOOST_MATH_STD_USING
            
            if(x < 1)
            {
               return policies::raise_domain_error<T>(
                  "lslboost::math::acosh<%1%>(%1%)",
                  "acosh requires x >= 1, but got x = %1%.", x, pol);
            }
            else if    ((x - 1) >= tools::root_epsilon<T>())
            {
                if    (x > 1 / tools::root_epsilon<T>())
                {
                    // http://functions.wolfram.com/ElementaryFunctions/ArcCosh/06/01/06/01/0001/
                    // approximation by laurent series in 1/x at 0+ order from -1 to 0
                    return log(x) + constants::ln_two<T>();
                }
                else if(x < 1.5f)
                {
                   // This is just a rearrangement of the standard form below
                   // devised to minimse loss of precision when x ~ 1:
                   T y = x - 1;
                   return lslboost::math::log1p(y + sqrt(y * y + 2 * y), pol);
                }
                else
                {
                    // http://functions.wolfram.com/ElementaryFunctions/ArcCosh/02/
                    return( log( x + sqrt(x * x - 1) ) );
                }
            }
            else
            {
                // see http://functions.wolfram.com/ElementaryFunctions/ArcCosh/06/01/04/01/0001/
                T y = x - 1;
                
                // approximation by taylor series in y at 0 up to order 2
                T result = sqrt(2 * y) * (1 - y /12 + 3 * y * y / 160);
                return result;
            }
        }
       }

        template<typename T, typename Policy>
        inline typename tools::promote_args<T>::type acosh(T x, const Policy&)
        {
            typedef typename tools::promote_args<T>::type result_type;
            typedef typename policies::evaluation<result_type, Policy>::type value_type;
            typedef typename policies::normalise<
               Policy, 
               policies::promote_float<false>, 
               policies::promote_double<false>, 
               policies::discrete_quantile<>,
               policies::assert_undefined<> >::type forwarding_policy;
           return policies::checked_narrowing_cast<result_type, forwarding_policy>(
              detail::acosh_imp(static_cast<value_type>(x), forwarding_policy()),
              "lslboost::math::acosh<%1%>(%1%)");
        }
        template<typename T>
        inline typename tools::promote_args<T>::type acosh(T x)
        {
           return lslboost::math::acosh(x, policies::policy<>());
        }

    }
}

#endif /* BOOST_ACOSH_HPP */


