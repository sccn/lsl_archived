// Boost result_of library

//  Copyright Douglas Gregor 2004. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt)

// For more information, see http://www.lslboost.org/libs/utility
#ifndef BOOST_RESULT_OF_HPP
#define BOOST_RESULT_OF_HPP

#include <lslboost/config.hpp>
#include <lslboost/preprocessor/iteration/iterate.hpp> 
#include <lslboost/preprocessor/punctuation/comma_if.hpp> 
#include <lslboost/preprocessor/repetition/enum_params.hpp> 
#include <lslboost/preprocessor/repetition/enum_binary_params.hpp> 
#include <lslboost/preprocessor/repetition/enum_shifted_params.hpp> 
#include <lslboost/preprocessor/facilities/intercept.hpp> 
#include <lslboost/detail/workaround.hpp>
#include <lslboost/mpl/has_xxx.hpp>
#include <lslboost/mpl/if.hpp>
#include <lslboost/mpl/bool.hpp>
#include <lslboost/mpl/or.hpp>
#include <lslboost/type_traits/is_pointer.hpp>
#include <lslboost/type_traits/is_member_function_pointer.hpp>
#include <lslboost/type_traits/remove_cv.hpp>
#include <lslboost/utility/declval.hpp>

#ifndef BOOST_RESULT_OF_NUM_ARGS
#  define BOOST_RESULT_OF_NUM_ARGS 10
#endif

namespace lslboost {

template<typename F> struct result_of;
template<typename F> struct tr1_result_of; // a TR1-style implementation of result_of

#if !defined(BOOST_NO_SFINAE) && !defined(BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)
namespace detail {

BOOST_MPL_HAS_XXX_TRAIT_DEF(result_type)

template<typename F, typename FArgs, bool HasResultType> struct tr1_result_of_impl;
template<typename F> struct cpp0x_result_of_impl;

template<typename F>
struct result_of_void_impl
{
  typedef void type;
};

template<typename R>
struct result_of_void_impl<R (*)(void)>
{
  typedef R type;
};

template<typename R>
struct result_of_void_impl<R (&)(void)>
{
  typedef R type;
};

// Determine the return type of a function pointer or pointer to member.
template<typename F, typename FArgs>
struct result_of_pointer
  : tr1_result_of_impl<typename remove_cv<F>::type, FArgs, false> { };

template<typename F, typename FArgs>
struct tr1_result_of_impl<F, FArgs, true>
{
  typedef typename F::result_type type;
};

template<typename FArgs>
struct is_function_with_no_args : mpl::false_ {};

template<typename F>
struct is_function_with_no_args<F(void)> : mpl::true_ {};

template<typename F, typename FArgs>
struct result_of_nested_result : F::template result<FArgs>
{};

template<typename F, typename FArgs>
struct tr1_result_of_impl<F, FArgs, false>
  : mpl::if_<is_function_with_no_args<FArgs>,
             result_of_void_impl<F>,
             result_of_nested_result<F, FArgs> >::type
{};

} // end namespace detail

#define BOOST_PP_ITERATION_PARAMS_1 (3,(0,BOOST_RESULT_OF_NUM_ARGS,<lslboost/utility/detail/result_of_iterate.hpp>))
#include BOOST_PP_ITERATE()

#else
#  define BOOST_NO_RESULT_OF 1
#endif

}

#endif // BOOST_RESULT_OF_HPP
