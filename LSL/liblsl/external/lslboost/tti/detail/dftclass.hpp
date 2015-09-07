
//  (C) Copyright Edward Diener 2012,2013
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt).

#if !defined(BOOST_TTI_DETAIL_FTCLASS_HPP)
#define BOOST_TTI_DETAIL_FTCLASS_HPP

#include <lslboost/function_types/parameter_types.hpp>
#include <lslboost/mpl/at.hpp>
#include <lslboost/mpl/identity.hpp>
#include <lslboost/mpl/int.hpp>
#include <lslboost/mpl/quote.hpp>

namespace lslboost
  {
  namespace tti
    {
    namespace detail
      {
      template<class BOOST_TTI_DETAIL_TP_F>
      struct class_type :
          lslboost::mpl::at
            <
            typename
            lslboost::function_types::parameter_types
              <
              BOOST_TTI_DETAIL_TP_F,
              lslboost::mpl::quote1
                <
                lslboost::mpl::identity
                >
              >::type,
              lslboost::mpl::int_<0>
            >
        {
        };
      }
    }
  }
  
#endif // BOOST_TTI_DETAIL_FTCLASS_HPP
