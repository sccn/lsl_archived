/* lslboost random.hpp header file
 *
 * Copyright Jens Maurer 2000-2001
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.lslboost.org/LICENSE_1_0.txt)
 *
 * See http://www.lslboost.org/libs/random for documentation.
 *
 * $Id: random.hpp 71018 2011-04-05 21:27:52Z steven_watanabe $
 *
 * Revision history
 *  2000-02-18  portability fixes (thanks to Beman Dawes)
 *  2000-02-21  shuffle_output, inversive_congruential_schrage,
 *              generator_iterator, uniform_smallint
 *  2000-02-23  generic modulus arithmetic helper, removed *_schrage classes,
 *              implemented Streamable and EqualityComparable concepts for 
 *              generators, added Bernoulli distribution and Box-Muller
 *              transform
 *  2000-03-01  cauchy, lognormal, triangle distributions; fixed 
 *              uniform_smallint; renamed gaussian to normal distribution
 *  2000-03-05  implemented iterator syntax for distribution functions
 *  2000-04-21  removed some optimizations for better BCC/MSVC compatibility
 *  2000-05-10  adapted to BCC and MSVC
 *  2000-06-13  incorporated review results
 *  2000-07-06  moved basic templates from namespace detail to random
 *  2000-09-23  warning removals and int64 fixes (Ed Brey)
 *  2000-09-24  added lagged_fibonacci generator (Matthias Troyer)
 *  2001-02-18  moved to individual header files
 */

#ifndef BOOST_RANDOM_HPP
#define BOOST_RANDOM_HPP

// generators
#include <lslboost/random/additive_combine.hpp>
#include <lslboost/random/discard_block.hpp>
#include <lslboost/random/independent_bits.hpp>
#include <lslboost/random/inversive_congruential.hpp>
#include <lslboost/random/lagged_fibonacci.hpp>
#include <lslboost/random/linear_congruential.hpp>
#include <lslboost/random/linear_feedback_shift.hpp>
#include <lslboost/random/mersenne_twister.hpp>
#include <lslboost/random/ranlux.hpp>
#include <lslboost/random/shuffle_order.hpp>
#include <lslboost/random/shuffle_output.hpp>
#include <lslboost/random/subtract_with_carry.hpp>
#include <lslboost/random/taus88.hpp>
#include <lslboost/random/xor_combine.hpp>

// misc
#include <lslboost/random/generate_canonical.hpp>
#include <lslboost/random/seed_seq.hpp>
#include <lslboost/random/random_number_generator.hpp>
#include <lslboost/random/variate_generator.hpp>

// distributions
#include <lslboost/random/bernoulli_distribution.hpp>
#include <lslboost/random/binomial_distribution.hpp>
#include <lslboost/random/cauchy_distribution.hpp>
#include <lslboost/random/chi_squared_distribution.hpp>
#include <lslboost/random/discrete_distribution.hpp>
#include <lslboost/random/exponential_distribution.hpp>
#include <lslboost/random/extreme_value_distribution.hpp>
#include <lslboost/random/fisher_f_distribution.hpp>
#include <lslboost/random/gamma_distribution.hpp>
#include <lslboost/random/geometric_distribution.hpp>
#include <lslboost/random/lognormal_distribution.hpp>
#include <lslboost/random/negative_binomial_distribution.hpp>
#include <lslboost/random/normal_distribution.hpp>
#include <lslboost/random/piecewise_constant_distribution.hpp>
#include <lslboost/random/piecewise_linear_distribution.hpp>
#include <lslboost/random/poisson_distribution.hpp>
#include <lslboost/random/student_t_distribution.hpp>
#include <lslboost/random/triangle_distribution.hpp>
#include <lslboost/random/uniform_01.hpp>
#include <lslboost/random/uniform_int.hpp>
#include <lslboost/random/uniform_int_distribution.hpp>
#include <lslboost/random/uniform_on_sphere.hpp>
#include <lslboost/random/uniform_real.hpp>
#include <lslboost/random/uniform_real_distribution.hpp>
#include <lslboost/random/uniform_smallint.hpp>
#include <lslboost/random/weibull_distribution.hpp>

#endif // BOOST_RANDOM_HPP
