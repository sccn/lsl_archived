// -- lambda.hpp -- Boost Lambda Library -----------------------------------
// Copyright (C) 1999, 2000 Jaakko Jarvi (jaakko.jarvi@cs.utu.fi)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)
//
// For more information, see http://lambda.cs.utu.fi 

#ifndef BOOST_LAMBDA_LAMBDA_HPP
#define BOOST_LAMBDA_LAMBDA_HPP


#include "lslboost/lambda/core.hpp"

#ifdef BOOST_NO_FDECL_TEMPLATES_AS_TEMPLATE_TEMPLATE_PARAMS
#include <istream>
#include <ostream>
#endif

#include "lslboost/lambda/detail/operator_actions.hpp"
#include "lslboost/lambda/detail/operator_lambda_func_base.hpp"
#include "lslboost/lambda/detail/operator_return_type_traits.hpp"


#include "lslboost/lambda/detail/operators.hpp"

#ifndef BOOST_LAMBDA_FAILS_IN_TEMPLATE_KEYWORD_AFTER_SCOPE_OPER
// sorry, member ptr does not work with gcc2.95
#include "lslboost/lambda/detail/member_ptr.hpp"
#endif


#endif
