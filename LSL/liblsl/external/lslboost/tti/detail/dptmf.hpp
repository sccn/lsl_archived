
//  (C) Copyright Edward Diener 2011,2012,2013
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt).

#if !defined(BOOST_TTI_DETAIL_PTMF_HPP)
#define BOOST_TTI_DETAIL_PTMF_HPP

#include <lslboost/config.hpp>
#include <lslboost/mpl/push_front.hpp>
#include <lslboost/function_types/member_function_pointer.hpp>

namespace lslboost
  {
  namespace tti
    {
    namespace detail
      {
      template
        <
        class BOOST_TTI_DETAIL_TP_T,
        class BOOST_TTI_DETAIL_TP_R,
        class BOOST_TTI_DETAIL_TP_FS,
        class BOOST_TTI_DETAIL_TP_TAG
        >
      struct ptmf_seq
        {
        typedef typename
        lslboost::function_types::member_function_pointer
          <
          typename
          lslboost::mpl::push_front
            <
            typename
            lslboost::mpl::push_front<BOOST_TTI_DETAIL_TP_FS,BOOST_TTI_DETAIL_TP_T>::type,
            BOOST_TTI_DETAIL_TP_R
            >::type,
          BOOST_TTI_DETAIL_TP_TAG
          >::type type;
        };
      }
    }
  }
  
#endif // BOOST_TTI_DETAIL_PTMF_HPP
