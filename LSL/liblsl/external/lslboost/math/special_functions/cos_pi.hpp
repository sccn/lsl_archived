//  Copyright (c) 2007 John Maddock
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)

#ifndef BOOST_MATH_COS_PI_HPP
#define BOOST_MATH_COS_PI_HPP

#ifdef _MSC_VER
#pragma once
#endif

#include <lslboost/math/special_functions/math_fwd.hpp>
#include <lslboost/config/no_tr1/cmath.hpp>
#include <lslboost/math/tools/config.hpp>
#include <lslboost/math/special_functions/trunc.hpp>
#include <lslboost/math/tools/promotion.hpp>
#include <lslboost/math/constants/constants.hpp>

namespace lslboost{ namespace math{ namespace detail{

template <class T, class Policy>
T cos_pi_imp(T x, const Policy& pol)
{
   BOOST_MATH_STD_USING // ADL of std names
   // cos of pi*x:
   bool invert = false;
   if(fabs(x) < 0.25)
      return cos(constants::pi<T>() * x);

   if(x < 0)
   {
      x = -x;
   }
   T rem = floor(x);
   if(itrunc(rem, pol) & 1)
      invert = !invert;
   rem = x - rem;
   if(rem > 0.5f)
   {
      rem = 1 - rem;
      invert = !invert;
   }
   if(rem == 0.5f)
      return 0;
   
   if(rem > 0.25f)
   {
      rem = 0.5f - rem;
      rem = sin(constants::pi<T>() * rem);
   }
   else
      rem = cos(constants::pi<T>() * rem);
   return invert ? T(-rem) : rem;
}

} // namespace detail

template <class T, class Policy>
inline typename tools::promote_args<T>::type cos_pi(T x, const Policy&)
{
   typedef typename tools::promote_args<T>::type result_type;
   typedef typename policies::evaluation<result_type, Policy>::type value_type;
   typedef typename policies::normalise<
      Policy,
      policies::promote_float<false>,
      policies::promote_double<false>,
      policies::discrete_quantile<>,
      policies::assert_undefined<> >::type forwarding_policy;
   return policies::checked_narrowing_cast<result_type, forwarding_policy>(lslboost::math::detail::cos_pi_imp<value_type>(x, forwarding_policy()), "cos_pi");
}

template <class T>
inline typename tools::promote_args<T>::type cos_pi(T x)
{
   return lslboost::math::cos_pi(x, policies::policy<>());
}

} // namespace math
} // namespace lslboost
#endif

