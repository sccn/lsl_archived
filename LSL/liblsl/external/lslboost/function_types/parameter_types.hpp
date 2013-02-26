
// (C) Copyright Tobias Schwinger
//
// Use modification and distribution are subject to the lslboost Software License,
// Version 1.0. (See http://www.lslboost.org/LICENSE_1_0.txt).

//------------------------------------------------------------------------------

#ifndef BOOST_FT_PARAMETER_TYPES_HPP_INCLUDED
#define BOOST_FT_PARAMETER_TYPES_HPP_INCLUDED

#include <lslboost/blank.hpp>
#include <lslboost/mpl/if.hpp>

#include <lslboost/mpl/aux_/lambda_support.hpp>
#include <lslboost/type_traits/detail/template_arity_spec.hpp>

#include <lslboost/mpl/pop_front.hpp>

#include <lslboost/function_types/is_callable_builtin.hpp>
#include <lslboost/function_types/components.hpp>

namespace lslboost 
{ 
  namespace function_types 
  {
    using mpl::placeholders::_;
 
    template< typename T, typename ClassTypeTransform = add_reference<_> >
    struct parameter_types;

    namespace detail
    {
      template<typename T, typename ClassTypeTransform> 
      struct parameter_types_impl
        : mpl::pop_front
          < typename function_types::components<T,ClassTypeTransform>::types 
          >::type
      { };
    }

    template<typename T, typename ClassTypeTransform> struct parameter_types
      : mpl::if_
        < function_types::is_callable_builtin<T>
        , detail::parameter_types_impl<T,ClassTypeTransform>, lslboost::blank
        >::type
    {
      BOOST_MPL_AUX_LAMBDA_SUPPORT(2,parameter_types,(T,ClassTypeTransform)) 
    };
  }
  BOOST_TT_AUX_TEMPLATE_ARITY_SPEC(2,function_types::parameter_types)
}

#endif

