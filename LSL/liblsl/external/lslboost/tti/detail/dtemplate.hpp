
//  (C) Copyright Edward Diener 2011,2012,2013
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt).

#if !defined(BOOST_TTI_DETAIL_TEMPLATE_HPP)
#define BOOST_TTI_DETAIL_TEMPLATE_HPP

#include <lslboost/config.hpp>
#include <lslboost/mpl/bool.hpp>
#include <lslboost/mpl/eval_if.hpp>
#include <lslboost/mpl/has_xxx.hpp>
#include <lslboost/preprocessor/cat.hpp>
#include <lslboost/preprocessor/debug/assert.hpp>
#include <lslboost/preprocessor/facilities/is_empty.hpp>
#include <lslboost/type_traits/is_class.hpp>

#define BOOST_TTI_DETAIL_IS_HELPER_BOOST_PP_NIL

#define BOOST_TTI_DETAIL_IS_NIL(param) \
  BOOST_PP_IS_EMPTY \
    ( \
    BOOST_PP_CAT(BOOST_TTI_DETAIL_IS_HELPER_,param) \
    ) \
/**/

#define BOOST_TTI_DETAIL_TRAIT_ASSERT_NOT_NIL(trait,name,params) \
  BOOST_PP_ASSERT_MSG(0, "The parameter must be BOOST_PP_NIL") \
/**/

#define BOOST_TTI_DETAIL_TRAIT_CHECK_IS_NIL(trait,name,params) \
  BOOST_PP_IIF \
    ( \
    BOOST_TTI_DETAIL_IS_NIL(params), \
    BOOST_TTI_DETAIL_TRAIT_HAS_TEMPLATE, \
    BOOST_TTI_DETAIL_TRAIT_ASSERT_NOT_NIL \
    ) \
    (trait,name,params) \
/**/

#define BOOST_TTI_DETAIL_TRAIT_HAS_TEMPLATE_THT(trait,name) \
  BOOST_MPL_HAS_XXX_TEMPLATE_NAMED_DEF(BOOST_PP_CAT(trait,_detail_mpl), name, false) \
  template<class BOOST_TTI_DETAIL_TP_T> \
  struct BOOST_PP_CAT(trait,_tht) : \
    BOOST_PP_CAT(trait,_detail_mpl)<BOOST_TTI_DETAIL_TP_T> \
    { \
    }; \
/**/

#define BOOST_TTI_DETAIL_TRAIT_HAS_TEMPLATE(trait,name,params) \
  BOOST_TTI_DETAIL_TRAIT_HAS_TEMPLATE_THT(trait,name) \
  template<class BOOST_TTI_DETAIL_TP_T> \
  struct trait \
    { \
    typedef typename \
  	lslboost::mpl::eval_if \
  		< \
  		lslboost::is_class<BOOST_TTI_DETAIL_TP_T>, \
  		BOOST_PP_CAT(trait,_tht)<BOOST_TTI_DETAIL_TP_T>, \
  		lslboost::mpl::false_ \
  		>::type type; \
    BOOST_STATIC_CONSTANT(bool,value=type::value); \
    }; \
/**/

#endif // !BOOST_TTI_DETAIL_TEMPLATE_HPP
