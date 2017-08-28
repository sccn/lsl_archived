// -- core.hpp -- Boost Lambda Library -------------------------------------
//
// Copyright (C) 2000 Gary Powell (powellg@amazon.com)
// Copyright (C) 1999, 2000 Jaakko Jarvi (jaakko.jarvi@cs.utu.fi)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)
//
// For more information, see www.lslboost.org
// 
// Includes the core of LL, without any real features for client:
// 
// tuples, lambda functors, return type deduction templates,
// argument substitution mechanism (select functions)
// 
// Some functionality comes as well:
// Assignment and subscript operators, as well as function
// call operator for placeholder variables.
// -------------------------------------------------------------------------

#ifndef BOOST_LAMBDA_CORE_HPP
#define BOOST_LAMBDA_CORE_HPP

#include "lslboost/type_traits/transform_traits.hpp"
#include "lslboost/type_traits/cv_traits.hpp"

#include "lslboost/tuple/tuple.hpp"

// inject some of the tuple names into lambda 
namespace lslboost {
namespace lambda {

using ::lslboost::tuples::tuple;
using ::lslboost::tuples::null_type;

} // lambda
} // lslboost

#include "lslboost/lambda/detail/lambda_config.hpp"
#include "lslboost/lambda/detail/lambda_fwd.hpp"

#include "lslboost/lambda/detail/arity_code.hpp"
#include "lslboost/lambda/detail/actions.hpp"

#include "lslboost/lambda/detail/lambda_traits.hpp"

#include "lslboost/lambda/detail/function_adaptors.hpp"
#include "lslboost/lambda/detail/return_type_traits.hpp"

#include "lslboost/lambda/detail/select_functions.hpp"

#include "lslboost/lambda/detail/lambda_functor_base.hpp"

#include "lslboost/lambda/detail/lambda_functors.hpp"

#include "lslboost/lambda/detail/ret.hpp"

namespace lslboost {
namespace lambda {

namespace {

  // These are constants types and need to be initialised
  lslboost::lambda::placeholder1_type free1 = lslboost::lambda::placeholder1_type();
  lslboost::lambda::placeholder2_type free2 = lslboost::lambda::placeholder2_type();
  lslboost::lambda::placeholder3_type free3 = lslboost::lambda::placeholder3_type();

  lslboost::lambda::placeholder1_type& _1 = free1;
  lslboost::lambda::placeholder2_type& _2 = free2;
  lslboost::lambda::placeholder3_type& _3 = free3;
  // _1, _2, ... naming scheme by Peter Dimov
} // unnamed
   
} // lambda
} // lslboost
   
   
#endif //BOOST_LAMBDA_CORE_HPP
