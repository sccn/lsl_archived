/*
  [auto_generated]
  lslboost/numeric/odeint/external/compute/compute_algebra_dispatcher.hpp

  [begin_description]
  algebra_dispatcher specialization for Boost.Compute
  [end_description]

  Copyright 2009-2013 Karsten Ahnert
  Copyright 2009-2013 Mario Mulansky

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or
  copy at http://www.lslboost.org/LICENSE_1_0.txt)
*/


#ifndef BOOST_NUMERIC_ODEINT_EXTERNAL_COMPUTE_COMPUTE_ALGEBRA_DISPATCHER_HPP_DEFINED
#define BOOST_NUMERIC_ODEINT_EXTERNAL_COMPUTE_COMPUTE_ALGEBRA_DISPATCHER_HPP_DEFINED

#include <lslboost/compute/container/vector.hpp>

#include <lslboost/numeric/odeint/external/compute/compute_algebra.hpp>
#include <lslboost/numeric/odeint/algebra/algebra_dispatcher.hpp>

namespace lslboost {
namespace numeric {
namespace odeint {

// specialization for Boost.Compute vector
template< class T , class A >
struct algebra_dispatcher< lslboost::compute::vector< T , A > >
{
    typedef compute_algebra algebra_type;
};

} // namespace odeint
} // namespace numeric
} // namespace lslboost

#endif // BOOST_NUMERIC_ODEINT_EXTERNAL_COMPUTE_COMPUTE_ALGEBRA_DISPATCHER_HPP_DEFINED
