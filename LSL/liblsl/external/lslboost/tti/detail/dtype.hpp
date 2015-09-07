
//  (C) Copyright Edward Diener 2011,2012,2013
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt).

#if !defined(BOOST_TTI_DETAIL_TYPE_HPP)
#define BOOST_TTI_DETAIL_TYPE_HPP

#include <lslboost/config.hpp>
#include <lslboost/mpl/apply.hpp>
#include <lslboost/mpl/bool.hpp>
#include <lslboost/mpl/eval_if.hpp>
#include <lslboost/mpl/has_xxx.hpp>
#include <lslboost/preprocessor/cat.hpp>
#include <lslboost/tti/detail/ddeftype.hpp>
#include <lslboost/tti/detail/dlambda.hpp>
#include <lslboost/tti/gen/namespace_gen.hpp>
#include <lslboost/type_traits/is_class.hpp>

#define BOOST_TTI_DETAIL_TRAIT_INVOKE_HAS_TYPE(trait,name) \
template<class BOOST_TTI_DETAIL_TP_T,class BOOST_TTI_DETAIL_TP_MFC> \
struct BOOST_PP_CAT(trait,_detail_type_invoke) : \
  lslboost::mpl::apply<BOOST_TTI_DETAIL_TP_MFC,typename BOOST_TTI_DETAIL_TP_T::name> \
  { \
  }; \
/**/

#define BOOST_TTI_DETAIL_TRAIT_HAS_TYPE_OP_CHOOSE(trait,name) \
BOOST_MPL_HAS_XXX_TRAIT_NAMED_DEF(BOOST_PP_CAT(trait,_detail_type_mpl), name, false) \
BOOST_TTI_DETAIL_TRAIT_INVOKE_HAS_TYPE(trait,name) \
template<class BOOST_TTI_DETAIL_TP_T,class BOOST_TTI_DETAIL_TP_U,class BOOST_TTI_DETAIL_TP_B> \
struct BOOST_PP_CAT(trait,_detail_type_op_choose) \
  { \
  BOOST_MPL_ASSERT((BOOST_TTI_NAMESPACE::detail::is_lambda_expression<BOOST_TTI_DETAIL_TP_U>)); \
  typedef typename BOOST_PP_CAT(trait,_detail_type_invoke)<BOOST_TTI_DETAIL_TP_T,BOOST_TTI_DETAIL_TP_U>::type type; \
  }; \
\
template<class BOOST_TTI_DETAIL_TP_T,class BOOST_TTI_DETAIL_TP_U> \
struct BOOST_PP_CAT(trait,_detail_type_op_choose)<BOOST_TTI_DETAIL_TP_T,BOOST_TTI_DETAIL_TP_U,lslboost::mpl::false_::type> : \
  lslboost::mpl::false_ \
  { \
  }; \
\
template<class BOOST_TTI_DETAIL_TP_T> \
struct BOOST_PP_CAT(trait,_detail_type_op_choose)<BOOST_TTI_DETAIL_TP_T,BOOST_TTI_NAMESPACE::detail::deftype,lslboost::mpl::true_::type> : \
  lslboost::mpl::true_ \
  { \
  }; \
/**/

#define BOOST_TTI_DETAIL_TRAIT_HAS_TYPE_OP(trait,name) \
BOOST_TTI_DETAIL_TRAIT_HAS_TYPE_OP_CHOOSE(trait,name) \
template<class BOOST_TTI_DETAIL_TP_T,class BOOST_TTI_DETAIL_TP_U> \
struct BOOST_PP_CAT(trait,_detail_type_op) : \
  BOOST_PP_CAT(trait,_detail_type_op_choose) \
    < \
    BOOST_TTI_DETAIL_TP_T, \
    BOOST_TTI_DETAIL_TP_U, \
    typename BOOST_PP_CAT(trait,_detail_type_mpl)<BOOST_TTI_DETAIL_TP_T>::type \
    > \
  { \
  }; \
/**/

#define BOOST_TTI_DETAIL_TRAIT_HAS_TYPE(trait,name) \
BOOST_TTI_DETAIL_TRAIT_HAS_TYPE_OP(trait,name) \
template<class BOOST_TTI_DETAIL_TP_T,class BOOST_TTI_DETAIL_TP_U> \
struct BOOST_PP_CAT(trait,_detail_type) : \
	lslboost::mpl::eval_if \
		< \
 		lslboost::is_class<BOOST_TTI_DETAIL_TP_T>, \
 		BOOST_PP_CAT(trait,_detail_type_op)<BOOST_TTI_DETAIL_TP_T,BOOST_TTI_DETAIL_TP_U>, \
 		lslboost::mpl::false_ \
		> \
  { \
  }; \
/**/

#endif // BOOST_TTI_DETAIL_TYPE_HPP
