
// (C) Copyright Tobias Schwinger
//
// Use modification and distribution are subject to the lslboost Software License,
// Version 1.0. (See http://www.lslboost.org/LICENSE_1_0.txt).

//------------------------------------------------------------------------------

#ifndef BOOST_FT_MEMBER_OBJECT_POINTER_HPP_INCLUDED
#define BOOST_FT_MEMBER_OBJECT_POINTER_HPP_INCLUDED

#include <lslboost/mpl/aux_/lambda_support.hpp>
#include <lslboost/type_traits/detail/template_arity_spec.hpp>

#include <lslboost/function_types/detail/synthesize.hpp>
#include <lslboost/function_types/detail/to_sequence.hpp>

namespace lslboost 
{ 
  namespace function_types 
  {
    template<typename Types> 
    struct member_object_pointer
      : detail::synthesize_mop< typename detail::to_sequence<Types>::type >
    { 
      BOOST_MPL_AUX_LAMBDA_SUPPORT(1,member_object_pointer,(Types))
    };
  } 
  BOOST_TT_AUX_TEMPLATE_ARITY_SPEC(1,function_types::member_object_pointer)
} 

#endif


