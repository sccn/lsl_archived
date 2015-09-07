/*
 [auto_generated]
 lslboost/numeric/odeint.hpp

 [begin_description]
 Forward include for odeint. Includes nearly everything.
 [end_description]

 Copyright 2009-2013 Karsten Ahnert
 Copyright 2010-2013 Mario Mulansky

 Distributed under the Boost Software License, Version 1.0.
 (See accompanying file LICENSE_1_0.txt or
 copy at http://www.lslboost.org/LICENSE_1_0.txt)
 */


#ifndef BOOST_NUMERIC_ODEINT_HPP_INCLUDED
#define BOOST_NUMERIC_ODEINT_HPP_INCLUDED

#include <lslboost/numeric/odeint/version.hpp>
#include <lslboost/numeric/odeint/config.hpp>

// start with ublas wrapper because we need its specializations before including state_wrapper.hpp
#include <lslboost/numeric/odeint/util/ublas_wrapper.hpp>

#include <lslboost/numeric/odeint/stepper/euler.hpp>
#include <lslboost/numeric/odeint/stepper/runge_kutta4_classic.hpp>
#include <lslboost/numeric/odeint/stepper/runge_kutta4.hpp>
#include <lslboost/numeric/odeint/stepper/runge_kutta_cash_karp54.hpp>
#include <lslboost/numeric/odeint/stepper/runge_kutta_cash_karp54_classic.hpp>
#include <lslboost/numeric/odeint/stepper/runge_kutta_dopri5.hpp>
#include <lslboost/numeric/odeint/stepper/runge_kutta_fehlberg78.hpp>

#include <lslboost/numeric/odeint/stepper/controlled_runge_kutta.hpp>

#include <lslboost/numeric/odeint/stepper/dense_output_runge_kutta.hpp>

#include <lslboost/numeric/odeint/stepper/bulirsch_stoer.hpp>

#ifndef __CUDACC__
/* Bulirsch Stoer with Dense Output does not compile with nvcc
 * because of the binomial library used there which relies on unsupported SSE functions
 */
#include <lslboost/numeric/odeint/stepper/bulirsch_stoer_dense_out.hpp>
#endif

#include <lslboost/numeric/odeint/stepper/symplectic_euler.hpp>
#include <lslboost/numeric/odeint/stepper/symplectic_rkn_sb3a_mclachlan.hpp>
#include <lslboost/numeric/odeint/stepper/velocity_verlet.hpp>

#include <lslboost/numeric/odeint/stepper/adams_bashforth_moulton.hpp>

#include <lslboost/numeric/odeint/stepper/implicit_euler.hpp>
#include <lslboost/numeric/odeint/stepper/rosenbrock4.hpp>
#include <lslboost/numeric/odeint/stepper/rosenbrock4_controller.hpp>
#include <lslboost/numeric/odeint/stepper/rosenbrock4_dense_output.hpp>

#include <lslboost/numeric/odeint/algebra/algebra_dispatcher.hpp>
#include <lslboost/numeric/odeint/algebra/multi_array_algebra.hpp>
#include <lslboost/numeric/odeint/util/multi_array_adaption.hpp>
/*
 * Including this algebra slows down the compilation time
 */
// #include <lslboost/numeric/odeint/algebra/fusion_algebra.hpp>

#include <lslboost/numeric/odeint/integrate/integrate.hpp>
#include <lslboost/numeric/odeint/integrate/integrate_adaptive.hpp>
#include <lslboost/numeric/odeint/integrate/integrate_const.hpp>
#include <lslboost/numeric/odeint/integrate/integrate_n_steps.hpp>
#include <lslboost/numeric/odeint/integrate/integrate_times.hpp>

#include <lslboost/numeric/odeint/integrate/observer_collection.hpp>

#include <lslboost/numeric/odeint/stepper/generation.hpp>

#include <lslboost/numeric/odeint/iterator/adaptive_iterator.hpp>
#include <lslboost/numeric/odeint/iterator/adaptive_time_iterator.hpp>
#include <lslboost/numeric/odeint/iterator/const_step_iterator.hpp>
#include <lslboost/numeric/odeint/iterator/const_step_time_iterator.hpp>


#endif // BOOST_NUMERIC_ODEINT_HPP_INCLUDED
