
//  (C) Copyright Edward Diener 2011,2012,2013
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt).

#if !defined(BOOST_TTI_DETAIL_TFUNCTION_HPP)
#define BOOST_TTI_DETAIL_TFUNCTION_HPP

#include <lslboost/config.hpp>
#include <lslboost/mpl/push_front.hpp>
#include <lslboost/function_types/function_type.hpp>

namespace lslboost
  {
  namespace tti
    {
    namespace detail
      {
      template
        <
        class BOOST_TTI_DETAIL_TP_R,
        class BOOST_TTI_DETAIL_TP_FS,
        class BOOST_TTI_DETAIL_TP_TAG
        >
      struct tfunction_seq
        {
        typedef typename lslboost::mpl::push_front<BOOST_TTI_DETAIL_TP_FS,BOOST_TTI_DETAIL_TP_R>::type ftseq;
        typedef typename lslboost::function_types::function_type<ftseq,BOOST_TTI_DETAIL_TP_TAG>::type type;
        };
      }
    }
  }
  
#endif // BOOST_TTI_DETAIL_TFUNCTION_HPP
