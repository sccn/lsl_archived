
//  (C) Copyright Edward Diener 2011,2012,2013
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt).

#if !defined(BOOST_TTI_DETAIL_COMP_MEM_FUN_HPP)
#define BOOST_TTI_DETAIL_COMP_MEM_FUN_HPP

#include <lslboost/config.hpp>
#include <lslboost/mpl/bool.hpp>
#include <lslboost/preprocessor/cat.hpp>
#include <lslboost/tti/detail/dftclass.hpp>
#include <lslboost/tti/detail/dnullptr.hpp>
#include <lslboost/tti/gen/namespace_gen.hpp>
#include <lslboost/type_traits/remove_const.hpp>
#include <lslboost/type_traits/detail/yes_no_type.hpp>

#if defined(__SUNPRO_CC)

#define BOOST_TTI_DETAIL_TRAIT_HAS_COMP_MEMBER_FUNCTION(trait,name) \
  template<class BOOST_TTI_DETAIL_TP_T> \
  struct BOOST_PP_CAT(trait,_detail_hcmf) \
    { \
    template<class BOOST_TTI_DETAIL_TP_F> \
    struct cl_type : \
      lslboost::remove_const \
        < \
        typename BOOST_TTI_NAMESPACE::detail::class_type<BOOST_TTI_DETAIL_TP_F>::type \
        > \
      { \
      }; \
    \
    template<BOOST_TTI_DETAIL_TP_T> \
    struct helper {}; \
    \
    template<class BOOST_TTI_DETAIL_TP_U> \
    static ::lslboost::type_traits::yes_type chkt(helper<&BOOST_TTI_DETAIL_TP_U::name> *); \
    \
    template<class BOOST_TTI_DETAIL_TP_U> \
    static ::lslboost::type_traits::no_type chkt(...); \
    \
    typedef lslboost::mpl::bool_<sizeof(chkt<typename cl_type<BOOST_TTI_DETAIL_TP_T>::type>(BOOST_TTI_DETAIL_NULLPTR))==sizeof(::lslboost::type_traits::yes_type)> type; \
    }; \
/**/

#else

#define BOOST_TTI_DETAIL_TRAIT_HAS_COMP_MEMBER_FUNCTION(trait,name) \
  template<class BOOST_TTI_DETAIL_TP_T> \
  struct BOOST_PP_CAT(trait,_detail_hcmf) \
    { \
    template<class BOOST_TTI_DETAIL_TP_F> \
    struct cl_type : \
      lslboost::remove_const \
        < \
        typename BOOST_TTI_NAMESPACE::detail::class_type<BOOST_TTI_DETAIL_TP_F>::type \
        > \
      { \
      }; \
    \
    template<BOOST_TTI_DETAIL_TP_T> \
    struct helper; \
    \
    template<class BOOST_TTI_DETAIL_TP_U> \
    static ::lslboost::type_traits::yes_type chkt(helper<&BOOST_TTI_DETAIL_TP_U::name> *); \
    \
    template<class BOOST_TTI_DETAIL_TP_U> \
    static ::lslboost::type_traits::no_type chkt(...); \
    \
    typedef lslboost::mpl::bool_<sizeof(chkt<typename cl_type<BOOST_TTI_DETAIL_TP_T>::type>(BOOST_TTI_DETAIL_NULLPTR))==sizeof(::lslboost::type_traits::yes_type)> type; \
    }; \
/**/

#endif


#endif // BOOST_TTI_DETAIL_COMP_MEM_FUN_HPP
