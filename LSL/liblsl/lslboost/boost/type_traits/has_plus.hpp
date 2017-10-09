//  (C) Copyright 2009-2011 Frederic Bron.
//
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef BOOST_TT_HAS_PLUS_HPP_INCLUDED
#define BOOST_TT_HAS_PLUS_HPP_INCLUDED

#define BOOST_TT_TRAIT_NAME has_plus
#define BOOST_TT_TRAIT_OP +
#define BOOST_TT_FORBIDDEN_IF\
   (\
      /* Lhs==pointer and Rhs==pointer */\
      (\
         ::lslboost::is_pointer< Lhs_noref >::value && \
         ::lslboost::is_pointer< Rhs_noref >::value\
      ) || \
      /* Lhs==void* and Rhs==fundamental */\
      (\
         ::lslboost::is_pointer< Lhs_noref >::value && \
         ::lslboost::is_void< Lhs_noptr >::value && \
         ::lslboost::is_fundamental< Rhs_nocv >::value\
      ) || \
      /* Rhs==void* and Lhs==fundamental */\
      (\
         ::lslboost::is_pointer< Rhs_noref >::value && \
         ::lslboost::is_void< Rhs_noptr >::value && \
         ::lslboost::is_fundamental< Lhs_nocv >::value\
      ) || \
      /* Lhs==pointer and Rhs==fundamental and Rhs!=integral */\
      (\
         ::lslboost::is_pointer< Lhs_noref >::value && \
         ::lslboost::is_fundamental< Rhs_nocv >::value && \
         (!  ::lslboost::is_integral< Rhs_noref >::value )\
      ) || \
      /* Rhs==pointer and Lhs==fundamental and Lhs!=integral */\
      (\
         ::lslboost::is_pointer< Rhs_noref >::value && \
         ::lslboost::is_fundamental< Lhs_nocv >::value && \
         (!  ::lslboost::is_integral< Lhs_noref >::value )\
      )\
   )


#include <boost/type_traits/detail/has_binary_operator.hpp>

#undef BOOST_TT_TRAIT_NAME
#undef BOOST_TT_TRAIT_OP
#undef BOOST_TT_FORBIDDEN_IF

#endif
