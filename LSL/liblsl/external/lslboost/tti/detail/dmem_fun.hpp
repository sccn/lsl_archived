
//  (C) Copyright Edward Diener 2011,2012,2013
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt).

#if !defined(BOOST_TTI_DETAIL_MEM_FUN_HPP)
#define BOOST_TTI_DETAIL_MEM_FUN_HPP

#include <lslboost/config.hpp>
#include <lslboost/function_types/is_member_function_pointer.hpp>
#include <lslboost/function_types/property_tags.hpp>
#include <lslboost/mpl/and.hpp>
#include <lslboost/mpl/logical.hpp>
#include <lslboost/mpl/assert.hpp>
#include <lslboost/mpl/bool.hpp>
#include <lslboost/mpl/eval_if.hpp>
#include <lslboost/mpl/vector.hpp>
#include <lslboost/preprocessor/cat.hpp>
#include <lslboost/type_traits/detail/yes_no_type.hpp>
#include <lslboost/type_traits/is_class.hpp>
#include <lslboost/type_traits/is_same.hpp>
#include <lslboost/type_traits/remove_const.hpp>
#include <lslboost/tti/detail/dcomp_mem_fun.hpp>
#include <lslboost/tti/detail/ddeftype.hpp>
#include <lslboost/tti/detail/dnullptr.hpp>
#include <lslboost/tti/detail/dptmf.hpp>
#include <lslboost/tti/gen/namespace_gen.hpp>

#if defined(__SUNPRO_CC)

#define BOOST_TTI_DETAIL_TRAIT_HAS_TYPES_MEMBER_FUNCTION(trait,name) \
  template<class BOOST_TTI_DETAIL_TP_PMEMF,class BOOST_TTI_DETAIL_TP_C> \
  struct BOOST_PP_CAT(trait,_detail_types) \
    { \
    template<BOOST_TTI_DETAIL_TP_PMEMF> \
    struct helper {}; \
    \
    template<class BOOST_TTI_DETAIL_TP_EC> \
    static ::lslboost::type_traits::yes_type chkt(helper<&BOOST_TTI_DETAIL_TP_EC::name> *); \
    \
    template<class BOOST_TTI_DETAIL_TP_EC> \
    static ::lslboost::type_traits::no_type chkt(...); \
    \
    typedef lslboost::mpl::bool_<sizeof(chkt<BOOST_TTI_DETAIL_TP_C>(BOOST_TTI_DETAIL_NULLPTR))==sizeof(::lslboost::type_traits::yes_type)> type; \
    }; \
/**/

#else

#define BOOST_TTI_DETAIL_TRAIT_HAS_TYPES_MEMBER_FUNCTION(trait,name) \
  template<class BOOST_TTI_DETAIL_TP_PMEMF,class BOOST_TTI_DETAIL_TP_C> \
  struct BOOST_PP_CAT(trait,_detail_types) \
    { \
    template<BOOST_TTI_DETAIL_TP_PMEMF> \
    struct helper; \
    \
    template<class BOOST_TTI_DETAIL_TP_EC> \
    static ::lslboost::type_traits::yes_type chkt(helper<&BOOST_TTI_DETAIL_TP_EC::name> *); \
    \
    template<class BOOST_TTI_DETAIL_TP_EC> \
    static ::lslboost::type_traits::no_type chkt(...); \
    \
    typedef lslboost::mpl::bool_<sizeof(chkt<BOOST_TTI_DETAIL_TP_C>(BOOST_TTI_DETAIL_NULLPTR))==sizeof(::lslboost::type_traits::yes_type)> type; \
    }; \
/**/

#endif

#define BOOST_TTI_DETAIL_TRAIT_CTMF_INVOKE(trait,name) \
  BOOST_TTI_DETAIL_TRAIT_HAS_TYPES_MEMBER_FUNCTION(trait,name) \
  template<class BOOST_TTI_DETAIL_TP_T,class BOOST_TTI_DETAIL_TP_R,class BOOST_TTI_DETAIL_TP_FS,class BOOST_TTI_DETAIL_TP_TAG> \
  struct BOOST_PP_CAT(trait,_detail_ctmf_invoke) : \
    BOOST_PP_CAT(trait,_detail_types) \
      < \
      typename BOOST_TTI_NAMESPACE::detail::ptmf_seq<BOOST_TTI_DETAIL_TP_T,BOOST_TTI_DETAIL_TP_R,BOOST_TTI_DETAIL_TP_FS,BOOST_TTI_DETAIL_TP_TAG>::type, \
      BOOST_TTI_DETAIL_TP_T \
      > \
    { \
    }; \
/**/

#define BOOST_TTI_DETAIL_TRAIT_HAS_CALL_TYPES_MEMBER_FUNCTION(trait,name) \
  BOOST_TTI_DETAIL_TRAIT_CTMF_INVOKE(trait,name) \
  template<class BOOST_TTI_DETAIL_TP_T,class BOOST_TTI_DETAIL_TP_R,class BOOST_TTI_DETAIL_TP_FS,class BOOST_TTI_DETAIL_TP_TAG> \
  struct BOOST_PP_CAT(trait,_detail_call_types) : \
	lslboost::mpl::eval_if \
		< \
 		lslboost::is_class<BOOST_TTI_DETAIL_TP_T>, \
 		BOOST_PP_CAT(trait,_detail_ctmf_invoke) \
 			< \
 			BOOST_TTI_DETAIL_TP_T, \
 			BOOST_TTI_DETAIL_TP_R, \
 			BOOST_TTI_DETAIL_TP_FS, \
 			BOOST_TTI_DETAIL_TP_TAG \
 			>, \
 		lslboost::mpl::false_ \
		> \
    { \
    }; \
/**/

#define BOOST_TTI_DETAIL_TRAIT_CHECK_HAS_COMP_MEMBER_FUNCTION(trait,name) \
  BOOST_TTI_DETAIL_TRAIT_HAS_COMP_MEMBER_FUNCTION(trait,name) \
  template<class BOOST_TTI_DETAIL_TP_T> \
  struct BOOST_PP_CAT(trait,_detail_check_comp) : \
    BOOST_PP_CAT(trait,_detail_hcmf)<BOOST_TTI_DETAIL_TP_T> \
    { \
    BOOST_MPL_ASSERT((lslboost::function_types::is_member_function_pointer<BOOST_TTI_DETAIL_TP_T>)); \
    }; \
/**/

#define BOOST_TTI_DETAIL_TRAIT_HAS_MEMBER_FUNCTION(trait,name) \
  BOOST_TTI_DETAIL_TRAIT_HAS_CALL_TYPES_MEMBER_FUNCTION(trait,name) \
  BOOST_TTI_DETAIL_TRAIT_CHECK_HAS_COMP_MEMBER_FUNCTION(trait,name) \
  template<class BOOST_TTI_DETAIL_TP_T,class BOOST_TTI_DETAIL_TP_R,class BOOST_TTI_DETAIL_TP_FS,class BOOST_TTI_DETAIL_TP_TAG> \
  struct BOOST_PP_CAT(trait,_detail_hmf) : \
    lslboost::mpl::eval_if \
      < \
      lslboost::mpl::and_ \
        < \
        lslboost::is_same<BOOST_TTI_DETAIL_TP_R,BOOST_TTI_NAMESPACE::detail::deftype>, \
        lslboost::is_same<BOOST_TTI_DETAIL_TP_FS,lslboost::mpl::vector<> >, \
        lslboost::is_same<BOOST_TTI_DETAIL_TP_TAG,lslboost::function_types::null_tag> \
        >, \
      BOOST_PP_CAT(trait,_detail_check_comp)<BOOST_TTI_DETAIL_TP_T>, \
      BOOST_PP_CAT(trait,_detail_call_types)<BOOST_TTI_DETAIL_TP_T,BOOST_TTI_DETAIL_TP_R,BOOST_TTI_DETAIL_TP_FS,BOOST_TTI_DETAIL_TP_TAG> \
      > \
    { \
    }; \
/**/

#endif // BOOST_TTI_DETAIL_MEM_FUN_HPP
