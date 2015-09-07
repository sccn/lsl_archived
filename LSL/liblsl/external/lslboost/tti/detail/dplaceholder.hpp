
//  (C) Copyright Edward Diener 2012,2013
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt).

#if !defined(BOOST_TTI_DETAIL_PLACEHOLDER_HPP)
#define BOOST_TTI_DETAIL_PLACEHOLDER_HPP

#include <lslboost/mpl/lambda.hpp>
#include <lslboost/mpl/not.hpp>
#include <lslboost/type_traits/is_same.hpp>

namespace lslboost
  {
  namespace tti
    {
    namespace detail
      {
      template <class BOOST_TTI_DETAIL_TP_T>
      struct is_placeholder_expression :
        lslboost::mpl::not_
          <
          lslboost::is_same
            <
            typename lslboost::mpl::lambda<BOOST_TTI_DETAIL_TP_T>::type,
            BOOST_TTI_DETAIL_TP_T
            >
          >
        {
        };
      }
    }
  }
  
#endif // BOOST_TTI_DETAIL_PLACEHOLDER_HPP
