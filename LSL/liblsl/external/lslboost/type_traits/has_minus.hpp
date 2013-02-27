//  (C) Copyright 2009-2011 Frederic Bron.
//
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt).
//
//  See http://www.lslboost.org/libs/type_traits for most recent version including documentation.

#ifndef BOOST_TT_HAS_MINUS_HPP_INCLUDED
#define BOOST_TT_HAS_MINUS_HPP_INCLUDED

#define BOOST_TT_TRAIT_NAME has_minus
#define BOOST_TT_TRAIT_OP -
#define BOOST_TT_FORBIDDEN_IF\
   ::lslboost::type_traits::ice_or<\
      /* Lhs==pointer and Rhs==fundamental and Rhs!=integral */\
      ::lslboost::type_traits::ice_and<\
         ::lslboost::is_pointer< Lhs_noref >::value,\
         ::lslboost::is_fundamental< Rhs_nocv >::value,\
         ::lslboost::type_traits::ice_not< ::lslboost::is_integral< Rhs_noref >::value >::value\
      >::value,\
      /* Lhs==void* and (Rhs==fundamental or Rhs==pointer) */\
      ::lslboost::type_traits::ice_and<\
         ::lslboost::is_pointer< Lhs_noref >::value,\
         ::lslboost::is_void< Lhs_noptr >::value,\
         ::lslboost::type_traits::ice_or<\
            ::lslboost::is_fundamental< Rhs_nocv >::value,\
            ::lslboost::is_pointer< Rhs_noref >::value\
         >::value\
      >::value,\
      /* Rhs==void* and (Lhs==fundamental or Lhs==pointer) */\
      ::lslboost::type_traits::ice_and<\
         ::lslboost::is_pointer< Rhs_noref >::value,\
         ::lslboost::is_void< Rhs_noptr >::value,\
         ::lslboost::type_traits::ice_or<\
            ::lslboost::is_fundamental< Lhs_nocv >::value,\
            ::lslboost::is_pointer< Lhs_noref >::value\
         >::value\
      >::value,\
      /* Lhs=fundamental and Rhs=pointer */\
      ::lslboost::type_traits::ice_and<\
         ::lslboost::is_fundamental< Lhs_nocv >::value,\
         ::lslboost::is_pointer< Rhs_noref >::value\
      >::value,\
      /* two different pointers */\
      ::lslboost::type_traits::ice_and<\
         ::lslboost::is_pointer< Lhs_noref >::value,\
         ::lslboost::is_pointer< Rhs_noref >::value,\
         ::lslboost::type_traits::ice_not< ::lslboost::is_same< Lhs_nocv, Rhs_nocv >::value >::value\
      >::value\
   >::value


#include <lslboost/type_traits/detail/has_binary_operator.hpp>

#undef BOOST_TT_TRAIT_NAME
#undef BOOST_TT_TRAIT_OP
#undef BOOST_TT_FORBIDDEN_IF

#endif
