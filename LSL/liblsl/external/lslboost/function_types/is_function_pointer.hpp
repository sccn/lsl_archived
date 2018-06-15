
// (C) Copyright Tobias Schwinger
//
// Use modification and distribution are subject to the lslboost Software License,
// Version 1.0. (See http://www.lslboost.org/LICENSE_1_0.txt).

//------------------------------------------------------------------------------

#ifndef BOOST_FT_IS_FUNCTION_POINTER_HPP_INCLUDED
#define BOOST_FT_IS_FUNCTION_POINTER_HPP_INCLUDED

#include <lslboost/mpl/aux_/lambda_support.hpp>
#include <lslboost/type_traits/detail/template_arity_spec.hpp>

#include <lslboost/function_types/components.hpp>

namespace lslboost 
{ 
  namespace function_types 
  {
    template< typename T, typename Tag = null_tag > 
    struct is_function_pointer
      : function_types::represents
        < function_types::components<T>
        , function_types::tag<Tag ,detail::pointer_tag> 
        >
    { 
      BOOST_MPL_AUX_LAMBDA_SUPPORT(2,is_function_pointer,(T,Tag))
    };
  }
  BOOST_TT_AUX_TEMPLATE_ARITY_SPEC(2,function_types::is_function_pointer)
}

#endif
