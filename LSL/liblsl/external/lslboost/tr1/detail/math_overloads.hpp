//  (C) Copyright John Maddock 2005.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)

#ifndef BOOST_TR1_MATH_OVERLOADS_HPP_INCLUDED
#  define BOOST_TR1_MATH_OVERLOADS_HPP_INCLUDED
#  include <lslboost/config.hpp>

#  ifndef BOOST_NO_SFINAE
#     include <lslboost/utility/enable_if.hpp>
#     include <lslboost/type_traits/is_convertible.hpp>
#     define BOOST_TR1_MATH_RETURN(RET) typename ::lslboost::enable_if< ::lslboost::is_convertible<T,double>, RET >::type
#  else
#     define BOOST_TR1_MATH_RETURN(RET) RET
#  endif

#  include <lslboost/type_traits/is_floating_point.hpp>
#  include <lslboost/type_traits/is_same.hpp>
#  include <lslboost/mpl/if.hpp>

namespace lslboost{ namespace tr1_detail{

template <class T, class U>
struct largest_real
{
   typedef typename lslboost::mpl::if_<
      lslboost::is_same<long double, T>,
      long double,
      typename lslboost::mpl::if_<
         lslboost::is_same<long double, U>,
         long double,
         typename lslboost::mpl::if_<
            lslboost::is_same<double, T>,
            double,
            typename lslboost::mpl::if_<
               lslboost::is_same<double, U>,
               double,
               float
            >::type
         >::type
      >::type
   >::type type;
};

template <class T, class U>
struct promote_to_real
{
   typedef typename largest_real<
      typename lslboost::mpl::if_< lslboost::is_floating_point<T>, T, double>::type,
      typename lslboost::mpl::if_< lslboost::is_floating_point<U>, U, double>::type
   >::type type;
};

} }

#endif

