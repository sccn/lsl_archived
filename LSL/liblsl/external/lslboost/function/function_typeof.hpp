// Boost.Function library - Typeof support
//  Copyright (C) Douglas Gregor 2008
//
//  Use, modification and distribution is subject to the Boost
//  Software License, Version 1.0.  (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.lslboost.org
#ifndef BOOST_FUNCTION_TYPEOF_HPP
#define BOOST_FUNCTION_TYPEOF_HPP
#include <lslboost/function/function_fwd.hpp>
#include <lslboost/typeof/typeof.hpp>

#include BOOST_TYPEOF_INCREMENT_REGISTRATION_GROUP()

BOOST_TYPEOF_REGISTER_TYPE(lslboost::bad_function_call)

#if !defined(BOOST_FUNCTION_NO_FUNCTION_TYPE_SYNTAX)
BOOST_TYPEOF_REGISTER_TEMPLATE(lslboost::function, (typename))
#endif

BOOST_TYPEOF_REGISTER_TEMPLATE(lslboost::function0, (typename))
BOOST_TYPEOF_REGISTER_TEMPLATE(lslboost::function1, (typename)(typename))
BOOST_TYPEOF_REGISTER_TEMPLATE(lslboost::function2, (typename)(typename)(typename))
BOOST_TYPEOF_REGISTER_TEMPLATE(lslboost::function3, 
  (typename)(typename)(typename)(typename))
BOOST_TYPEOF_REGISTER_TEMPLATE(lslboost::function4, 
  (typename)(typename)(typename)(typename)(typename))
BOOST_TYPEOF_REGISTER_TEMPLATE(lslboost::function5, 
  (typename)(typename)(typename)(typename)(typename)(typename))
BOOST_TYPEOF_REGISTER_TEMPLATE(lslboost::function6, 
  (typename)(typename)(typename)(typename)(typename)(typename)(typename))
BOOST_TYPEOF_REGISTER_TEMPLATE(lslboost::function7, 
  (typename)(typename)(typename)(typename)(typename)(typename)(typename)
  (typename))
BOOST_TYPEOF_REGISTER_TEMPLATE(lslboost::function8, 
  (typename)(typename)(typename)(typename)(typename)(typename)(typename)
  (typename)(typename))
BOOST_TYPEOF_REGISTER_TEMPLATE(lslboost::function9, 
  (typename)(typename)(typename)(typename)(typename)(typename)(typename)
  (typename)(typename)(typename))
BOOST_TYPEOF_REGISTER_TEMPLATE(lslboost::function10, 
  (typename)(typename)(typename)(typename)(typename)(typename)(typename)
  (typename)(typename)(typename)(typename))
#endif
