/*
  [auto_generated]
  lslboost/numeric/odeint/algebra/fusion_algebra_dispatcher.hpp

  [begin_description]
  tba.
  [end_description]

  Copyright 2013 Karsten Ahnert
  Copyright 2013 Mario Mulansky

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or
  copy at http://www.lslboost.org/LICENSE_1_0.txt)
*/


#ifndef BOOST_NUMERIC_ODEINT_ALGEBRA_FUSION_ALGEBRA_DISPATCHER_HPP_DEFINED
#define BOOST_NUMERIC_ODEINT_ALGEBRA_FUSION_ALGEBRA_DISPATCHER_HPP_DEFINED

#include <lslboost/numeric/odeint/algebra/fusion_algebra.hpp>
#include <lslboost/numeric/odeint/algebra/algebra_dispatcher.hpp>

#include <lslboost/utility/enable_if.hpp>
#include <lslboost/fusion/include/is_sequence.hpp>



namespace lslboost {
namespace numeric {
namespace odeint {

// specialization for fusion sequences
template< class FusionSequence >
struct algebra_dispatcher_sfinae< FusionSequence ,
                           typename lslboost::enable_if<
                               typename lslboost::fusion::traits::is_sequence< FusionSequence >::type >::type >
{
    typedef fusion_algebra algebra_type;
};


} // namespace odeint
} // namespace numeric
} // namespace lslboost


#endif // BOOST_NUMERIC_ODEINT_ALGEBRA_FUSION_ALGEBRA_DISPATCHER_HPP_DEFINED
