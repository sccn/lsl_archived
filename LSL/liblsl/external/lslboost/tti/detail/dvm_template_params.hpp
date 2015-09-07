
//  (C) Copyright Edward Diener 2011,2012,2013
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt).

#if !defined(BOOST_TTI_DETAIL_VM_TEMPLATE_PARAMS_HPP)
#define BOOST_TTI_DETAIL_VM_TEMPLATE_PARAMS_HPP

#include <lslboost/config.hpp>
#include <lslboost/preprocessor/config/config.hpp>

#if BOOST_PP_VARIADICS

#include <lslboost/mpl/eval_if.hpp>
#include <lslboost/mpl/has_xxx.hpp>
#include <lslboost/mpl/identity.hpp>
#include <lslboost/preprocessor/arithmetic/add.hpp>
#include <lslboost/preprocessor/variadic/size.hpp>
#include <lslboost/preprocessor/cat.hpp>
#include <lslboost/preprocessor/comparison/equal.hpp>
#include <lslboost/preprocessor/control/iif.hpp>
#include <lslboost/preprocessor/detail/is_binary.hpp>
#include <lslboost/preprocessor/facilities/is_empty.hpp>
#include <lslboost/preprocessor/seq/enum.hpp>
#include <lslboost/preprocessor/seq/seq.hpp>
#include <lslboost/preprocessor/variadic/elem.hpp>
#include <lslboost/preprocessor/variadic/to_seq.hpp>
#include <lslboost/tti/detail/dtemplate.hpp>
#include <lslboost/tti/detail/dtemplate_params.hpp>
#include <lslboost/type_traits/is_class.hpp>

#if !defined(BOOST_MPL_CFG_NO_HAS_XXX_TEMPLATE)
#if !BOOST_WORKAROUND(BOOST_MSVC, <= 1400)

#define BOOST_TTI_DETAIL_VM_TRAIT_HAS_TEMPLATE_CHECK_PARAMS(trait,name,...) \
  BOOST_TTI_DETAIL_HAS_MEMBER_WITH_FUNCTION_SFINAE \
    (  \
      ( BOOST_PP_ADD(BOOST_PP_VARIADIC_SIZE(__VA_ARGS__),4), ( trait, name, 1, false, __VA_ARGS__ ) )  \
    )  \
/**/

#else // !!BOOST_WORKAROUND(BOOST_MSVC, <= 1400)

#define BOOST_TTI_DETAIL_VM_TRAIT_HAS_TEMPLATE_CHECK_PARAMS(trait,name,...) \
  BOOST_TTI_DETAIL_HAS_MEMBER_WITH_TEMPLATE_SFINAE \
    ( \
      ( BOOST_PP_ADD(BOOST_PP_VARIADIC_SIZE(__VA_ARGS__),4), ( trait, name, 1, false, __VA_ARGS__ ) )  \
    ) \
/**/

#endif // !BOOST_WORKAROUND(BOOST_MSVC, <= 1400)
#else // defined(BOOST_MPL_CFG_NO_HAS_XXX_TEMPLATE)

#define BOOST_TTI_DETAIL_VM_TRAIT_HAS_TEMPLATE_CHECK_PARAMS(trait,name,...) \
  BOOST_TTI_DETAIL_SAME(trait,name) \
/**/

#endif // !defined(BOOST_MPL_CFG_NO_HAS_XXX_TEMPLATE)

#define BOOST_TTI_DETAIL_VM_CHECK_MORE_THAN_TWO(trait,...) \
  BOOST_PP_IIF \
    ( \
    BOOST_PP_EQUAL \
      ( \
      BOOST_PP_VARIADIC_SIZE(__VA_ARGS__), \
      2 \
      ), \
    BOOST_TTI_DETAIL_VM_TRAIT_CHOOSE_FROM_TWO, \
    BOOST_TTI_DETAIL_VM_TRAIT_EXPAND_ARGUMENTS \
    ) \
    (trait,__VA_ARGS__) \
/**/

#define BOOST_TTI_DETAIL_VM_TRAIT_CHOOSE_FROM_TWO(trait,...) \
  BOOST_PP_IIF \
    ( \
    BOOST_PP_IS_BINARY \
      ( \
      BOOST_PP_VARIADIC_ELEM(1,__VA_ARGS__) \
      ), \
    BOOST_TTI_DETAIL_TRAIT_HAS_TEMPLATE_CHECK_PARAMS, \
    BOOST_TTI_DETAIL_VM_TRAIT_CHOOSE_IF_NIL \
    ) \
  ( \
  trait, \
  BOOST_PP_VARIADIC_ELEM(0,__VA_ARGS__), \
  BOOST_PP_VARIADIC_ELEM(1,__VA_ARGS__) \
  ) \
/**/

#define BOOST_TTI_DETAIL_VM_IS_NIL(param) \
  BOOST_PP_IS_EMPTY \
    ( \
    BOOST_PP_CAT(BOOST_TTI_DETAIL_IS_HELPER_,param) \
    ) \
/**/

#define BOOST_TTI_DETAIL_VM_TRAIT_CHOOSE_IF_NIL(trait,name,param) \
  BOOST_PP_IIF \
    ( \
    BOOST_TTI_DETAIL_VM_IS_NIL(param), \
    BOOST_TTI_DETAIL_TRAIT_HAS_TEMPLATE, \
    BOOST_TTI_DETAIL_VM_CALL_TRAIT_HAS_TEMPLATE_CHECK_PARAMS \
    ) \
  (trait,name,param) \
/**/

#define BOOST_TTI_DETAIL_VM_VARIADIC_TAIL(...) \
  BOOST_PP_SEQ_ENUM \
    ( \
    BOOST_PP_SEQ_TAIL \
      ( \
      BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__) \
      ) \
    ) \
/**/

#define BOOST_TTI_DETAIL_VM_TRAIT_EXPAND_ARGUMENTS(trait,...) \
  BOOST_TTI_DETAIL_VM_CALL_TRAIT_HAS_TEMPLATE_CHECK_PARAMS \
    ( \
    trait, \
    BOOST_PP_VARIADIC_ELEM(0,__VA_ARGS__), \
    BOOST_TTI_DETAIL_VM_VARIADIC_TAIL(__VA_ARGS__) \
    ) \
/**/

#define BOOST_TTI_DETAIL_VM_TRAIT_HAS_TEMPLATE(trait,...) \
  BOOST_TTI_DETAIL_TRAIT_HAS_TEMPLATE \
    ( \
    trait, \
    BOOST_PP_VARIADIC_ELEM(0,__VA_ARGS__), \
    BOOST_PP_NIL \
    ) \
/**/

#define BOOST_TTI_DETAIL_VM_CT_INVOKE(trait,name,...) \
  BOOST_TTI_DETAIL_VM_TRAIT_HAS_TEMPLATE_CHECK_PARAMS(BOOST_PP_CAT(trait,_detail),name,__VA_ARGS__) \
  template<class BOOST_TTI_DETAIL_TP_T> \
  struct BOOST_PP_CAT(trait,_detail_vm_ct_invoke) : \
  	BOOST_PP_CAT(trait,_detail)<BOOST_TTI_DETAIL_TP_T> \
    { \
    }; \
/**/

#define BOOST_TTI_DETAIL_VM_CALL_TRAIT_HAS_TEMPLATE_CHECK_PARAMS(trait,name,...) \
  BOOST_TTI_DETAIL_VM_CT_INVOKE(trait,name,__VA_ARGS__) \
  template<class BOOST_TTI_DETAIL_TP_T> \
  struct trait \
    { \
    typedef typename \
  	lslboost::mpl::eval_if \
  		< \
  		lslboost::is_class<BOOST_TTI_DETAIL_TP_T>, \
  		BOOST_PP_CAT(trait,_detail_vm_ct_invoke)<BOOST_TTI_DETAIL_TP_T>, \
  		lslboost::mpl::false_ \
  		>::type type; \
    BOOST_STATIC_CONSTANT(bool,value=type::value); \
    }; \
/**/

#endif // BOOST_PP_VARIADICS

#endif // BOOST_TTI_DETAIL_VM_TEMPLATE_PARAMS_HPP
