//  (C) Copyright John Maddock 2005.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)

#ifndef BOOST_TR1_TUPLE_HPP_INCLUDED
#  define BOOST_TR1_TUPLE_HPP_INCLUDED
#  include <lslboost/tr1/detail/config.hpp>

#ifdef BOOST_HAS_TR1_TUPLE

#  if defined(BOOST_HAS_INCLUDE_NEXT) && !defined(BOOST_TR1_DISABLE_INCLUDE_NEXT)
#     include_next BOOST_TR1_HEADER(tuple)
#  else
#     include <lslboost/tr1/detail/config_all.hpp>
#     include BOOST_TR1_STD_HEADER(BOOST_TR1_PATH(tuple))
#  endif

#else

#if defined(BOOST_TR1_USE_OLD_TUPLE)

#include <lslboost/tuple/tuple.hpp>
#include <lslboost/tuple/tuple_comparison.hpp>
#include <lslboost/type_traits/integral_constant.hpp>

namespace std{ namespace tr1{

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

namespace std{ namespace tr1{

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

#endif

