//  (C) Copyright John Maddock 2010.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)

#ifndef BOOST_MATH_TUPLE_HPP_INCLUDED
#  define BOOST_MATH_TUPLE_HPP_INCLUDED
#  include <lslboost/config.hpp>

#include <lslboost/tr1/detail/config.hpp>  // for BOOST_HAS_TR1_TUPLE

#ifndef BOOST_NO_CXX11_HDR_TUPLE

#include <tuple>

namespace lslboost{ namespace math{

using ::std::tuple;

// [6.1.3.2] Tuple creation functions
using ::std::ignore;
using ::std::make_tuple;
using ::std::tie;
using ::std::get;

// [6.1.3.3] Tuple helper classes
using ::std::tuple_size;
using ::std::tuple_element;

}}

#elif defined(BOOST_HAS_TR1_TUPLE)

#include <lslboost/tr1/tuple.hpp>

namespace lslboost{ namespace math{

using ::std::tr1::tuple;

// [6.1.3.2] Tuple creation functions
using ::std::tr1::ignore;
using ::std::tr1::make_tuple;
using ::std::tr1::tie;
using ::std::tr1::get;

// [6.1.3.3] Tuple helper classes
using ::std::tr1::tuple_size;
using ::std::tr1::tuple_element;

}}

#elif (defined(__BORLANDC__) && (__BORLANDC__ <= 0x600)) || defined(__IBMCPP__)

#include <lslboost/tuple/tuple.hpp>
#include <lslboost/tuple/tuple_comparison.hpp>
#include <lslboost/type_traits/integral_constant.hpp>

namespace lslboost{ namespace math{

using ::lslboost::tuple;

// [6.1.3.2] Tuple creation functions
using ::lslboost::tuples::ignore;
using ::lslboost::make_tuple;
using ::lslboost::tie;

// [6.1.3.3] Tuple helper classes
template <class T> 
struct tuple_size 
   : public ::lslboost::integral_constant
   < ::std::size_t, ::lslboost::tuples::length<T>::value>
{};

template < int I, class T>
struct tuple_element
{
   typedef typename lslboost::tuples::element<I,T>::type type;
};

#if !BOOST_WORKAROUND(__BORLANDC__, < 0x0582)
// [6.1.3.4] Element access
using ::lslboost::get;
#endif

} } // namespaces

#else

#include <lslboost/fusion/include/tuple.hpp>
#include <lslboost/fusion/include/std_pair.hpp>

namespace lslboost{ namespace math{

using ::lslboost::fusion::tuple;

// [6.1.3.2] Tuple creation functions
using ::lslboost::fusion::ignore;
using ::lslboost::fusion::make_tuple;
using ::lslboost::fusion::tie;
using ::lslboost::fusion::get;

// [6.1.3.3] Tuple helper classes
using ::lslboost::fusion::tuple_size;
using ::lslboost::fusion::tuple_element;

}}

#endif

#endif


