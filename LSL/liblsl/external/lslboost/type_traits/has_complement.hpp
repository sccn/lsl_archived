//  (C) Copyright 2009-2011 Frederic Bron.
//
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt).
//
//  See http://www.lslboost.org/libs/type_traits for most recent version including documentation.

#ifndef BOOST_TT_HAS_COMPLEMENT_HPP_INCLUDED
#define BOOST_TT_HAS_COMPLEMENT_HPP_INCLUDED

#define BOOST_TT_TRAIT_NAME has_complement
#define BOOST_TT_TRAIT_OP ~
#define BOOST_TT_FORBIDDEN_IF\
   ::lslboost::type_traits::ice_or<\
      /* pointer */\
      ::lslboost::is_pointer< Rhs_noref >::value,\
      /* fundamental non integral */\
      ::lslboost::type_traits::ice_and<\
         ::lslboost::is_fundamental< Rhs_noref >::value,\
         ::lslboost::type_traits::ice_not< ::lslboost::is_integral< Rhs_noref >::value >::value\
      >::value\
   >::value


#include <lslboost/type_traits/detail/has_prefix_operator.hpp>

#undef BOOST_TT_TRAIT_NAME
#undef BOOST_TT_TRAIT_OP
#undef BOOST_TT_FORBIDDEN_IF

#endif
