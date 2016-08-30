//  (C) Copyright John Maddock 2008.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)

#ifndef BOOST_TR1_UNORDERED_MAP_HPP_INCLUDED
#  define BOOST_TR1_UNORDERED_MAP_HPP_INCLUDED
#  include <lslboost/tr1/detail/config.hpp>

#ifdef BOOST_HAS_TR1_UNORDERED_MAP

#  if defined(BOOST_HAS_INCLUDE_NEXT) && !defined(BOOST_TR1_DISABLE_INCLUDE_NEXT)
#     include_next BOOST_TR1_HEADER(unordered_map)
#  else
#     include <lslboost/tr1/detail/config_all.hpp>
#     include BOOST_TR1_STD_HEADER(BOOST_TR1_PATH(unordered_map))
#  endif

#else

#include <lslboost/unordered_map.hpp>

namespace std{ namespace tr1{

   using ::lslboost::unordered_map;
   using ::lslboost::unordered_multimap;
   using ::lslboost::swap;

} } // namespaces

#endif

#endif
