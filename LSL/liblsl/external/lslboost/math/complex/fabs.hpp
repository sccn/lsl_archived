//  (C) Copyright John Maddock 2005.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)

#ifndef BOOST_MATH_COMPLEX_FABS_INCLUDED
#define BOOST_MATH_COMPLEX_FABS_INCLUDED

#ifndef BOOST_MATH_HYPOT_INCLUDED
#  include <lslboost/math/special_functions/hypot.hpp>
#endif

namespace lslboost{ namespace math{

template<class T> 
inline T fabs(const std::complex<T>& z)
{
   return ::lslboost::math::hypot(z.real(), z.imag());
}

} } // namespaces

#endif // BOOST_MATH_COMPLEX_FABS_INCLUDED
