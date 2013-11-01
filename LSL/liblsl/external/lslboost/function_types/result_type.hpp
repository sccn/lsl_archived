
// (C) Copyright Tobias Schwinger
//
// Use modification and distribution are subject to the lslboost Software License,
// Version 1.0. (See http://www.lslboost.org/LICENSE_1_0.txt).

//------------------------------------------------------------------------------

#ifndef BOOST_FT_RESULT_TYPE_HPP_INCLUDED
#define BOOST_FT_RESULT_TYPE_HPP_INCLUDED

#include <lslboost/blank.hpp>
#include <lslboost/mpl/if.hpp>

#include <lslboost/mpl/aux_/lambda_support.hpp>
#include <lslboost/type_traits/detail/template_arity_spec.hpp>

#include <lslboost/mpl/at.hpp>

#include <lslboost/function_types/is_callable_builtin.hpp>
#include <lslboost/function_types/components.hpp>

namespace lslboost 
{ 
  namespace function_types 
  {
    template< typename T > struct result_type;

    namespace detail
    {
      template<typename T> struct result_type_impl
        : mpl::at_c
          < typename function_types::components<T>::types, 0 >
      { };
    }

    template<typename T> struct result_type
      : mpl::if_
        < function_types::is_callable_builtin<T>
        , detail::result_type_impl<T>, lslboost::blank
        >::type
    { 
      BOOST_MPL_AUX_LAMBDA_SUPPORT(1,result_type,(T)) 
    };
  }
  BOOST_TT_AUX_TEMPLATE_ARITY_SPEC(1,function_types::result_type)
}

#endif

